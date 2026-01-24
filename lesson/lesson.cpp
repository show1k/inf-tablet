#include "lesson.h"
#include <QJsonObject>
#include <QDebug>
#include <QSet>

lesson::lesson(QObject *parent) : QObject(parent){}

void lesson::set_nameLesson(QString name)
{ nameLesson = name; }

void lesson::set_nameTeacher(QString name)
{ nameTeacher = name; }

void lesson::set_nameClass(QString name)
{ nameClass = name; }

void lesson::set_group(QString grp)
{ group = grp; }

void lesson::set_numLesson(int num)
{ numLesson = num; }

void lesson::set_startLesson(QTime start)
{ startLesson = start; }

void lesson::set_endLesson(QTime end)
{ endLesson = end; }

void lesson::set_cancel(bool c)
{ cancel = c; }

void lesson::set_information_of_lesson(const QJsonObject &lesson)
{
    nameLesson = lesson["name"].toString();
    nameReplace = lesson["name_replace"].toString();
    nameTeacher = lesson["teacher"].toString();
    nameClass = lesson["class"].toString();
    group = lesson["grp"].toString();
    numLesson = lesson["num"].toString().toInt();
    startLesson = QTime::fromString(lesson["starttime"].toString(), "hh:mm:ss");
    endLesson = QTime::fromString(lesson["endtime"].toString(), "hh:mm:ss");
    if (lesson.find("cancel") != lesson.end())
    {
        if (lesson["cancel"].toString() == "true")
            cancel = true;
        else
            cancel = false;
    }
    else
        cancel = false;
}

lesson* lesson::sum_lessons(const QList<lesson *> lessons)
{
    qDebug() << lessons;
    lesson *newLesson = new lesson;
    QString numClass;
    QString grpClass;
    QString letterClass;
    QSet<QString> groups;

    for (lesson* l : lessons)
    {
        QString num;
        QString nameClass = l->return_nameClass();
        QString grp = l->return_group();
        groups.insert(grp);
        for (int i = 0; i < nameClass.count(); i++)
        {
            QChar n = nameClass[i];
            if (n.isDigit())
                num += n;
            else
                letterClass += n;
        }
        numClass = num;
    }

    if (groups.count() > 1)
    {
        QSet<QString>::Iterator iter = groups.begin();
        grpClass = *iter;
        for (iter = groups.begin() + 1; iter != groups.end(); iter++)
            grpClass += " и " + *iter;
    }
    else
        grpClass = *groups.begin();

    QString nameClass = numClass + letterClass;
    newLesson->set_nameLesson(lessons[0]->return_nameLesson());
    newLesson->set_nameTeacher(lessons[0]->return_nameTeacher());
    newLesson->set_nameClass(nameClass);
    newLesson->set_group(grpClass);
    newLesson->set_numLesson(lessons[0]->return_numLesson());
    newLesson->set_startLesson(lessons[0]->return_startLesson());
    newLesson->set_endLesson(lessons[0]->return_endLesson());
    newLesson->set_cancel(lessons[0]->return_cancel());

    return newLesson;
}

void lesson::display()
{
    qDebug() << nameLesson << " " << nameReplace << " " << nameClass << " " << numLesson << '\n' << startLesson << "-" << endLesson << cancel << group;
}
