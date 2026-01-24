#ifndef YANDEXDOWNLOADCONTROLLER_H
#define YANDEXDOWNLOADCONTROLLER_H
#include "internet_connection/internetconnection.h"
#include "schedule/controlleruploadschedule.h"
#include "yandexdirectorychecker.h"
#include "yandeximagebreakdownloader.h"
#include "yandeximageteachersdownloader.h"

class YandexDownloadController : public QObject
{
    Q_OBJECT
private:
    QPointer<YandexImageTeachersDownloader> teacherDownloader;
    QPointer<YandexImageBreakDownloader> breakDownloader;
    QPointer<YandexDirectoryChecker> yandexDirChecker;
    QPointer<internetConnection> intConnect;
    QPointer<ControllerUploadSchedule> conSchedule;
    bool imageDownloadFinished;
    bool breakDownloadFinished;
public:
    explicit YandexDownloadController(internetConnection* ptrIntcon, ControllerUploadSchedule* ptrConSchedule, QObject *parent = nullptr);
public slots:
    void start_download(const QJsonObject &schedule);
    void start_reserve_images();
private slots:
    void checkAllDownloadsFinished();
signals:
    void startDownload();
    void download_finished();
    void download_finished_with_errors();
    void pathsReady(const QStringList &paths);
};

#endif // YANDEXDOWNLOADCONTROLLER_H
