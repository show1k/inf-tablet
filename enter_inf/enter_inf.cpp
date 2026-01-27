#include "enter_inf.h"
#include <QDir>
#include <QStandardPaths>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>

enter_inf::enter_inf(QObject *parent) : QObject(parent)
{
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QDir dir(publicPath);
    if (!dir.exists("Log_inf"))
        dir.mkdir("Log_inf");
    QString PathInf = publicPath + "/Log_inf/";
    HomePath = PathInf;
    connect(this, &enter_inf::inf_ready, this, &enter_inf::send_data);
}

enter_inf::~enter_inf()
{
    qDebug() << "finish enterInf";
}

void enter_inf::send_data()
{
    emit sendData(DataEljur, AdminCab, About, YandexToken);
}

void enter_inf::get_inf_eljur(const QStringList Inf)
{
     DataEljur = Inf; // 0 = url; 1 = login; 2 = password; 3 = devkey; 4 = vendor

//    QString Url = "https://lyceum-fa.eljur.ru";
//    QString Login = "lyceum-fa_api";
//    QString Password = "9093ce33241a0fe2fffa323f62719af8";
//    QString DevKey = "0805a97401448899a9d160f238e30ae8";
//    QString Vendor = "lyceum-fa";

    if (!save)
    {
        save = connect(this, &enter_inf::infEljur_Correct,this, [=]()
        {
         save_inf("eljur", Inf);
         yandex_disk();
         disconnect(save);
        });
    }

    QString fullUrl = DataEljur[0] + "/api/getrules?&vendor=" + DataEljur[4] + "&devkey=" + DataEljur[3] + "&login=" + DataEljur[1] + "&password=" + DataEljur[2];
    qDebug() << Inf << fullUrl;
    check_correct_inf_eljur(fullUrl);
}

void enter_inf::get_inf_yanDisk(const QStringList Inf)
{
    QString Token = Inf[0];

    //QString Token = "y0__xCFr7HsBxjblgMg5u3DqRKwRRZuqKnmlTXw1AvkAqQo3D8EMQ";

    if (!save)
    {
        save = connect(this, &enter_inf::infYanDisk_Correct,this, [=]()
        {
        save_inf("YandexDisk", Inf);
        about_school();
        disconnect(save);
        });
    }
    qDebug() << Inf[0];
    check_correct_inf_YandexDisk(Token);

}

void enter_inf::get_inf_aboutSch(const QStringList Inf)
{
    QStringList adminCabs = Inf[0].split(" ");
    QString avgNumLes =  Inf[1];
    QString avgStartLes = Inf[2];
    QStringList s = {avgNumLes, avgStartLes};

    About = s;
    AdminCab = adminCabs;

    qDebug() << AdminCab << avgNumLes << avgStartLes;
    save_inf("About_School", Inf);
    emit infAbout_ready();
    emit inf_ready();
}

void enter_inf::eljur()
{
    QString PathEljur =HomePath;
    QDir dir(PathEljur);
    if (!dir.exists("eljur"))
    {
        emit open_window_eljur();
    }
    else
    {
        QStringList inf = read_file(PathEljur + "eljur");
        DataEljur = inf;
        qDebug() << DataEljur;
        yandex_disk();
    }
}

void enter_inf::yandex_disk()
{
    QString PathYanDisk = HomePath;
    QDir dir(PathYanDisk);
    if (!dir.exists("YandexDisk"))
    {
        emit open_window_yandex_disk();
    }
    else
    {
        QStringList inf = read_file(PathYanDisk + "YandexDisk");
        YandexToken = inf[0];
        about_school();
    }
}

void enter_inf::about_school()
{
    QString PathAboutSch = HomePath;
    QDir dir(PathAboutSch);
    if (!dir.exists("About_School"))
    {
        emit open_window_about_school();
    }
    else
    {
        QStringList inf = read_file(PathAboutSch + "About_School");
        QStringList adminCabs = inf[0].split(" ");
        QString avgNumLes =  inf[1];
        QString avgStartLes = inf[2];
        QStringList s = {avgNumLes, avgStartLes};

        AdminCab = adminCabs;
        About = s;

        qDebug() << AdminCab << avgNumLes << avgStartLes;

        emit inf_ready();

    }
}

QStringList enter_inf::read_file(const QString &Path)
{
    QFile file(Path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Не удалось открыть файл для чтения:" << Path;
        return {};
    }

    QStringList result;
    QTextStream in(&file);
    in.setCodec("UTF-8");

    while (!in.atEnd()) {
        QString line = in.readLine();
        result.append(line);
    }

    file.close();
    return result;
}

void enter_inf::save_inf(const QString& name, const QStringList& data)
{
    QString Path = HomePath + name;
    QFile file(Path);

    if(file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream out(&file);
        for (const QString &line : data)
            out << line << "\n";

        file.close();
        qDebug() << "Данные сохранены в:" << Path;
    }
    else
        qWarning() << "Не удалось открыть файл для записи:" << Path;
}

void enter_inf::check_correct_inf_eljur(const QString& Url)
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkRequest request((QUrl(Url)));

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        if (reply->error() == QNetworkReply::NoError)
        {
            QByteArray data = reply->readAll();
            qDebug() << "Ответ от сервера:" << data;
            QJsonDocument doc = QJsonDocument::fromJson(data);
            QJsonObject root = doc.object();
            if (root.contains("response"))
            {
                QJsonObject response = root["response"].toObject();
                if(response.contains("state"))
                {
                     int state = response["state"].toInt();
                     if (state == 200)
                     {
                        //qDebug() << "все гуд";
                        emit infEljur_Correct();
                     }
                     else
                     {
                         emit infEljur_InCorrect();
                         disconnect(save);
                     }
                }
                else
                {
                    emit infEljur_InCorrect();
                    disconnect(save);
                }
            }
            else
            {
                emit infEljur_InCorrect();
                disconnect(save);
            }
        }
        else
        {
            qDebug() << "Ошибка сети:" << reply->errorString();
            emit infEljur_InCorrect();
            disconnect(save);
        }

        reply->deleteLater();
        manager->deleteLater();
    });

}

void enter_inf::check_correct_inf_YandexDisk(const QString &Token)
{
    QString Url_YD = "https://cloud-api.yandex.net/v1/disk";
    QUrl url(Url_YD);
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Authorization", ("OAuth " + Token).toUtf8());

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        if (reply->error() == QNetworkReply::NoError) {
            //QByteArray data = reply->readAll();
            //qDebug() << "Ответ от сервера:" << data;
            emit infYanDisk_Correct();
        }
        else
        {
             qDebug() << "Ошибка сети:" << reply->errorString();
             emit infYanDisk_InCorrect();
             disconnect(save);
        }

        reply->deleteLater();
        manager->deleteLater();
    });
}
