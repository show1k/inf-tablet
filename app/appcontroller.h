#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H
#include <auroraapp.h>
#include "time/timeofapp.h"
#include "schedule/controlleruploadschedule.h"
#include "download_pictures/yandexdownloadcontroller.h"
#include "internet_connection/internetconnection.h"
#include "cabinetselector.h"
#include "lesson/controllerlessons.h"
#include "set_pictures/controllersetpictures.h"
#include "schedule/collectinformationschool.h"
#include "enter_inf/enter_inf.h"

class AppController : public QObject
{
    Q_OBJECT
private:
    QScopedPointer<QQuickView> view;
    QPointer<CabinetSelector> cabinetSelector;
    QPointer<QTimer> secTimer;
    QPointer<TimeofApp> time;
    QPointer<ControllerUploadSchedule> conSchedule;
    QPointer<YandexDownloadController> yandexDownloaderCon;
    QPointer<internetConnection> internetCon;
    QPointer<ControllerSetPictures> conPictures;
    QPointer<ControllerLessons> conLessons;
    QPointer<collectInformationSchool> colInfSch;
    QPointer<enter_inf> enterInf;
    QJsonObject schedule;
    QString cabinet;
    QList<QString> pathsToImagesBreak;
    QMetaObject::Connection startAppConnection;
    QMetaObject::Connection selectLesson;
    QMetaObject::Connection updateInf;
    bool schedule_finished;
    bool pathsToImagesBreak_finished;
    QStringList get_admin_cab();
    void choice_page_cab();
    void set_Enter_inf();
public:
    AppController(QObject *parent = nullptr);
    void setView();
    void setTimer();
    void setTimeofApp();
private slots:
    void start_app();
    void display_curtime();
    void setCabinetSelector();
    void setCabinet(const QString&);
    void start_download_information();
    void set_schedule(const QJsonObject &classrooms_shedule);
    void set_pathsToImages(const QStringList &paths);
    void not_internet_and_reserve_schedule();
    void not_internet_and_reserve_infOfSchool();
    void not_reserve_images();
    void schedule_finish();
    void pathsToImagesBreak_finish();
    void set_conLessons();
    void set_conPictures();
    void update_information();
    void set_admin_cab();
    void prepare_app();
signals:
    void roomsReady();
    void informationReady();
    void startDownloadScheduleToQml();
    void startDownloadPicturesToQml();
    void startDownloadReserveScheduleAndCheckReservePicturesToQml();
    void checkInternetToQml();
    void notInternetAndReserveInfOfSchool();
    void notInternetAndReserveScheduleToQml();
    void notReserveImagesToQml();
    void cabinetReady();
    void triggerFadeAnimation();
    void stopFadeAnimation();
    void clear_information();
};

#endif // APPCONTROLLER_H
