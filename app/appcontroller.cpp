#include "appcontroller.h"
#include <QTimer>
#include <QQmlContext>
#include "globalVar.h"

AppController::AppController(QObject *parent) : QObject(parent)
{
    setView();
    startAppConnection = connect(this, &AppController::cabinetReady, this, &AppController::start_app);
    QTimer::singleShot(0, this, &AppController::setCabinetSelector);
    QTimer::singleShot(0, this, &AppController::start_download_information);
}

void AppController::start_app()
{
    setTimer();
    setTimeofApp();

    connect(secTimer, &QTimer::timeout, time, &TimeofApp::updateTime);
    connect(time, &TimeofApp::timeChanged, this, &AppController::display_curtime);
    secTimer->start(1000);
    disconnect(startAppConnection);

    choice_page_cab();
}

void AppController::update_information()
{
    secTimer->stop();
    emit clear_information();
    updateInf = connect(this, &AppController::informationReady, this, [=]()
    {
        secTimer->start(1000);

        choice_page_cab();

        disconnect(updateInf);
    });
    QTimer::singleShot(0, this, &AppController::start_download_information);
}

void AppController::start_download_information()
{
    if(conSchedule)
    {
        qDebug() << "conSchedule еще не завершил работу";
        return;
    }
    if(internetCon)
    {
        qDebug() << "internetCon еще не завершил работу";
        return;
    }
    if(yandexDownloaderCon)
    {
        qDebug() << "yandexDownloaderCon еще не завершил работу";
        return;
    }

    schedule_finished = false;
    pathsToImagesBreak_finished = false;

    internetCon = new internetConnection(this);
    connect(internetCon, &internetConnection::internet_connected, this, &AppController::startDownloadScheduleToQml);
    connect(internetCon, &internetConnection::internet_disconnected, this, &AppController::startDownloadReserveScheduleAndCheckReservePicturesToQml);
    connect(internetCon, &internetConnection::finish_check, internetCon, &internetConnection::deleteLater);

    if (!colInfSch)
    {
        colInfSch = new collectInformationSchool(internetCon, this);
        connect(colInfSch, &collectInformationSchool::noData, this, &AppController::not_internet_and_reserve_infOfSchool);
        connect(colInfSch, &collectInformationSchool::infReady, this, &AppController::roomsReady);
        connect(colInfSch, &collectInformationSchool::noData, colInfSch, &collectInformationSchool::deleteLater);
    }

    conSchedule = new ControllerUploadSchedule(colInfSch, internetCon, this);
    connect(conSchedule, &ControllerUploadSchedule::notHaveSchedule, this, &AppController::not_internet_and_reserve_schedule);
    connect(conSchedule, &ControllerUploadSchedule::schedule_ready_to_send, this, &AppController::set_schedule);
    connect(conSchedule, &ControllerUploadSchedule::get_schedule_finish, conSchedule, &ControllerUploadSchedule::deleteLater);
    connect(colInfSch, &collectInformationSchool::noData, conSchedule, &ControllerUploadSchedule::deleteLater);

    yandexDownloaderCon = new YandexDownloadController(internetCon, conSchedule, this);
    connect(yandexDownloaderCon, &YandexDownloadController::startDownload, this, &AppController::startDownloadPicturesToQml);
    connect(yandexDownloaderCon, &YandexDownloadController::pathsReady, this, &AppController::set_pathsToImages);
    connect(yandexDownloaderCon, &YandexDownloadController::download_finished, this, &AppController::pathsToImagesBreak_finish);
    connect(yandexDownloaderCon, &YandexDownloadController::download_finished_with_errors, this, &AppController::not_reserve_images);
    connect(yandexDownloaderCon, &YandexDownloadController::download_finished, yandexDownloaderCon, &YandexDownloadController::deleteLater);

    internetCon->check_internet_connection();
}

void AppController::not_internet_and_reserve_schedule()
{
    emit notInternetAndReserveScheduleToQml();
    qDebug() << "not_internet_and_reserve_schedule";
    QTimer::singleShot(5000, this, &AppController::start_download_information);
}

void AppController::not_internet_and_reserve_infOfSchool()
{
    emit notInternetAndReserveInfOfSchool();
    qDebug() << "not_internet_and_reserve_infOfSchool";
    QTimer::singleShot(5000, this, &AppController::start_download_information);
}

void AppController::not_reserve_images()
{
    emit notReserveImagesToQml();
    qDebug() << "not_reserve_images";
}

void AppController::schedule_finish()
{
    schedule_finished = true;
    if (schedule_finished && pathsToImagesBreak_finished)
    {
        qDebug() << "informationReady";
        emit informationReady();
    }
}

void AppController::pathsToImagesBreak_finish()
{
    pathsToImagesBreak_finished = true;
    if (schedule_finished && pathsToImagesBreak_finished)
    {
        qDebug() << "informatoinReady";
        emit informationReady();
    }
}

