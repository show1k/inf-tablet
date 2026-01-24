#include "yandexdownloadcontroller.h"
#include <QThread>

YandexDownloadController::YandexDownloadController(internetConnection* ptrIntcon, ControllerUploadSchedule* ptrConSchedule, QObject *parent)
    : QObject(parent), intConnect(ptrIntcon), conSchedule(ptrConSchedule)
{
    connect(intConnect, &internetConnection::internet_connected, this, [=]()
    {
        connect(conSchedule, &ControllerUploadSchedule::schedule_ready_to_send, this, &YandexDownloadController::startDownload);
        connect(conSchedule, &ControllerUploadSchedule::schedule_ready_to_send, this, &YandexDownloadController::start_download);
    });
    connect(intConnect, &internetConnection::internet_disconnected, this, &YandexDownloadController::start_reserve_images);
    imageDownloadFinished = false;
    breakDownloadFinished = false;
}

void YandexDownloadController::start_download(const QJsonObject &schedule)
{
    if (teacherDownloader || breakDownloader) {
        return;
    }

    // Initialize image downloader
    teacherDownloader = new YandexImageTeachersDownloader();
    QThread *imageThread = teacherDownloader->thread();

    connect(teacherDownloader, &YandexImageTeachersDownloader::finished, this, [=]() {
        imageDownloadFinished = true;
        imageThread->quit();
    });
    connect(imageThread, &QThread::finished, this, [=]() {
        imageThread->wait();
        imageThread->deleteLater();
        teacherDownloader->deleteLater();
        teacherDownloader = nullptr;
        qDebug() << "teacherDownloader deleted and thread finished";
        checkAllDownloadsFinished();
    });

    // Initialize break downloader
    breakDownloader = new YandexImageBreakDownloader();
    QThread *breakThread = breakDownloader->thread();
    connect(breakDownloader, &YandexImageBreakDownloader::pathReady, this, &YandexDownloadController::pathsReady);
    connect(breakDownloader, &YandexImageBreakDownloader::finished, this, [=]()
    {
        breakDownloadFinished = true;
        breakThread->quit();
    });
    connect(breakThread, &QThread::finished, this, [=]() {
        breakThread->wait(); // Safe in main thread
        breakThread->deleteLater();
        breakDownloader->deleteLater();
        breakDownloader = nullptr;
        qDebug() << "breakDownloader deleted and thread finished";
        checkAllDownloadsFinished();
    });

    QMetaObject::invokeMethod(teacherDownloader, "download_images", Qt::QueuedConnection,
                              Q_ARG(QJsonObject, schedule));
    QMetaObject::invokeMethod(breakDownloader, "download_break_files", Qt::QueuedConnection);
}

void YandexDownloadController::start_reserve_images()
{
    if (yandexDirChecker)
        return;

    yandexDirChecker = new YandexDirectoryChecker();
    QThread *checkerThread = yandexDirChecker->thread();
    connect(yandexDirChecker, &YandexDirectoryChecker::allDirectoriesReady, this, [=](const QStringList &breakPaths)
    {
        emit pathsReady(breakPaths);
        checkerThread->quit();
    });
    connect(yandexDirChecker, &YandexDirectoryChecker::directoryCheckFailed, this, [=]()
    {
        emit download_finished_with_errors();
        checkerThread->quit();
    });
    connect(checkerThread, &QThread::finished, this, [=]()
    {
        checkerThread->wait();
        checkerThread->deleteLater();
        yandexDirChecker->deleteLater();
        yandexDirChecker = nullptr;
        qDebug() << "yandexDirChecker deleted and thread finished";
        emit download_finished();
    });

    QMetaObject::invokeMethod(yandexDirChecker, "check_directories", Qt::QueuedConnection);
}

void YandexDownloadController::checkAllDownloadsFinished()
{
    if (imageDownloadFinished && breakDownloadFinished) {
        qDebug() << "All downloads (images and break files) completed";
        emit download_finished();
    }
}
