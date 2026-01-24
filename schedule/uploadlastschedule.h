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
    int pendingRequests;
    QStringList get_classes();
public:
    explicit UploadLastSchedule(QJsonObject &classrooms_schedule_m, QObject *parent = nullptr);
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
    void allSchedulesLoaded(const QJsonObject &schedules);
    void classroomScheduleLoaded(const QJsonObject &classrooms_shedule);
};

#endif // UPLOADLASTSCHEDULE_H
