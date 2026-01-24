#include "timeofapp.h"
#include <QTimer>
#include <QQuickItem>

TimeofApp::TimeofApp(QQuickView *view, QObject *parent) : QObject(parent), m_view(view)
{
    currentDay = TimeofApp::returnTime().date();
    connect(this, &TimeofApp::timeChanged, this, &TimeofApp::check_current_day);
    connect(this, &TimeofApp::timeChanged, this, &TimeofApp::set_time_interface);
    QTimer::singleShot(0, this, &TimeofApp::set_time_interface);
}

QDateTime TimeofApp::currentTime = QDateTime::currentDateTime();

QDateTime TimeofApp::returnTime()
{
    return currentTime;
}

void TimeofApp::updateTime()
{
    currentTime = QDateTime::currentDateTime();
    emit timeChanged();
}

void TimeofApp::check_current_day()
{
    if (currentDay != TimeofApp::returnTime().date())
    {
        currentDay = TimeofApp::returnTime().date();
        emit dayChanged();
    }
}

void TimeofApp::set_time_interface()
{
    if (!timeWidjet || !dataWidjet)
        if (m_view.data())
            if(m_view->rootObject())
            {
                QQuickItem *window = qobject_cast<QQuickItem*>(m_view->rootObject());
                timeWidjet = window->findChild<QObject*>("timeLabel");
                dataWidjet = window->findChild<QObject*>("data");
            }
    timeWidjet->setProperty("text", TimeofApp::returnTime().toString("hh:mm"));
    dataWidjet->setProperty("text", TimeofApp::returnTime().toString("dd.MM.yyyy"));
}
