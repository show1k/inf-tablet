#ifndef UPLOADWEEKSCHEDULE_H
#define UPLOADWEEKSCHEDULE_H
#include "school.h"
#include <QNetworkAccessManager>

class UploadWeekSchedule : public QObject
{
    Q_OBJECT
private:
    QPointer<School> lyceum;
    QPointer<QNetworkAccessManager> manager;
    QPointer<QNetworkAccessManager> manager2;
    QPointer<QNetworkAccessManager> manager3;
    int pendingRequests;
    QMap<QString, QJsonObject> daySchedules; // Хранит расписания по дням недели
    QStringList classes;
    QStringList get_classes();
public:
    explicit UploadWeekSchedule(QObject *parent = nullptr);
    ~UploadWeekSchedule();
public slots:
    void start_upload_shedule();
private slots:
    void upload_class_shedule(const QString &clas);
    void save_day_schedule(const QJsonArray &schedule, const QString &clas, const QString &date);
    void upload_outLessons(const QString &clas);
    void upload_replace_lessons(const QString &clas);
signals:
    void classScheduleLoaded(const QJsonArray &schedule, const QString &clas, const QString &date);
    void allSchedulesDownloaded();
};

#endif // UPLOADWEEKSCHEDULE_H
