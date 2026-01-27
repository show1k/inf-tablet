#ifndef UPLOADLASTSCHEDULE_H
#define UPLOADLASTSCHEDULE_H
#include "school.h"
#include <QJsonObject>
#include "collectinformationschool.h"

class UploadLastSchedule : public QObject
{
    Q_OBJECT
private:
    QPointer<School> lyceum;
    QPointer<QNetworkAccessManager> manager1;
    QPointer<QNetworkAccessManager> manager2;
    QPointer<QNetworkAccessManager> manager3;
    QJsonObject clas_shedule;
    QJsonObject classrooms_schedule;
    QStringList get_classes();
    static const int MAX_RETRIES = 3;
    QAtomicInt pendingRequests;
    enum RequestType {RegularSchedule, OutLessons, ReplaceLessons};
    void startRequestWithRetry(const QString &clas, const QUrl &url, QNetworkAccessManager *manager, RequestType type, int retryCount = 0);

    QJsonArray parseResponse(const QJsonObject &root, RequestType type) const;
    int calculateBackoffDelay(int retryCount) const;
    void checkAllCompleted();
public:
    explicit UploadLastSchedule(const QStringList& Data, QJsonObject &classrooms_schedule_m, QObject *parent = nullptr);
public slots:
    void get_classroom_shedule();
    void upload_class_shedule(const QString &clas);
    void upload_outLessons(const QString &clas);
    void start_upload_shedule();
private slots:
    void process_classroom_shedule();
    void upload_replace_lessons(const QString &clas);
signals:
    void classScheduleLoaded(const QJsonArray &schedule, const QString &clas);
    void allSchedulesLoaded();
    void classroomScheduleLoaded(const QJsonObject &classrooms_shedule);
};

#endif // UPLOADLASTSCHEDULE_H
