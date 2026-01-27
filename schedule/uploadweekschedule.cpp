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
#include <QTimer>

UploadWeekSchedule::UploadWeekSchedule(const QStringList &Data, QObject *parent) : QObject(parent)
{
    lyceum = new School(Data, this);
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

    // Инициализируем QMap для хранения расписаний по дням недели
    daySchedules.clear();
    QStringList days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    for (const QString &day : days)
        daySchedules[day] = QJsonObject();

    // Подключаем обработчик для сохранения расписания
    connect(this, &UploadWeekSchedule::classScheduleLoaded, this, &UploadWeekSchedule::proccess_schedule);

    pendingRequests.storeRelease(0);

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

void UploadWeekSchedule::save_schedules()
{
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QDir dir(publicPath);
    QStringList days = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    for (const QString& dayOfWeek : days)
    {
        QString filename = dir.filePath(QString("schedules/schedule_%1.json").arg(dayOfWeek));
        QFile file(filename);
        const QJsonObject& daySchedule = daySchedules[dayOfWeek];
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

void UploadWeekSchedule::proccess_schedule(const QJsonArray &schedule, const QString &clas, const QString &date)
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
    }
    if (!pendingRequests.deref())
        checkAllCompleted();
}

int UploadWeekSchedule::calculateBackoffDelay(int retryCount) const
{
    return qMin(10000, 1000 * (1 << retryCount));
}

void UploadWeekSchedule::checkAllCompleted()
{
    if (pendingRequests.loadAcquire() == 0) {
        qDebug() << "All schedules downloaded successfully";
        save_schedules();
        emit allSchedulesDownloaded();
    }
}

// Парсер как метод класса (имеет доступ к RequestType)
QJsonArray UploadWeekSchedule::parseResponse(const QJsonObject &root, RequestType type, const QString &date) const
{
    QJsonObject response = root.value("response").toObject();
    QJsonObject result = response.value("result").toObject();

    if (type == ReplaceLessons) {
        return result.value("replace").toArray();
    }

    QJsonObject days = result.value("days").toObject();
    QJsonObject dayData = days.value(date).toObject();

    if (type == RegularSchedule) {
        return dayData.value("items").toArray();
    } else { // OutLessons
        return dayData.value("items_extday").toArray();
    }
}

void UploadWeekSchedule::startRequestWithRetry(const QString &clas, const QString &date, const QUrl &url, QNetworkAccessManager *manager, RequestType type, int retryCount)
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
                       << "for" << clas << "on" << date
                       << "(attempt" << (retryCount + 1) << "/" << (MAX_RETRIES) << "):" << errorMsg;

            reply->deleteLater();

            if (retryCount < MAX_RETRIES) {
                int delay = calculateBackoffDelay(retryCount + 1);
                qInfo() << "Retrying" << clas << date << "after" << delay << "ms";
                QTimer::singleShot(delay, this, [=]() {
                    startRequestWithRetry(clas, date, url, manager, type, retryCount + 1);
                });
                pendingRequests.deref();
                return; // Не декрементируем — задача продолжается
            }

            qWarning() << "Max retries reached for" << clas << date;
            emit classScheduleLoaded(QJsonArray(), clas, date);
            return;
        }

        // Успешный ответ
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        reply->deleteLater();

        if (doc.isNull() || !doc.isObject()) {
            qWarning() << "Invalid JSON for" << clas << "on" << date;
            if (retryCount < MAX_RETRIES) {
                QTimer::singleShot(calculateBackoffDelay(retryCount + 1), this, [=]() {
                    startRequestWithRetry(clas, date, url, manager, type, retryCount + 1);
                });
                pendingRequests.deref();
                return; // Не декрементируем — задача продолжается
            }
        }

        // Финальный результат
        QJsonArray result = doc.isNull() ? QJsonArray() : parseResponse(doc.object(), type, date);
        emit classScheduleLoaded(result, clas, date);
    });
}

// Основные функции остаются компактными
void UploadWeekSchedule::upload_class_shedule(const QString &clas)
{
    QStringList urls = lyceum->get_schedule_urls_for_week(clas);

    for (const QString &urlStr : urls) {
        QString date = QDate::fromString(
            urlStr.split("days=").at(1).split("&").at(0), "yyyyMMdd"
        ).toString("yyyyMMdd");

        startRequestWithRetry(clas, date, QUrl(urlStr), manager, RegularSchedule, 0);
    }
}

void UploadWeekSchedule::upload_outLessons(const QString &clas)
{
    QStringList urls = lyceum->get_schedule_urls_for_week(clas);

    for (const QString &urlStr : urls) {
        QString date = QDate::fromString(
            urlStr.split("days=").at(1).split("&").at(0), "yyyyMMdd"
        ).toString("yyyyMMdd");

        startRequestWithRetry(clas, date, QUrl(urlStr), manager2, OutLessons, 0);
    }
}

void UploadWeekSchedule::upload_replace_lessons(const QString &clas)
{
    QStringList urls = lyceum->get_replaceLes_urls_for_week(clas);

    for (const QString &urlStr : urls) {
        QString date = QDate::fromString(
            urlStr.split("days=").at(1).split("&").at(0), "yyyyMMdd"
        ).toString("yyyyMMdd");

        startRequestWithRetry(clas, date, QUrl(urlStr), manager3, ReplaceLessons, 0);
    }
}
