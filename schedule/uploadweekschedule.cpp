#include "uploadweekschedule.h"
#include <QThread>
#include <QDir>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>
#include "collectinformationschool.h"

UploadWeekSchedule::UploadWeekSchedule(QObject *parent) : QObject(parent)
{
    lyceum = new School(this);
    manager = new QNetworkAccessManager(this);
    manager2 = new QNetworkAccessManager(this);
    manager3 = new QNetworkAccessManager(this);
    pendingRequests = 0;

    classes = collectInformationSchool::return_classes();

    // Перемещаем объект в отдельный поток
    QThread *thread = new QThread();
    moveToThread(thread);
    thread->start();

    // Создаем папку schedules в текущей директории
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QDir dir(publicPath);
    if (!dir.exists("schedules"))
        dir.mkdir("schedules");
}

UploadWeekSchedule::~UploadWeekSchedule()
{
    // Очищаем ресурсы
    if (lyceum) lyceum->deleteLater();
    if (manager) manager->deleteLater();
    if (manager2) manager2->deleteLater();
    if (manager3) manager3->deleteLater();
}

QStringList UploadWeekSchedule::get_classes()
{
    QStringList classes;
    QStringList temp = collectInformationSchool::return_classes();
    QMap<QString, QString> mapClasses;
    QMap<QString, QString>::iterator iter;

    for (const QString& a : temp)
    {
        QString clas;
        for(const QChar x : a)
        {
            if (x.isDigit())
                clas += x;
        }
        iter = mapClasses.find(clas);
        if (iter == mapClasses.end())
            mapClasses[clas] = clas;
    }
    for (iter = mapClasses.begin(); iter != mapClasses.end(); iter++)
    {
        classes.append(*iter);
    }
    return classes;
}

void UploadWeekSchedule::start_upload_shedule()
{
    QStringList classesOutLes = get_classes();
    QStringList classesForReplaceLes = collectInformationSchool::return_classes();
    pendingRequests = 7 * (classes.size() + classesOutLes.size() + classesForReplaceLes.size()); // Количество классов * 7 дней

    // Инициализируем QMap для хранения расписаний по дням недели
    daySchedules.clear();
    QStringList days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    for (const QString &day : days)
        daySchedules[day] = QJsonObject();

    // Подключаем обработчик для сохранения расписания
    connect(this, &UploadWeekSchedule::classScheduleLoaded, this, &UploadWeekSchedule::save_day_schedule);

    // Запускаем запросы для всех классов
    for (int i = 0; i < classes.size(); i++)
    {
        const QString clas = classes.at(i);
        QMetaObject::invokeMethod(this, "upload_class_shedule", Qt::QueuedConnection,
                                    Q_ARG(QString, clas));
    }

    for (const QString& clas : classesOutLes)
    {
        QMetaObject::invokeMethod(this, "upload_outLessons", Qt::QueuedConnection,
                                    Q_ARG(QString, clas));
    }

    for (const QString& clas : classesForReplaceLes)
    {
        QMetaObject::invokeMethod(this, "upload_replace_lessons", Qt::QueuedConnection,
                                    Q_ARG(QString, clas));
    }
}

void UploadWeekSchedule::upload_class_shedule(const QString &clas)
{
    QStringList urls = lyceum->get_schedule_urls_for_week(clas);

    for (int i = 0; i < urls.size(); i++)
    {
        QNetworkRequest request;
        QUrl scheduleUrl = urls.at(i);
        request.setUrl(scheduleUrl);

        QNetworkReply *reply = manager->get(request);

        QString date = QDate::fromString(urls.at(i).split("days=").at(1).split("&").at(0), "yyyyMMdd").toString("yyyyMMdd");

        connect(reply, &QNetworkReply::finished, this, [=]()
        {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject root = doc.object();

            QJsonObject jv = root.value("response").toObject();
            QJsonObject jv2 = jv.value("result").toObject();
            QJsonObject jv3 = jv2.value("days").toObject();
            QJsonObject jv4 = jv3.value(date).toObject();
            QJsonArray jv5 = jv4.value("items").toArray();

            emit classScheduleLoaded(jv5, clas, date);
            reply->deleteLater();
            pendingRequests--;

            if (pendingRequests == 0)
            {
                qDebug() << "All schedules downloaded and saved";
                emit allSchedulesDownloaded();
            }
        });

        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, [=]()
        {
            qWarning() << "Error for class" << clas << "on date" << date << ":" << reply->errorString();
            emit classScheduleLoaded(QJsonArray(), clas, date);
            reply->deleteLater();
            pendingRequests--;

            if (pendingRequests == 0)
            {
                qDebug() << "All schedules downloaded and saved (with errors)";
                emit allSchedulesDownloaded();
            }
        });
    }
}

