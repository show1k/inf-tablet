#ifndef YANDEXIMAGEBREAKDOWNLOADER_H
#define YANDEXIMAGEBREAKDOWNLOADER_H
#include <QObject>
#include <QNetworkReply>
#include <QPointer>

class YandexImageBreakDownloader : public QObject
{
    Q_OBJECT
private:
    QPointer<QNetworkAccessManager> manager;
    QString yandextoken;
    int m_remainingDownloads;
    QList<QString> savedPaths;
public:
    explicit YandexImageBreakDownloader(QObject *parent = nullptr);
    ~YandexImageBreakDownloader();
public slots:
    void download_break_files();
private slots:
    void handleFileListReply(QNetworkReply *reply);
    void downloadFile(const QString &filePath, const QString &savePath);
    void handleDownloadLinkReply(QNetworkReply *reply, const QString &savePath);
    void handleFileDownloadReply(QNetworkReply *reply, const QString &savePath);
    void checkAllDownloadsFinished();
signals:
    void pathReady(const QStringList &paths);
    void finished();
};

#endif // YANDEXIMAGEBREAKDOWNLOADER_H
