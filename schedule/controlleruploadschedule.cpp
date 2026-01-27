#include "controlleruploadschedule.h"
#include <QTimer>
#include <QJsonArray>
#include <QThread>

ControllerUploadSchedule::ControllerUploadSchedule(const QStringList &Data, collectInformationSchool *ptrColInfSch, internetConnection* ptrIntcon, QObject *parent) : QObject(parent),
    intConnect(ptrIntcon), colInfSch(ptrColInfSch), DataSchool(Data)
{
    connect(intConnect, &internetConnection::internet_connected, this, [=]
    {
        connect(colInfSch, &collectInformationSchool::infReady, this, &ControllerUploadSchedule::isInternet, Qt::UniqueConnection);
        colInfSch->check_infReady();
    });
    connect(intConnect, &internetConnection::internet_disconnected, this, [=]
    {
        connect(colInfSch, &collectInformationSchool::infReady, this, &ControllerUploadSchedule::isNotInternet, Qt::UniqueConnection);
        colInfSch->check_infReady();
    });
}

void ControllerUploadSchedule::isInternet()
{
    QStringList classrooms = collectInformationSchool::return_rooms();
    for (int i = 0; i < classrooms.size(); i++)
    {
        const QString& key = classrooms.at(i);
        classrooms_schedule.insert(key, QJsonArray());
    }

    downloadInfileEnded = false;
    downloadEnded = false;
    connect(this, &ControllerUploadSchedule::download_infile_schedule_end, this, &ControllerUploadSchedule::download_infile_finish);
    connect(this, &ControllerUploadSchedule::download_schedule_end, this, &ControllerUploadSchedule::download_finish);
    QTimer::singleShot(0, this, &ControllerUploadSchedule::download_schedule);
    QTimer::singleShot(0, this, &ControllerUploadSchedule::get_schedule);
}

void ControllerUploadSchedule::isNotInternet()
{
    QStringList classrooms = collectInformationSchool::return_rooms();
    for (int i = 0; i < classrooms.size(); i++)
    {
        const QString& key = classrooms.at(i);
        classrooms_schedule.insert(key, QJsonArray());
    }

     QTimer::singleShot(0, this, &ControllerUploadSchedule::download_reserve_schedule);
}

void ControllerUploadSchedule::get_schedule()
{
    if (uploader)
        return;
    uploader = new UploadLastSchedule(DataSchool, classrooms_schedule, this);
    connect(uploader, &UploadLastSchedule::classroomScheduleLoaded, this, [=](const QJsonObject &classrooms_shedule)
    {
        emit schedule_ready_to_send(classrooms_shedule);
        emit download_schedule_end();
        uploader->deleteLater();
        uploader = nullptr;
    });

    uploader->get_classroom_shedule();
}

void ControllerUploadSchedule::download_schedule()
{
    if (uploaderWeek)
        return;

    uploaderWeek = new UploadWeekSchedule(DataSchool);
    QThread *thread = uploaderWeek->thread();
    QObject::connect(uploaderWeek, &UploadWeekSchedule::allSchedulesDownloaded, thread, &QThread::quit);
    QObject::connect(thread, &QThread::finished, this, [=]()
    {
        thread->deleteLater();
        uploaderWeek->deleteLater();
        uploaderWeek = nullptr;
        emit download_infile_schedule_end();
    });
    uploaderWeek->start_upload_shedule();
}

void ControllerUploadSchedule::download_reserve_schedule()
{
    if (uploaderReserve)
        return;
    uploaderReserve = new ClassroomScheduleReader(classrooms_schedule, this);
    connect(uploaderReserve, &ClassroomScheduleReader::classroomScheduleLoaded, this, [=](const QJsonObject &classrooms_shedule)
    {
        emit schedule_ready_to_send(classrooms_shedule);
        uploaderReserve->deleteLater();
        emit get_schedule_finish();
    });
    connect(uploaderReserve, &ClassroomScheduleReader::notReserveSchedule, this, [=]()
    {
        uploaderReserve->deleteLater();
        emit notHaveSchedule();
        emit get_schedule_finish();
    });
    uploaderReserve->read_schedule_for_current_day();

}

void ControllerUploadSchedule::download_finish()
{
    downloadEnded = true;
    if (downloadInfileEnded && downloadEnded)
        emit get_schedule_finish();
}

void ControllerUploadSchedule::download_infile_finish()
{
    downloadInfileEnded = true;
    if (downloadInfileEnded && downloadEnded)
        emit get_schedule_finish();
}
