#ifndef TIMEOFAPP_H
#define TIMEOFAPP_H
#include <QObject>
#include <QDate>
#include <QPointer>
#include <QQuickView>

class TimeofApp : public QObject
{
    Q_OBJECT
private:
    static QDateTime currentTime;
    QDate currentDay;
    QPointer<QObject> timeWidjet;
    QPointer<QObject> dataWidjet;
    QPointer<QQuickView> m_view;
public:
    explicit TimeofApp(QQuickView* view, QObject *parent = nullptr);
    static QDateTime returnTime();
public slots:
    void updateTime();
    void check_current_day();
    void set_time_interface();
signals:
    void timeChanged();
    void dayChanged();
};

#endif // TIMEOFAPP_H
