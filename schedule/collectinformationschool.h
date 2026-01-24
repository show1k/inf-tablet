#ifndef COLLECTINFORMATIONSCHOOL_H
#define COLLECTINFORMATIONSCHOOL_H
#include "school.h"
#include "internet_connection/internetconnection.h"

class collectInformationSchool : public QObject
{
    Q_OBJECT
private:
    QPointer<internetConnection> intConnect;
    QPointer<QNetworkAccessManager> manager;
    QPointer<School> lyceum;
    static QStringList classes;
    static QStringList rooms;
    bool inf;
    QSet<QString> roomsSet;
    int pendingRequests;
    void get_classses(const QJsonObject&);
    void get_rooms();
public:
    explicit collectInformationSchool(internetConnection* ptrIntcon, QObject *parent = nullptr);
    static QStringList return_classes();
    static QStringList return_rooms();
    void check_infReady();
private slots:
    void request_classes();
    void request_rooms();
    void get_roomsSet(const QJsonObject&, const QString&);
    void save_classes();
    void save_rooms();
    void read_information();
signals:
    void infReady();
    void noData();
};

#endif // COLLECTINFORMATIONSCHOOL_H
