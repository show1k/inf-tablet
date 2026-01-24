#include "internetconnection.h"
#include <QNetworkRequest>
#include <QTimer>
#include <QNetworkReply>

internetConnection::internetConnection(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void internetConnection::check_internet_connection()
{
    QNetworkRequest request(QUrl("https://api.github.com"));
    QNetworkReply *reply = manager->get(request);

    QTimer *timer = new QTimer(this);
    timer->setSingleShot(true);

    QMetaObject::Connection connection = connect(manager, &QNetworkAccessManager::finished, this, [=]()
    {
        timer->stop();
        timer->deleteLater();

        if (reply->error() == QNetworkReply::NoError)
        {
             qDebug() << "internet_connected";
            emit internet_connected();
        }
        else
        {
            qDebug() << "internet_disconnected:";
            emit internet_disconnected();
        }
        reply->deleteLater();
        emit finish_check();
    });

    connect(timer, &QTimer::timeout, this, [=]()
    {
        if(reply->isRunning())
        {
            disconnect(connection);
            reply->abort();
            qDebug() << "internet_disconnected:";
            emit internet_disconnected();
            reply->deleteLater();
        }
        timer->deleteLater();
        emit finish_check();
    });

    timer->start(2000);
}
