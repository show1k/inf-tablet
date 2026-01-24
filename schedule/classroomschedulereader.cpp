#include "classroomschedulereader.h"
#include <QDebug>
#include <QDir>
#include <QJsonDocument>
#include <QJsonArray>
#include <QStandardPaths>

ClassroomScheduleReader::ClassroomScheduleReader(QJsonObject &classrooms_schedule_m, QObject *parent) : QObject(parent), classrooms_schedule(classrooms_schedule_m)
{
}

void ClassroomScheduleReader::read_schedule_for_current_day()
{
    // Определяем текущий день недели
    QLocale enLocale(QLocale::English);
    QDate currentDate = TimeofApp::returnTime().date();
    QString dayOfWeek = enLocale.toString(currentDate, "dddd");
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    QDir dir(publicPath);
    QString filename = dir.filePath(QString("schedules/schedule_%1.json").arg(dayOfWeek));

    // Отладка: проверяем, какой файл пытаемся открыть
    qDebug() << "Attempting to read schedule file:" << filename;

    // Читаем JSON-файл
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open schedule file for" << dayOfWeek << ":" << file.errorString();
        emit notReserveSchedule();
        return;
    }

    // Парсим JSON
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (doc.isNull() || !doc.isObject()) {
        qWarning() << "Invalid JSON format in" << filename;
        emit notReserveSchedule();
        return;
    }

    QJsonObject class_schedule = doc.object();

    // Проверяем, пустой ли JSON
    /*if (class_schedule.isEmpty()) {
        qWarning() << "Class schedule is empty for" << dayOfWeek;
        emit notReserveSchedule();
        return;
    }*/

    // Обрабатываем расписание
    process_classroom_schedule(class_schedule);
}

void ClassroomScheduleReader::process_classroom_schedule(const QJsonObject &class_schedule)
{
    // Обрабатываем расписание для каждого класса
    for (int i = 0; i < classrooms_schedule.count(); i++)
    {
        QString classroom_name = classrooms_schedule.keys().at(i);

        // Инициализируем пустой массив для расписания аудитории
        QJsonArray cur_classroom_schedule;

        for (int j = 0; j < class_schedule.count(); j++)
        {
            QString classname = class_schedule.keys().at(j);
            QJsonArray cur_clas_schedule = class_schedule.value(classname).toArray();

            for (int k = 0; k < cur_clas_schedule.count(); k++)
            {
                QJsonObject cur_lesson = cur_clas_schedule[k].toObject();

                if (cur_lesson["room"].toString() == classroom_name)
                {
                    cur_lesson.insert("class", classname);
                    // Добавляем урок в массив
                    cur_classroom_schedule.append(cur_lesson);
                }
            }
        }
        classrooms_schedule[classroom_name] = cur_classroom_schedule;
    }

    // Отладка: выводим результат
    /*QStringList room_keys = classrooms_schedule.keys();
    for (int i = 0; i < room_keys.size(); ++i)
    {
        QString classroom_name = room_keys.at(i);
        qDebug() << "\n" << classroom_name << "\n" << classrooms_schedule.value(classroom_name) << "\n";
    }*/

    // Отправляем результат через сигнал
    emit classroomScheduleLoaded(classrooms_schedule);
}
