#ifndef YANDEXIMAGETEACHERSDOWNLOADER_H
#define YANDEXIMAGETEACHERSDOWNLOADER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QPointer>
#include <QJsonObject>

class YandexImageTeachersDownloader : public QObject
{
    Q_OBJECT
private:
    QPointer<QNetworkAccessManager> manager;
    QString yandextoken;
    int m_remainingDownloads;
public:
    explicit YandexImageTeachersDownloader(QObject *parent = nullptr);
    ~YandexImageTeachersDownloader();
public slots:
    void download_images(QJsonObject shedule);
private slots:
    void downloadImage(const QString &filePath, const QString &savePath);
    void handleDownloadLinkReply(QNetworkReply *reply, const QString &savePath);
    void downloadFile(const QString &url, const QString &savePath);
    void handleFileDownloadReply(QNetworkReply *reply, const QString &savePath);
    void checkAllDownloadsFinished();
private:
    QStringList get_teachers_name(QJsonObject &shedule);
    QString getCorrectExtension(const QByteArray &imageData);
    QStringList get_add_name() const;
signals:
    void finished();
};

#endif // YANDEXIMAGETEACHERSDOWNLOADER_H
