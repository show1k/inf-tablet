#ifndef CONTROLLERUPLOADSCHEDULE_H
#define CONTROLLERUPLOADSCHEDULE_H
#include "internet_connection/internetconnection.h"
#include "uploadlastschedule.h"
#include "uploadweekschedule.h"
#include "classroomschedulereader.h"
#include "collectinformationschool.h"

class ControllerUploadSchedule : public QObject
{
    Q_OBJECT
private:
    QPointer<UploadLastSchedule> uploader;
    QPointer<UploadWeekSchedule> uploaderWeek;
    QPointer<ClassroomScheduleReader> uploaderReserve;
    QPointer<internetConnection> intConnect;
    QPointer<collectInformationSchool> colInfSch;
    QJsonObject classrooms_schedule;
    bool downloadInfileEnded;
    bool downloadEnded;
public:
    explicit ControllerUploadSchedule(collectInformationSchool* ptrColInfSch, internetConnection* ptrIntcon, QObject *parent = nullptr);
    void start();
private slots:
    void isInternet();
    void isNotInternet();
    void get_schedule();
    void download_schedule();
    void download_reserve_schedule();
    void download_finish();
    void download_infile_finish();
signals:
    void schedule_ready_to_send(const QJsonObject& schedule);
    void download_infile_schedule_end();
    void download_schedule_end();
    void get_schedule_finish();
    void notHaveSchedule();
};

#endif // CONTROLLERUPLOADSCHEDULE_H
