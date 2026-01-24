#include "yandeximageteachersdownloader.h"
#include "app/globalVar.h"
#include <QThread>
#include <QDir>
#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonArray>
#include <QStandardPaths>

YandexImageTeachersDownloader::YandexImageTeachersDownloader(QObject *parent) : QObject(parent)
{
    yandextoken = "y0__xCFr7HsBxjblgMg5u3DqRKwRRZuqKnmlTXw1AvkAqQo3D8EMQ";
    m_remainingDownloads = 0;
    manager = new QNetworkAccessManager(this);
    QThread *thread = new QThread(); // No parent

    moveToThread(thread);
    thread->start();

    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QDir dir(publicPath);
    if (!dir.exists("yandeximageteachers")) {
        dir.mkpath("yandeximageteachers");
        qDebug() << "Created directory: yandeximageteachers";
    }
}

YandexImageTeachersDownloader::~YandexImageTeachersDownloader()
{
    if (manager)
        manager->deleteLater();
}

void YandexImageTeachersDownloader::download_images(QJsonObject shedule)
{
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";

    m_remainingDownloads = 0;
    QStringList teachers_names_list = get_teachers_name(shedule);
    QStringList add_name = get_add_name();
    for (const QString& name : add_name)
        teachers_names_list.append(name);

    // Supported file extensions
    QStringList extensions = {".jpg", ".JPG", ".jpeg", ".JPEG", ".png", ".PNG"};

    for (const QString &teacher_name : teachers_names_list) {
        for (const QString &ext : extensions) {
            QString filePath = QUrl::toPercentEncoding(teacher_name + "/" + teacher_name + ext);
            QString savePath = publicPath + "/yandeximageteachers/" + teacher_name + ext;

            m_remainingDownloads++;

            QMetaObject::invokeMethod(this, "downloadImage", Qt::QueuedConnection,
                                      Q_ARG(QString, filePath),
                                      Q_ARG(QString, savePath));
        }
    }

    if (m_remainingDownloads == 0)
    {
        qDebug() << "No images to download";
        emit finished();
    }
}

void YandexImageTeachersDownloader::downloadImage(const QString &filePath, const QString &savePath)
{
    QUrl url("https://cloud-api.yandex.net/v1/disk/resources/download");
    QUrlQuery query;
    query.addQueryItem("path", filePath);
    url.setQuery(query);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", ("OAuth " + yandextoken).toUtf8());

    QNetworkReply *reply = manager->get(request);

    // New signal-slot syntax with lambda
    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleDownloadLinkReply(reply, savePath);
    });
}

void YandexImageTeachersDownloader::handleDownloadLinkReply(QNetworkReply *reply, const QString &savePath)
{
    if (reply->error() == QNetworkReply::NoError) {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode == 302) {
            QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (!redirectUrl.isEmpty()) {
                downloadFile(redirectUrl.toString(), savePath);
                reply->deleteLater();
                return;
            }
        }

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        // Debug: Log the response
       // qDebug() << "Download link response for" << savePath << ":" << jsonDoc.toJson(QJsonDocument::Compact);

        QString downloadUrl = jsonObj["href"].toString();
        if (downloadUrl.isEmpty()) {
            qWarning() << "No download URL for" << savePath;
            reply->deleteLater();
            checkAllDownloadsFinished();
            return;
        }

        // Remove old files with the same base name
        QFileInfo fileInfo(savePath);
        QString baseName = fileInfo.completeBaseName();
        QDir dir = fileInfo.dir();
        QStringList filters = {baseName + ".*"};
        QStringList oldFiles = dir.entryList(filters, QDir::Files);
        for (const QString &filename : oldFiles) {
            QFile::remove(dir.filePath(filename));
            //qDebug() << "Removed old file:" << filename;
        }

        downloadFile(downloadUrl, savePath);
    } else {
        //qWarning() << "Error downloading link for" << savePath << ":" << reply->errorString();
        checkAllDownloadsFinished();
    }
    reply->deleteLater();
}

void YandexImageTeachersDownloader::downloadFile(const QString &url, const QString &savePath)
{
    QNetworkRequest request(url);
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    request.setSslConfiguration(sslConfig);

    QNetworkReply *reply = manager->get(request);

    // New signal-slot syntax with lambda
    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleFileDownloadReply(reply, savePath);
    });
}

void YandexImageTeachersDownloader::handleFileDownloadReply(QNetworkReply *reply, const QString &savePath)
{
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode == 302) {
        QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
        if (!redirectUrl.isEmpty()) {
            downloadFile(redirectUrl.toString(), savePath);
            reply->deleteLater();
            return;
        }
    }

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray data = reply->readAll();
        if (data.isEmpty()) {
            qWarning() << "Error: Data is empty for" << savePath;
            checkAllDownloadsFinished();
            reply->deleteLater();
            return;
        }

        QString correctExtension = getCorrectExtension(data);
        QString finalSavePath = savePath;

        if (!savePath.endsWith(correctExtension, Qt::CaseInsensitive)) {
            QFileInfo fi(savePath);
            finalSavePath = fi.path() + "/" + fi.completeBaseName() + correctExtension;

            // Remove old file if it exists
            if (QFile::exists(savePath)) {
                QFile::remove(savePath);
                qDebug() << "Removed file with incorrect extension:" << savePath;
            }
        }

        QFile file(finalSavePath);
        if (file.open(QIODevice::WriteOnly)) {
            qint64 bytesWritten = file.write(data);
            file.close();
            if (bytesWritten > 0) {
                //qDebug() << "File saved:" << finalSavePath;
            } else {
                qWarning() << "Error: File is empty or not written:" << finalSavePath;
            }
        } else {
            qWarning() << "Error: Unable to open file for writing:" << finalSavePath << ":" << file.errorString();
        }
    } else {
        qWarning() << "Error downloading file for" << savePath << ":" << reply->errorString();
    }
    reply->deleteLater();
    checkAllDownloadsFinished();
}

void YandexImageTeachersDownloader::checkAllDownloadsFinished()
{
    m_remainingDownloads--;
    if (m_remainingDownloads <= 0)
    {
        qDebug() << "All downloads completed";
        emit finished();
    }
}

QStringList YandexImageTeachersDownloader::get_teachers_name(QJsonObject &shedule)
{
    QStringList teachers_names_list;
    QStringList classroomKeys = shedule.keys();
    for (const QString &classroom : classroomKeys) {
        QJsonArray cur_classroom_schedule = shedule.value(classroom).toArray();
        for (const QJsonValue &lesson_value : cur_classroom_schedule) {
            QJsonObject cur_lesson = lesson_value.toObject();
            QString teacher = cur_lesson["teacher"].toString();
            if (!teacher.isEmpty()) {
                teachers_names_list.append(teacher);
            }
        }
    }
    return teachers_names_list;
}

QString YandexImageTeachersDownloader::getCorrectExtension(const QByteArray &imageData)
{
    if (imageData.startsWith("\xFF\xD8\xFF")) {
        return ".jpg";
    } else if (imageData.startsWith("\x89PNG")) {
        return ".png";
    } else if (imageData.startsWith("GIF8")) {
        return ".gif";
    } else if (imageData.startsWith("BM")) {
        return ".bmp";
    }
    return ".jpg"; // Default
}

QStringList YandexImageTeachersDownloader::get_add_name() const
{
    return admin_cabs;
}
