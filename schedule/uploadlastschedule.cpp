#include "uploadlastschedule.h"
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QMap>

UploadLastSchedule::UploadLastSchedule(const QStringList &Data, QJsonObject &classrooms_schedule_m, QObject *parent) : QObject(parent), classrooms_schedule(classrooms_schedule_m)
{
    lyceum = new School(Data, this);
    manager1 = new QNetworkAccessManager(this);
    manager2 = new QNetworkAccessManager(this);
    manager3 = new QNetworkAccessManager(this);
    pendingRequests = 0;

    QStringList temp = collectInformationSchool::return_classes();
    for (int i = 0; i < temp.size(); i++)
    {
        const QString& key = temp.at(i);
        clas_shedule.insert(key, QJsonArray());
    }
}

QStringList UploadLastSchedule::get_classes()
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

void UploadLastSchedule::get_classroom_shedule()
{
    connect(this, &UploadLastSchedule::allSchedulesLoaded, this, &UploadLastSchedule::process_classroom_shedule);

    start_upload_shedule();
}

void UploadLastSchedule::start_upload_shedule()
{
    QStringList classesForOutLes = get_classes();
    QStringList classesForReplaceLes = collectInformationSchool::return_classes();
    qDebug() << classesForOutLes;
    qDebug() << classesForReplaceLes;
    pendingRequests = clas_shedule.keys().size() + classesForOutLes.size() + classesForReplaceLes.size(); // Количество запросов

    // Подключаем обработчик один раз
    connect(this, &UploadLastSchedule::classScheduleLoaded, this, [=](const QJsonArray &schedule, const QString &clas) {
        if (!schedule.isEmpty())
        {
            qDebug() << clas << schedule;
            if (!clas_shedule[clas].toArray().isEmpty())
            {
                QJsonArray v = clas_shedule[clas].toArray();
                for (const QJsonValue& les : schedule)
                {
                    QJsonObject a = les.toObject();
                    v.append(a);
                }
                clas_shedule[clas] = v;
            }
            else
            {
                clas_shedule[clas] = schedule;
            }
            qDebug() << clas_shedule[clas];
        }
        pendingRequests--; // Уменьшаем счетчик

        if (pendingRequests == 0) {
            emit allSchedulesLoaded(clas_shedule); // Все запросы завершены
        }
    });

    // Запускаем запросы для всех классов
    QStringList keys = clas_shedule.keys(); // Копируем ключи один раз
    for (int i = 0; i < keys.size(); ++i)
    {
        upload_class_shedule(keys.at(i));
    }

    for (const QString& clas : classesForOutLes)
    {
        upload_outLessons(clas);
    }

    for (const QString& clas : classesForReplaceLes)
    {
        upload_replace_lessons(clas);
    }
}

void UploadLastSchedule::process_classroom_shedule()
{
    // Обрабатываем расписание для каждого класса с использованием индексного цикла
    for (int i = 0; i < classrooms_schedule.count(); i++)
    {
        QString classroom_name = classrooms_schedule.keys().at(i);

        // Инициализируем пустой массив для расписания аудитории
        QJsonArray cur_classroom_schedule;

        for (int j = 0; j < clas_shedule.count(); j++)
        {
            QString classname = clas_shedule.keys().at(j);
            QJsonArray cur_clas_schedule = clas_shedule.value(classname).toArray();
            if (classname == "11Д")
                qDebug () << cur_clas_schedule;
            for (int k = 0; k < cur_clas_schedule.count(); k++)
            {
                QJsonObject cur_lesson = cur_clas_schedule[k].toObject();

                if (cur_lesson["room"].toString() == classroom_name)
                {
                    cur_lesson.insert("class", classname);
                    // Добавляем урок в массив
                    cur_classroom_schedule.append(cur_lesson);
                }
            }
        }
        classrooms_schedule[classroom_name] = cur_classroom_schedule;
    }

        // Отладочный вывод
        QStringList room_keys = classrooms_schedule.keys();
        for (int i = 0; i < room_keys.size(); ++i)
        {
            QString classroom_name = room_keys.at(i);
            qDebug() << "\n" << classroom_name << "\n" << classrooms_schedule.value(classroom_name) << "\n";
        }

    // Отправляем результат через сигнал
        emit classroomScheduleLoaded(classrooms_schedule);
}

void UploadLastSchedule::upload_class_shedule(const QString &clas)
{
    QNetworkRequest request;
    QUrl sheduleurl = lyceum->get_shedule_url(clas);
    request.setUrl(sheduleurl);

    QNetworkReply *reply = manager1->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();

        QJsonObject jv = root.value("response").toObject();
        QJsonObject jv2 = jv.value("result").toObject();
        QJsonObject jv3 = jv2.value("days").toObject();
        QJsonObject jv4 = jv3.value(QDateTime::currentDateTime().toString("yyyyMMdd")).toObject();
        QJsonArray jv5 = jv4.value("items").toArray();

        emit classScheduleLoaded(jv5, clas);
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, [=]()
    {
            qWarning() << "Error for class" << clas << ":" << reply->errorString();
            emit classScheduleLoaded(QJsonArray(), clas);
            reply->deleteLater();
    });
}

void UploadLastSchedule::upload_outLessons(const QString &clas)
{
    qDebug() << "start upload out les for " << clas;
    QNetworkRequest request;
    QUrl sheduleurl = lyceum->get_shedule_url(clas);
    request.setUrl(sheduleurl);

    QNetworkReply *reply = manager2->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();

        QJsonObject jv = root.value("response").toObject();
        QJsonObject jv2 = jv.value("result").toObject();
        QJsonObject jv3 = jv2.value("days").toObject();
        QJsonObject jv4 = jv3.value(QDateTime::currentDateTime().toString("yyyyMMdd")).toObject();
        QJsonArray jv5 = jv4.value("items_extday").toArray();
        qDebug() << "outLes" <<  jv5;
        emit classScheduleLoaded(jv5, clas);
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, [=]()
    {
            qWarning() << "Error for class" << clas << ":" << reply->errorString();
            emit classScheduleLoaded(QJsonArray(), clas);
            reply->deleteLater();
    });
}

void UploadLastSchedule::upload_replace_lessons(const QString &clas)
{
    qDebug() << "start upload replace lessons for " << clas;
    QNetworkRequest request;
    QUrl sheduleurl = lyceum->get_replace_url(clas);
    //qDebug() << sheduleurl;
    request.setUrl(sheduleurl);

    QNetworkReply *reply = manager3->get(request);

    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject root = doc.object();

        QJsonObject jv = root.value("response").toObject();
        QJsonObject jv2 = jv.value("result").toObject();
        QJsonArray jv3 = jv2.value("replace").toArray();
        qDebug() << "replaces" <<  jv3;
        emit classScheduleLoaded(jv3, clas);
        reply->deleteLater();
    });

    connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), this, [=]()
    {
            qWarning() << "Error for class" << clas << ":" << reply->errorString();
            emit classScheduleLoaded(QJsonArray(), clas);
            reply->deleteLater();
    });
}
