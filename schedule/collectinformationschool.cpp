#include "collectinformationschool.h"
#include <QDir>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QStandardPaths>

collectInformationSchool::collectInformationSchool(internetConnection* ptrIntcon, QObject *parent) : QObject(parent), intConnect(ptrIntcon)
{
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QDir dir(publicPath);
    if (!dir.exists("informationOfSchool"))
        dir.mkdir("informationOfSchool");
    manager = new QNetworkAccessManager(this);
    lyceum = new School(this);
    inf = false;
    connect(intConnect, &internetConnection::internet_connected, this, &collectInformationSchool::request_classes);
    connect(intConnect, &internetConnection::internet_disconnected, this, &collectInformationSchool::read_information);
}

QStringList collectInformationSchool::classes;
QStringList collectInformationSchool::rooms;

QStringList collectInformationSchool::return_classes()
{
    return classes;
}

QStringList collectInformationSchool::return_rooms()
{
    return rooms;
}

void collectInformationSchool::check_infReady()
{
    if (inf)
        emit infReady();
}

void collectInformationSchool::request_classes()
{
    QNetworkRequest  request;
    QUrl urlClasses = lyceum->get_rules_url();
    request.setUrl(urlClasses);

    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [=]
    {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();
        get_classses(root);
        reply->deleteLater();
    });
}

void collectInformationSchool::request_rooms()
{
    pendingRequests = classes.size() * 7; // кол во дней
    for (int i = 0; i < classes.size(); i++)
    {
        QString clas = classes.at(i);
        QStringList urls = lyceum->get_schedule_urls_for_week(clas);
        for (int j = 0; j < urls.size(); j++)
        {
            QNetworkRequest request;
            QUrl scheduleUrl = urls.at(j);
            request.setUrl(scheduleUrl);

            QString date = QDate::fromString(urls.at(j).split("days=").at(1).split("&").at(0), "yyyyMMdd").toString("yyyyMMdd");

            QNetworkReply *reply = manager->get(request);
            connect(reply, &QNetworkReply::finished, this, [=]
            {
                QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
                QJsonObject root = doc.object();
                QTimer::singleShot(0, this, [this, root, date]
                {
                    get_roomsSet(root, date);
                });
                reply->deleteLater();
            });
        }
    }
}

void collectInformationSchool::get_roomsSet(const QJsonObject &response, const QString &date)
{
    QJsonObject jv1 = response.value("response").toObject();
    QJsonObject jv2 = jv1.value("result").toObject();
    QJsonObject jv3 = jv2.value("days").toObject();
    QJsonObject jv4 = jv3.value(date).toObject();
    QJsonArray items = jv4.value("items").toArray();

    for (int i = 0; i < items.size(); i++)
    {
        QJsonObject itemObj = items.at(i).toObject();
        QString room = itemObj.value("room").toString();
        if (!room.isEmpty())
            roomsSet.insert(room);
    }
    pendingRequests--;
    if (pendingRequests == 0)
        get_rooms();
}

void collectInformationSchool::get_classses(const QJsonObject &response)
{
    QJsonObject jv1 = response.value("response").toObject();
    QJsonObject jv2 = jv1.value("result").toObject();
    QJsonObject jv3 = jv2.value("relations").toObject();
    QJsonObject groups = jv3.value("groups").toObject();
    classes = groups.keys();

    qDebug() << "classes:" << classes;
    QTimer::singleShot(0, this, &collectInformationSchool::save_classes);
    QTimer::singleShot(0, this, &collectInformationSchool::request_rooms);
}

void collectInformationSchool::get_rooms()
{
    rooms = roomsSet.toList();
    rooms.sort();
    qDebug() << "rooms:"<< rooms;
    inf = true;
    emit infReady();
    QTimer::singleShot(0, this, &collectInformationSchool::save_rooms);
}

void collectInformationSchool::save_classes()
{
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QString filePath = publicPath + "/informationOfSchool/classes.txt";
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qDebug() << "Ошибка: Не удалось открыть файл для записи классов по пути:" << filePath;
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8"); // Установка кодировки UTF-8 для поддержки русского текста
    for (int i = 0; i < classes.size(); i++)
    {
        const QString& className = classes.at(i);
        out << className << "\n";
    }
    file.close();
    qDebug() << "Список классов успешно сохранен в файл по пути:" << filePath;
}

void collectInformationSchool::save_rooms()
{
   QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
   QString publicPath = homePath + "/Public/DataInfTablet";
   QString filePath = publicPath + "/informationOfSchool/rooms.txt";
   QFile file(filePath);
   if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
   {
       qDebug() << "Ошибка: Не удалось открыть файл для записи кабинетов по пути:" << filePath;
       return;
   }

   QTextStream out(&file);
   out.setCodec("UTF-8"); // Установка кодировки UTF-8 для поддержки русского текста
   for (int i = 0; i < rooms.size(); i++)
   {
       const QString& roomName = rooms.at(i);

       out << roomName << "\n";
   }
   file.close();
   qDebug() << "Список кабинетов успешно сохранен в файл по пути:" << filePath;
}

void collectInformationSchool::read_information()
{
    // Чтение классов
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QString classesFilePath = publicPath + "/informationOfSchool/classes.txt";
    QFile classesFile(classesFilePath);
    if (!classesFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Ошибка: Не удалось открыть файл для чтения классов по пути:" << classesFilePath;
        emit noData();
        return;
    }

    QTextStream classesIn(&classesFile);
    classesIn.setCodec("UTF-8"); // Установка кодировки UTF-8 для поддержки русского текста
    while (!classesIn.atEnd())
    {
        QString className = classesIn.readLine().trimmed();
        if (!className.isEmpty())
            classes.append(className);
    }
    classesFile.close();
    qDebug() << "Список классов успешно прочитан из файла по пути:" << classesFilePath << classes;

    // Чтение кабинетов
    QString roomsFilePath = publicPath + "/informationOfSchool/rooms.txt";
    QFile roomsFile(roomsFilePath);
    if (!roomsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Ошибка: Не удалось открыть файл для чтения кабинетов по пути:" << roomsFilePath;
        emit noData();
        return;
    }

    QTextStream roomsIn(&roomsFile);
    roomsIn.setCodec("UTF-8"); // Установка кодировки UTF-8 для поддержки русского текста
    while (!roomsIn.atEnd())
    {
        QString roomName = roomsIn.readLine().trimmed();
        if (!roomName.isEmpty())
            rooms.append(roomName);
    }
    roomsFile.close();
    qDebug() << "Список кабинетов успешно прочитан из файла по пути:" << roomsFilePath << rooms;
    inf = true;
    emit infReady();
}

