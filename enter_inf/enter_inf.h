#ifndef ENTER_INF_H
#define ENTER_INF_H

#include <QObject>
#include <QStringList>

class enter_inf : public QObject
{
    Q_OBJECT
private:
    void yandex_disk();
    void about_school();
    QStringList read_file(const QString& Path);
    void check_correct_inf_eljur(const QString &Url);
    void check_correct_inf_YandexDisk(const QString &Token);
    void save_inf(const QString&, const QStringList&);
    void send_data();
    QMetaObject::Connection save;
    QString HomePath;
public:
    QStringList AdminCab;
    QStringList DataEljur;
    QString YandexToken;
    QStringList About;
    void eljur();
    explicit enter_inf(QObject *parent = nullptr);
    ~enter_inf();
public slots:
    void get_inf_eljur(const QStringList);
    void get_inf_yanDisk(const QStringList);
    void get_inf_aboutSch(const QStringList);
signals:
    void open_window_eljur();
    void open_window_yandex_disk();
    void open_window_about_school();
    void infEljur_Correct();
    void infEljur_InCorrect();
    void infYanDisk_Correct();
    void infYanDisk_InCorrect();
    void infAbout_ready();
    void inf_ready();
    void sendData(const QStringList, const QStringList, const QStringList, const QString);
};

#endif // ENTER_INF_H
