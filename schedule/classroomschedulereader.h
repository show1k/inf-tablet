#ifndef CLASSROOMSCHEDULEREADER_H
#define CLASSROOMSCHEDULEREADER_H
#include <QJsonObject>
#include "time/timeofapp.h"

class ClassroomScheduleReader : public QObject
{
    Q_OBJECT
private:
    void process_classroom_schedule(const QJsonObject &class_schedule);
    QJsonObject classrooms_schedule;
public:
    explicit ClassroomScheduleReader(QJsonObject &classrooms_schedule_m, QObject *parent = nullptr);
public slots:
    void read_schedule_for_current_day();
signals:
    void classroomScheduleLoaded(const QJsonObject &classrooms_schedule);
    void notReserveSchedule();
};

#endif // CLASSROOMSCHEDULEREADER_H
