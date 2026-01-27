#include "uploadlastschedule.h"
#include <QJsonArray>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QMap>
#include <QTimer>

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

    // Подключаем обработчик один раз
    connect(this, &UploadLastSchedule::classScheduleLoaded, this, [=](const QJsonArray &schedule, const QString &clas) {
        if (!schedule.isEmpty())
        {
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
        }
        if (!pendingRequests.deref())
            checkAllCompleted();
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

int UploadLastSchedule::calculateBackoffDelay(int retryCount) const
{
    return qMin(10000, 1000 * (1 << retryCount));
}

void UploadLastSchedule::checkAllCompleted()
{
    if (pendingRequests.loadAcquire() == 0) {
        qDebug() << "clas_shedule ready";
        emit allSchedulesLoaded();
    }
}

// Парсер как метод класса (имеет доступ к RequestType)
QJsonArray UploadLastSchedule::parseResponse(const QJsonObject &root, RequestType type) const
{
    QJsonObject response = root.value("response").toObject();
    QJsonObject result = response.value("result").toObject();

    if (type == ReplaceLessons) {
        return result.value("replace").toArray();
    }

    QJsonObject days = result.value("days").toObject();
    QJsonObject dayData = days.value(QDateTime::currentDateTime().toString("yyyyMMdd")).toObject();

    if (type == RegularSchedule) {
        return dayData.value("items").toArray();
    } else { // OutLessons
        return dayData.value("items_extday").toArray();
    }
}

void UploadLastSchedule::startRequestWithRetry(const QString &clas, const QUrl &url, QNetworkAccessManager *manager, RequestType type, int retryCount)
{
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    pendingRequests.ref();  // Атомарно увеличиваем ДО запуска

    // Таймер таймаута (10 секунд)
    QTimer *timeoutTimer = new QTimer(this);
    timeoutTimer->setSingleShot(true);
    // При таймауте — отменяем запрос (вызовет обработчик ошибок)
    connect(timeoutTimer, &QTimer::timeout, reply, [reply]()
    {
        reply->abort(); // Приведёт к срабатыванию ошибки OperationCanceledError
    });

    timeoutTimer->start(10000);

    // Единый обработчик завершения (успех/ошибка/таймаут)
    connect(reply, &QNetworkReply::finished, this, [=]()
    {
        // Останавливаем и удаляем таймер
        timeoutTimer->stop();
        timeoutTimer->deleteLater();

        QNetworkReply::NetworkError error = reply->error();
        bool isTimeout = (error == QNetworkReply::OperationCanceledError);

        if (error != QNetworkReply::NoError) {
            QString errorMsg = isTimeout ? "Request timeout (10s)" : reply->errorString();
            qWarning() << (isTimeout ? "Timeout" : "Error")
                       << "for" << clas
                       << "(attempt" << (retryCount + 1) << "/" << (MAX_RETRIES) << "):" << errorMsg;

            reply->deleteLater();

            if (retryCount < MAX_RETRIES) {
                int delay = calculateBackoffDelay(retryCount + 1);
                qInfo() << "Retrying" << clas << "after" << delay << "ms";
                QTimer::singleShot(delay, this, [=]() {
                    startRequestWithRetry(clas, url, manager, type, retryCount + 1);
                });
                pendingRequests.deref();
                return;
            }

            qWarning() << "Max retries reached for" << clas;
            emit classScheduleLoaded(QJsonArray(), clas);
            return;
        }

        // Успешный ответ
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        reply->deleteLater();

        if (doc.isNull() || !doc.isObject()) {
            qWarning() << "Invalid JSON for" << clas;
            if (retryCount < MAX_RETRIES) {
                QTimer::singleShot(calculateBackoffDelay(retryCount + 1), this, [=]() {
                    startRequestWithRetry(clas, url, manager, type, retryCount + 1);
                });
                pendingRequests.deref();
                return;
            }
        }

        // Финальный результат
        QJsonArray result = doc.isNull() ? QJsonArray() : parseResponse(doc.object(), type);
        emit classScheduleLoaded(result, clas);
    });
}

// Основные функции остаются компактными
void UploadLastSchedule::upload_class_shedule(const QString &clas)
{
    QString url = lyceum->get_shedule_url(clas);
    startRequestWithRetry(clas, QUrl(url), manager1, RegularSchedule, 0);
}

void UploadLastSchedule::upload_outLessons(const QString &clas)
{
    QString url = lyceum->get_shedule_url(clas);
    startRequestWithRetry(clas, QUrl(url), manager2, OutLessons, 0);
}

void UploadLastSchedule::upload_replace_lessons(const QString &clas)
{
    QString url = lyceum->get_replace_url(clas);
    startRequestWithRetry(clas, QUrl(url), manager3, ReplaceLessons, 0);
}
