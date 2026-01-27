#ifndef UPLOADWEEKSCHEDULE_H
#define UPLOADWEEKSCHEDULE_H
#include "school.h"
#include <QNetworkAccessManager>
#include <QAtomicInt>

class UploadWeekSchedule : public QObject
{
    Q_OBJECT
private:
    QPointer<School> lyceum;
    QPointer<QNetworkAccessManager> manager;
    QPointer<QNetworkAccessManager> manager2;
    QPointer<QNetworkAccessManager> manager3;
    QMap<QString, QJsonObject> daySchedules; // Хранит расписания по дням недели
    QStringList classes;
    QStringList get_classes();
    static const int MAX_RETRIES = 3;
    QAtomicInt pendingRequests;
    enum RequestType {RegularSchedule, OutLessons, ReplaceLessons};
    void startRequestWithRetry(const QString &clas, const QString &date, const QUrl &url, QNetworkAccessManager *manager, RequestType type, int retryCount = 0);

    QJsonArray parseResponse(const QJsonObject &root, RequestType type, const QString &date) const;
    int calculateBackoffDelay(int retryCount) const;
    void checkAllCompleted();
    void save_schedules();
public:
    explicit UploadWeekSchedule(const QStringList& Data, QObject *parent = nullptr);
    ~UploadWeekSchedule();
public slots:
    void start_upload_shedule();
private slots:
    void upload_class_shedule(const QString &clas);
    void proccess_schedule(const QJsonArray &schedule, const QString &clas, const QString &date);
    void upload_outLessons(const QString &clas);
    void upload_replace_lessons(const QString &clas);
signals:
    void classScheduleLoaded(const QJsonArray &schedule, const QString &clas, const QString &date);
    void allSchedulesDownloaded();
};

#endif // UPLOADWEEKSCHEDULE_H
