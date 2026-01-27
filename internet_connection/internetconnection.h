#ifndef INTERNETCONNECTION_H
#define INTERNETCONNECTION_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QPointer>

class internetConnection : public QObject
{
    Q_OBJECT
private:
    QPointer<QNetworkAccessManager> manager;
public:
    explicit internetConnection(QObject *parent = nullptr);
public slots:
    void check_internet_connection();
signals:
    void internet_connected();
    void internet_disconnected();
    void finish_check();
};

#endif // INTERNETCONNECTION_H
