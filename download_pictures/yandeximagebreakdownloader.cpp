#include "yandeximagebreakdownloader.h"
#include <QThread>
#include <QDir>
#include <QUrlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>

YandexImageBreakDownloader::YandexImageBreakDownloader(QObject *parent) : QObject(parent)
{
    yandextoken = "y0__xCFr7HsBxjblgMg5u3DqRKwRRZuqKnmlTXw1AvkAqQo3D8EMQ";
    m_remainingDownloads = 0;
    manager = new QNetworkAccessManager(this);
    QThread *thread = new QThread(); // No parent

    moveToThread(thread);
    thread->start();

    // Ensure yandeximages directory exists
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QDir dir(publicPath);
    if (!dir.exists("yandeximagesbreak")) {
        dir.mkpath("yandeximagesbreak");
        qDebug() << "Created directory: yandeximagesbreak";
    }

    // удаление старых файлов
    QString pathDir = publicPath + "/yandeximagesbreak/";
    dir.setPath(pathDir);
    QStringList oldFiles = dir.entryList(QDir::Files, QDir::NoSort);
    for (const QString &filename : oldFiles)
    {
        QString fullPath = dir.filePath(filename);
        if (QFile::exists(fullPath))
        {
            if (QFile::remove(fullPath))
            {
                qDebug() << "Удалён файл:" << filename;
            }
            else
            {
                qWarning() << "Не удалось удалить файл:" << filename;
            }
        }
    }
}

YandexImageBreakDownloader::~YandexImageBreakDownloader()
{
    // Thread is managed externally (e.g., in controller)
    if (manager)
        manager->deleteLater();
}

void YandexImageBreakDownloader::download_break_files()
{
    QUrl url("https://cloud-api.yandex.net/v1/disk/resources");
    QUrlQuery query;
    query.addQueryItem("path", "/перемены");
    url.setQuery(query);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", ("OAuth " + yandextoken).toUtf8());

    QNetworkReply *reply = manager->get(request);

    // Connect using new signal-slot syntax
    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleFileListReply(reply);
    });
}

void YandexImageBreakDownloader::handleFileListReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QString publicPath = homePath + "/Public/DataInfTablet";

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        QJsonObject embedded = jsonObj.value("_embedded").toObject();
        QJsonArray items = embedded.value("items").toArray();

        if (items.isEmpty()) {
            qWarning() << "No files found in /перемены";
            emit finished();
            reply->deleteLater();
            return;
        }

        for (const QJsonValue &item : items) {
            QJsonObject fileObj = item.toObject();
            QString filePath = fileObj["path"].toString();
            QString fileName = fileObj["name"].toString();
            QString savePath = publicPath + "/yandeximagesbreak/" + fileName;

            m_remainingDownloads++;
            // Use QueuedConnection to ensure execution in the object's thread
            QMetaObject::invokeMethod(this, "downloadFile", Qt::QueuedConnection,
                                      Q_ARG(QString, filePath),
                                      Q_ARG(QString, savePath));
        }
    } else {
        qWarning() << "Error fetching file list for /перемены:" << reply->errorString();
        emit finished();
    }
    reply->deleteLater();
}

void YandexImageBreakDownloader::downloadFile(const QString &filePath, const QString &savePath)
{
    QUrl url("https://cloud-api.yandex.net/v1/disk/resources/download");
    QUrlQuery query;
    query.addQueryItem("path", filePath);
    url.setQuery(query);

    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", ("OAuth " + yandextoken).toUtf8());

    QNetworkReply *reply = manager->get(request);

    // Connect using new signal-slot syntax
    connect(reply, &QNetworkReply::finished, this, [=]() {
        handleDownloadLinkReply(reply, savePath);
    });
}

void YandexImageBreakDownloader::handleDownloadLinkReply(QNetworkReply *reply, const QString &savePath)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (statusCode == 302)
        {
            QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (!redirectUrl.isEmpty())
            {
                // Direct download for redirect
                QNetworkRequest redirectRequest(redirectUrl);
                QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
                sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
                redirectRequest.setSslConfiguration(sslConfig);

                QNetworkReply *redirectReply = manager->get(redirectRequest);
                connect(redirectReply, &QNetworkReply::finished, this, [=]() {
                    handleFileDownloadReply(redirectReply, savePath);
                });
                reply->deleteLater();
                return;
            }
        }

        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        // Debug: Log the response
        //qDebug() << "Download link response for" << savePath << ":" << jsonDoc.toJson(QJsonDocument::Compact);

        QString downloadUrl = jsonObj["href"].toString();
        if (downloadUrl.isEmpty()) {
            qWarning() << "No download URL for" << savePath;
            reply->deleteLater();
            checkAllDownloadsFinished();
            return;
        }

        QNetworkRequest fileRequest(downloadUrl);
        QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
         // Remove VerifyNone for stricter SSL check
         fileRequest.setSslConfiguration(sslConfig);

        QNetworkReply *fileReply = manager->get(fileRequest);
        connect(fileReply, &QNetworkReply::finished, this, [=]()
        {
            handleFileDownloadReply(fileReply, savePath);
        });
    }
    else
    {
        qWarning() << "Error downloading link for" << savePath << ":" << reply->errorString();
        checkAllDownloadsFinished();
    }
    reply->deleteLater();
}

void YandexImageBreakDownloader::handleFileDownloadReply(QNetworkReply *reply, const QString &savePath)
{
    if (reply->error() == QNetworkReply::NoError)
    {
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode == 302)
        {
            QUrl redirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
            if (!redirectUrl.isEmpty())
            {
                QNetworkRequest redirectRequest(redirectUrl);
                QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
                redirectRequest.setSslConfiguration(sslConfig);

                QNetworkReply *redirectReply = manager->get(redirectRequest);
                connect(redirectReply, &QNetworkReply::finished, this, [=]()
                {
                    handleFileDownloadReply(redirectReply, savePath);
                });
                reply->deleteLater();
                return;
            }
        }
        QByteArray data = reply->readAll();
                if (data.isEmpty()) {
                    qWarning() << "Error: Data is empty for" << savePath << "Error:" << reply->errorString();
                    // Retry or log more details
                    qDebug() << "Network error details:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()
                             << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
                    checkAllDownloadsFinished();
                    reply->deleteLater();
                    return;
                }
        QFile file(savePath);
        if (file.open(QIODevice::WriteOnly))
        {
            qint64 bytesWritten = file.write(data);
            file.close();
            if (bytesWritten > 0)
            {
                qDebug() << "File saved:" << savePath;
                savedPaths.append(savePath);
            }
            else
            {
                qWarning() << "Error: File is empty or not written:" << savePath;
            }
        }
        else
        {
            qWarning() << "Error: Unable to open file for writing:" << savePath << ":" << file.errorString();
        }
    }
    else
    {
        qWarning() << "Error downloading file for" << savePath << ":" << reply->errorString();
    }
    reply->deleteLater();
    checkAllDownloadsFinished();
}

void YandexImageBreakDownloader::checkAllDownloadsFinished()
{
    m_remainingDownloads--;
    if (m_remainingDownloads <= 0)
    {
        qDebug() << "All break files downloaded";
        emit pathReady(savedPaths);
        emit finished();
    }
}