void UploadWeekSchedule::save_day_schedule(const QJsonArray &schedule, const QString &clas, const QString &date)
{
    // Получаем день недели из даты
    QLocale enLocale(QLocale::English);
    QDate qdate = QDate::fromString(date, "yyyyMMdd");
    QString dayOfWeek = enLocale.toString(qdate, "dddd"); // Monday, Tuesday и т.д.

    // Добавляем расписание для класса в соответствующий день недели
    if (daySchedules.contains(dayOfWeek))
    {
        QJsonObject daySchedule = daySchedules[dayOfWeek];
        if (!schedule.isEmpty())
        {
            if (!daySchedule[clas].toArray().isEmpty())
            {
                QJsonArray v = daySchedule[clas].toArray();
                for (const QJsonValue& les : schedule)
                {
                    QJsonObject a = les.toObject();
                    v.append(a);
                }
                daySchedule[clas] = v;
            }
            else
            {
                daySchedule[clas] = schedule;
            }
        }
        daySchedules[dayOfWeek] = daySchedule;
    }
    else
    {
        qWarning() << "Invalid day of week:" << dayOfWeek;
        return;
    }

    QJsonObject daySchedule = daySchedules[dayOfWeek];
    bool allClassesProcessed = true;
    for (int i = 0; i < classes.size(); i++)
    {
        const QString clas = classes.at(i);
        if (!daySchedule.contains(clas))
        {
            allClassesProcessed = false;
            break;
        }
    }

    // Если все классы для дня обработаны, сохраняем в файл
    if (allClassesProcessed)
    {
        QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
        QString publicPath = homePath + "/Public/DataInfTablet";
        QDir dir(publicPath);
        QString filename = dir.filePath(QString("schedules/schedule_%1.json").arg(dayOfWeek));
        QFile file(filename);

        if (file.open(QIODevice::WriteOnly))
        {
            QJsonDocument doc(daySchedule);
            file.write(doc.toJson());
            file.close();
            qDebug() << "Saved schedule for" << dayOfWeek << "to" << filename;
        }
        else
           qWarning() << "Failed to save schedule for" << dayOfWeek << ":" << file.errorString();
    }
}

void UploadWeekSchedule::upload_outLessons(const QString &clas)
{
    QStringList urls = lyceum->get_schedule_urls_for_week(clas);

    for (int i = 0; i < urls.size(); i++)
    {
        QNetworkRequest request;
        QUrl scheduleUrl = urls.at(i);
        request.setUrl(scheduleUrl);

        QNetworkReply *reply = manager2->get(request);

        QString date = QDate::fromString(urls.at(i).split("days=").at(1).split("&").at(0), "yyyyMMdd").toString("yyyyMMdd");

        connect(reply, &QNetworkReply::finished, this, [=]()
        {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject root = doc.object();

            QJsonObject jv = root.value("response").toObject();
            QJsonObject jv2 = jv.value("result").toObject();
            QJsonObject jv3 = jv2.value("days").toObject();
            QJsonObject jv4 = jv3.value(date).toObject();
            QJsonArray jv5 = jv4.value("items_extday").toArray();

            emit classScheduleLoaded(jv5, clas, date);
            reply->deleteLater();
            pendingRequests--;

            if (pendingRequests == 0)
            {
                qDebug() << "All schedules downloaded and saved";
                emit allSchedulesDownloaded();
            }
        });

        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, [=]()
        {
            qWarning() << "Error for class" << clas << "on date" << date << ":" << reply->errorString();
            emit classScheduleLoaded(QJsonArray(), clas, date);
            reply->deleteLater();
            pendingRequests--;

            if (pendingRequests == 0)
            {
                qDebug() << "All schedules downloaded and saved (with errors)";
                emit allSchedulesDownloaded();
            }
        });
    }
}

void UploadWeekSchedule::upload_replace_lessons(const QString &clas)
{
    QStringList urls = lyceum->get_replaceLes_urls_for_week(clas);

    for (int i = 0; i < urls.size(); i++)
    {
        QNetworkRequest request;
        QUrl scheduleUrl = urls.at(i);
        request.setUrl(scheduleUrl);

        QNetworkReply *reply = manager3->get(request);

        QString date = QDate::fromString(urls.at(i).split("days=").at(1).split("&").at(0), "yyyyMMdd").toString("yyyyMMdd");

        connect(reply, &QNetworkReply::finished, this, [=]()
        {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QJsonObject root = doc.object();

            QJsonObject jv = root.value("response").toObject();
            QJsonObject jv2 = jv.value("result").toObject();
            QJsonArray jv3 = jv2.value("replace").toArray();

            emit classScheduleLoaded(jv3, clas, date);
            reply->deleteLater();
            pendingRequests--;

            if (pendingRequests == 0)
            {
                qDebug() << "All schedules downloaded and saved";
                emit allSchedulesDownloaded();
            }
        });

        connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, [=]()
        {
            qWarning() << "Error for class" << clas << "on date" << date << ":" << reply->errorString();
            emit classScheduleLoaded(QJsonArray(), clas, date);
            reply->deleteLater();
            pendingRequests--;

            if (pendingRequests == 0)
            {
                qDebug() << "All schedules downloaded and saved (with errors)";
                emit allSchedulesDownloaded();
            }
        });
    }
}