void AppController::set_conLessons()
{
    if(conLessons)
        return;
    conLessons = new ControllerLessons(view.data(), this);
    connect(this, &AppController::clear_information, conLessons, &ControllerLessons::deleteLater);
    connect(conLessons, &ControllerLessons::connectTimerToCheck, this, [=]()
    {
        selectLesson = connect(time, &TimeofApp::timeChanged, conLessons, &ControllerLessons::check_time_in_range_of_lesson);
        connect(conLessons, &ControllerLessons::studyDayFinished, [=]()
        {
            disconnect(selectLesson);
        });
    });
    connect(conLessons, &ControllerLessons::lessonsReady, conLessons, &ControllerLessons::display);
    conLessons->get_lessons(schedule, cabinet);
}

void AppController::set_conPictures()
{
    if (conPictures)
        return;

    conPictures = new ControllerSetPictures(view.data(), this);
    connect(this, &AppController::clear_information, conPictures, &ControllerSetPictures::deleteLater);
    connect(conLessons, &ControllerLessons::setPictureTeacher, conPictures, &ControllerSetPictures::stopImageRotation);
    connect(conPictures, &ControllerSetPictures::triggerFadeAnimation, this, &AppController::triggerFadeAnimation);
    connect(conPictures, &ControllerSetPictures::stopFadeAnimation, this, &AppController::stopFadeAnimation);
    connect(conLessons, &ControllerLessons::setPictureTeacher, conPictures, &ControllerSetPictures::get_name_file_teacher);
    connect(conLessons, &ControllerLessons::unsetPictureTeacher, conPictures, &ControllerSetPictures::clear_image);
    connect(conLessons, &ControllerLessons::unsetPictureTeacher, conPictures, &ControllerSetPictures::startImageRotation);

    conPictures->setImagePaths(pathsToImagesBreak);
    connect(conLessons, &ControllerLessons::findNextLessonFinished, conPictures, &ControllerSetPictures::startImageRotation);
}

void AppController::setView()
{
    if(!view.data())
    {
        view.reset(Aurora::Application::createView());
        view->setSource(Aurora::Application::pathTo(QStringLiteral("qml/InfTablet.qml")));
        view->rootContext()->setContextProperty("AppController", this);
        view->show();
        emit checkInternetToQml();
    }
}

void AppController::setTimer()
{
    if(secTimer)
        return;
    secTimer = new QTimer(this);
}

void AppController::setCabinetSelector()
{
    if (cabinetSelector)
        return;
    cabinetSelector = new CabinetSelector(this);
    view->rootContext()->setContextProperty("CabinetSelector", cabinetSelector);
    connect(this, &AppController::roomsReady, cabinetSelector, &CabinetSelector::setCabinetModel);
    connect(cabinetSelector, &CabinetSelector::selectedCabinetChanged, this, &AppController::setCabinet);
    connect(cabinetSelector, &CabinetSelector::finished, cabinetSelector, &CabinetSelector::deleteLater);
}

void AppController::setCabinet(const QString &cab)
{
    cabinet = cab;
    qDebug() << "cabinetseted " << cabinet;
    emit cabinetReady();
}

void AppController::setTimeofApp()
{
    if (time)
        return;
    time = new TimeofApp(view.data(), this);
    connect(time, &TimeofApp::dayChanged, this, &AppController::update_information);
}

void AppController::set_schedule(const QJsonObject &classrooms_shedule)
{
    schedule = classrooms_shedule;
    //qDebug() << schedule;
    QTimer::singleShot(0, this, &AppController::schedule_finish);
}

void AppController::set_pathsToImages(const QStringList &paths)
{
    pathsToImagesBreak = paths;
    //qDebug() << "setedPaths" << pathsToImagesBreak;
}

void AppController::set_admin_cab()
{
    if (conPictures)
        return;

    conPictures = new ControllerSetPictures(view.data(), this);
    connect(this, &AppController::clear_information, conPictures, &ControllerSetPictures::deleteLater);
    QTimer::singleShot(0, this, [=]()
    {
        conPictures->set_admin_image(cabinet);
    });
}

QStringList AppController::get_admin_cab()
{
    return admin_cabs;
}

void AppController::choice_page_cab()
{
    QStringList admin_cab = get_admin_cab();
    if (!admin_cab.contains(cabinet))
    {
        QTimer::singleShot(0, this, &AppController::set_conLessons);
        QTimer::singleShot(0, this, &AppController::set_conPictures);
    }
    else
    {
        qDebug() << "cabinet of the admin: " << cabinet;
        QTimer::singleShot(0, this, &AppController::set_admin_cab);
    }
}

void AppController::display_curtime()
{
    qDebug() << TimeofApp::returnTime();
}
