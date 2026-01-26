#include "controllerlessons.h"
#include <QTimer>
#include <QQuickItem>
#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QMap>

ControllerLessons::ControllerLessons(const QString avgtime, const int avgles, QQuickView * view, QObject *parent) : QObject(parent), m_view(view), avgTime(avgtime), avgLes(avgles)
{
    connect(this, &ControllerLessons::lessonsReady, this, &ControllerLessons::set_lessons);
    connect(this, &ControllerLessons::setLessonsFinished, this, [=]()
    {
       connect(this, &ControllerLessons::findNextLessonFinished, this, [=]()
       {
           connect(this, &ControllerLessons::selectLesson, this, &ControllerLessons::select);
           connect(this, &ControllerLessons::unSelectLesson, this, &ControllerLessons::unselect);
           QTimer::singleShot(0, this, &ControllerLessons::check_time_in_range_of_lesson);
           emit connectTimerToCheck();
       });
       QTimer::singleShot(0, this, &ControllerLessons::find_next_lesson);
    });

    for (int i = 0; i < MAXLESSONS; ++i)
        lessons[i] = nullptr;
    numLessons = 0;
    currentLesson = 0;
    isAnySelectedLes = false;

    nameTeacher = QStringList{"teacher0", "teacher1", "teacher2", "teacher3", "teacher4",
                            "teacher5", "teacher6", "teacher7", "teacher8", "teacher9"};
    nameLesson = QStringList{"yrok0", "yrok1", "yrok2", "yrok3", "yrok4",
                           "yrok5", "yrok6", "yrok7", "yrok8", "yrok9"};
    group = QStringList{"group0", "group1", "group2", "group3", "group4",
                            "group5", "group6", "group7", "group8", "group9"};
    klass = QStringList{"klass0", "klass1", "klass2", "klass3", "klass4",
                            "klass5", "klass6", "klass7", "klass8", "klass9"};
    root = QStringList{"root0", "root1", "root2", "root3", "root4",
                           "root5", "root6", "root7", "root8", "root9"};
    time = QStringList{"time0", "time1", "time2", "time3", "time4",
                           "time5", "time6", "time7", "time8", "time9"};
    numLesson = QStringList{"les0", "les1", "les2", "les3", "les4", "les5",
            "les6", "les7", "les8", "les9"};
}

ControllerLessons::~ControllerLessons()
{
    QQuickItem *window = qobject_cast<QQuickItem*>(m_view->rootObject());
    for (int i = 0; i < numLessons; i++)
    {
        int num = lessons[i]->return_numLesson();
        QObject* teacher = window->findChild<QObject*>(nameTeacher[num]);
        QObject* lesson = window->findChild<QObject*>(nameLesson[num]);
        QObject* Class = window->findChild<QObject*>(klass[num]);
        QObject* grp = window->findChild<QObject*>(group[num]);
        QObject* rot = window->findChild<QObject*>(root[num]);
        QObject* rangeLesson = window->findChild<QObject*>(time[num]);

        teacher->setProperty("text", "");
        lesson->setProperty("text", "");
        Class->setProperty("text", "");
        if (grp)
            grp->setProperty("text", "");
        rot->setProperty("klassRowSpan", 1);

        rangeLesson->setProperty("text", "");
    }
    for (int i = 0; i < MAXLESSONS; ++i)
        if (lessons[i] != nullptr)
            delete lessons[i];
}

void ControllerLessons::get_lessons(const QJsonObject &schedule, const QString &cabinet)
{
    QFuture< QList<lesson*> > future = QtConcurrent::run([=]() -> QList<lesson*>
    {
        QList<lesson*> newLessons;
        QJsonArray lessonsInCabinet = check_replace_lessons(schedule[cabinet].toArray());
        for (int i = 0; i < lessonsInCabinet.size() && i < MAXLESSONS; ++i)
        {
            QJsonObject les = lessonsInCabinet[i].toObject();
            if (les.find("num") == les.end())
            {
                qDebug() << "start find num for OUTLES";
                les = set_num_to_outLes(les, lessonsInCabinet);
            }
            if (!les.isEmpty())
            {
                lesson *l = new lesson;
                l->set_information_of_lesson(les);
                newLessons.append(l);
            }
        }
        for (int i = 0; i < newLessons.count(); i++)
            newLessons[i]->display();
        return newLessons;
    });

    QFutureWatcher< QList<lesson*> > *watcher = new QFutureWatcher< QList<lesson*> >(this);
    connect(watcher, &QFutureWatcher< QList<lesson*> >::finished, this, [=]()
    {
        check_simillar_lessons(watcher->result());
        watcher->deleteLater();
    });

    watcher->setFuture(future);
}

QJsonArray ControllerLessons::check_replace_lessons(const QJsonArray &lessons)
{
    //qDebug() << lessons;
    QJsonArray replaces;
    QJsonArray replacelessons;
    QJsonArray newLessons;
    QList<int> continueIndex;
    for (int i = 0; i < lessons.count(); i++)
    {
        QJsonObject s = lessons[i].toObject();
        if (s.find("cancel") != s.end())
        {
            replaces.append(s);
            continueIndex.append(i);
        }
    }
    //qDebug() << replaces << continueIndex;
    for (int i = 0; i < replaces.count(); i++)
    {
        QJsonObject replace = replaces[i].toObject();
        for (int j = 0; j < lessons.count(); j++)
        {
            if (continueIndex.contains(j))
                continue;
            QJsonObject les = lessons[j].toObject();
            if (replace["num"].toString() == les["num"].toString() && replace["class"].toString() == les["class"].toString())
            {
                QJsonObject reples = les;
                reples["name_replace"] = replace["oldteacher"].toString() + " замена на " + replace["newteacher"].toString();
                if (reples["name"].toString() != replace["newlesson"].toString())
                    reples["name"] = replace["oldlesson"].toString() + " заменена на " + replace["newlesson"].toString();
                reples.insert("cancel", replace["cancel"].toString());
                replacelessons.append(reples);
                continueIndex.append(j);
                break;
            }
        }
    }
    //qDebug() << continueIndex << replacelessons;
    for (int i = 0; i < lessons.count(); i++)
    {
        if (continueIndex.contains(i))
            continue;
        newLessons.append(lessons[i]);
    }
    for (const QJsonValue& a : replacelessons)
    {
        QJsonObject s = a.toObject();
        newLessons.append(s);
    }
    //qDebug() << newLessons;
    return newLessons;
}

QJsonObject ControllerLessons::set_num_to_outLes(const QJsonObject &les, const QJsonArray &lesInCab)
{
    QTime startOutLes = QTime::fromString(les["starttime"].toString(), "hh:mm:ss");
    QTime max = QTime::fromString("00:01", "hh:mm");
    QTime min = QTime::fromString("23:59", "hh:mm");
    bool noLes = false;
    int minNum = 100, maxNum = 0;
    int num;
    for (const QJsonValue& a : lesInCab)
    {
        //qDebug() << a;
        QJsonObject m = a.toObject();
        //qDebug() << m["num"] << m["num"].toString().toInt();
        int tempNum;

        if (m.find("num") != m.end())
        {
            tempNum = m["num"].toString().toInt();
            noLes = true;
        }
        else
            continue;

        QTime temp = QTime::fromString(m["starttime"].toString(), "hh:mm:ss");
        qDebug() << "tempNum =" << tempNum;
        if (temp != startOutLes && temp < min)
        {
            //qDebug() << "tempNum 1(" << tempNum << "minNum" << minNum;
            if (minNum > tempNum)
                minNum = tempNum;
            //qDebug() << "minNim" << minNum;
            min = temp;
        }
        else if (temp != startOutLes && temp > max)
        {
            //qDebug() << "tempNum 2(" << tempNum << "maxNum" << maxNum;
            if (maxNum < tempNum)
                maxNum = tempNum;
            //qDebug() << "maxNum" << maxNum;
            max = temp;
        }
    }
    if (!noLes)
    {
        minNum = avgLes;
        maxNum = avgLes;
        min =  QTime::fromString(avgTime, "hh:mm");
        max =  QTime::fromString(avgTime, "hh:mm");
    }
    if (startOutLes < min)
    {
        int range = startOutLes.msecsTo(min) / 1000 / 60;
        qDebug() << "startOutLes =" << startOutLes << "minNum =" << minNum << "min =" << min << "range =" << range << "range / 45 =" << range / 45;
        num = minNum - (range - (range / 45) * 5) / 45;
    }
    else
    {
        int range = max.msecsTo(startOutLes) / 1000 / 60;
        qDebug() << "startOutLes =" << startOutLes << "maxNum =" << maxNum << "max =" << max << "range =" << range << "range / 45 =" << range / 45;
        num = maxNum + (range - (range / 45) * 5) / 45;
    }
    qDebug() << "new num for outLes" << num;
    QJsonObject t = les;
    t["num"] = QString::number(num);
    qDebug() << t;
    return t;
}

void ControllerLessons::check_time_in_range_of_lesson()
{
    if (currentLesson < numLessons && numLessons != 0)
    {
        QTime startLes = lessons[currentLesson]->return_startLesson();
        QTime endLes = lessons[currentLesson]->return_endLesson();
        QTime curTime = TimeofApp::returnTime().time();
        if (!isAnySelectedLes)
        {
            if (startLes <= curTime && endLes >= curTime)
            {
                isAnySelectedLes = true;
                qDebug() << "Lesson select:" << currentLesson;
                emit selectLesson();
                emit setPictureTeacher(lessons[currentLesson]);
            }
            else
            {
                //qDebug() << "время начала урока " << startLes << " время конца " << endLes << " время сейчас " << curTime;
            }
        }
        if (curTime > endLes)
        {
            qDebug() << "Lesson unselect:" << currentLesson;
            currentLesson++;
            isAnySelectedLes = false;
            emit unSelectLesson();
            emit unsetPictureTeacher();
        }
    }
    else
    {
        qDebug() << "study Day Finished";
        emit studyDayFinished();
    }
}

void ControllerLessons::handleLessonsReady(const QList<lesson*> &newLessons)
{
    for (lesson *l : newLessons)
    {
        if (numLessons < MAXLESSONS)
            lessons[numLessons++] = l;
        else
            delete l;
    }
    for (int i = 0; i < numLessons; i++)
        qDebug() << lessons[i];
    emit lessonsReady();
}

void ControllerLessons::check_simillar_lessons(const QList<lesson *> &newLessons)
{
    if (newLessons.isEmpty())
    {
        QTimer::singleShot(0, this, [=]
        {
           handleLessonsReady(newLessons);
        });
        return;
    }

    QList<lesson *> newLessons2 = newLessons;
    newLessons2.append(nullptr);
    lesson *lastLes = nullptr;
    QList<lesson*> lessons;
    QList<lesson*> sortedLessons;
    QList<lesson*> unitedLessons;
    bool first = true;
    for (lesson *l : newLessons2)
    {
        //qDebug() << l;
        if (newLessons2.count() > 1)
        {
            if (lastLes != nullptr)
            {
                if (l && lastLes->return_numLesson() == l->return_numLesson())
                {
                    if (first)
                    {
                        unitedLessons.append(lastLes);
                        unitedLessons.append(l);
                        first = false;
                    }
                    else
                        unitedLessons.append(l);
                }
                else
                {
                    if (!unitedLessons.isEmpty())
                    {
                        lessons.append(lesson::sum_lessons(unitedLessons));
                        qDeleteAll(unitedLessons);
                        unitedLessons.clear();
                        first = true;
                    }
                    else
                    {
                        if (lastLes)
                            lessons.append(lastLes);
                    }
                }
            }
            lastLes = l;
        }
        else
        {
            lessons.append(l);
        }
    }
    //qDebug() << "complete_check";
    QMap<int, lesson*> m;
    for (int i = 0; i < lessons.size(); i++)
    {
        //qDebug() << lessons[i];
        int num = lessons[i]->return_numLesson();
        m[num] = lessons[i];
    }

    for (int i = 0; i < 10; i++)
    {
        QMap<int, lesson*>::Iterator iter;
        iter = m.find(i);
        if (iter != m.end())
        {
            sortedLessons.append(iter.value());
        }
    }

    QTimer::singleShot(0, this, [=]
    {
       handleLessonsReady(sortedLessons);
    });
}

void ControllerLessons::set_lessons()
{
    if (!m_view)
        return;
    else if (!m_view->rootObject())
        return;
    QQuickItem *window = qobject_cast<QQuickItem*>(m_view->rootObject());

    for (int i = 0; i < numLessons; i++)
    {
        int num = lessons[i]->return_numLesson();
        QObject* teacher = window->findChild<QObject*>(nameTeacher[num]);
        QObject* lesson = window->findChild<QObject*>(nameLesson[num]);
        QObject* Class = window->findChild<QObject*>(klass[num]);
        QObject* grp = window->findChild<QObject*>(group[num], Qt::FindChildrenRecursively);
        QObject* rot = window->findChild<QObject*>(root[num]);
        QObject* rangeLesson = window->findChild<QObject*>(time[num]);

        if (lessons[i]->return_cancel() == true)
            teacher->setProperty("text", "Урок отменен");
        else if (lessons[i]->return_nameReplace().isEmpty())
            teacher->setProperty("text", lessons[i]->return_nameTeacher());
        else
            teacher->setProperty("text", lessons[i]->return_nameReplace());

        lesson->setProperty("text", lessons[i]->return_nameLesson());
        Class->setProperty("text", lessons[i]->return_nameClass());

        if (lessons[i]->return_group() != "")
        {
            if (grp)
                grp->setProperty("text", lessons[i]->return_group());
            rot->setProperty("klassRowSpan", 1);
        }
        else
            rot->setProperty("klassRowSpan", 2);

        QString rangeTime = lessons[i]->return_startLesson().toString("hh:mm") + "-" + lessons[i]->return_endLesson().toString("hh:mm");
        rangeLesson->setProperty("text", rangeTime);
    }
    emit setLessonsFinished();
}

void ControllerLessons::find_next_lesson()
{
    if (numLessons != 0)
    {
        while (lessons[currentLesson]->return_endLesson() < TimeofApp::returnTime().time())
        {
            if (currentLesson == numLessons - 1)
            {
                currentLesson++;
                break;
            }
            currentLesson++;
        }
    }
    qDebug() << currentLesson;
    emit findNextLessonFinished();
}

void ControllerLessons::select()
{
    if (!m_view)
        return;
    else if (!m_view->rootObject())
        return;
    QQuickItem *window = qobject_cast<QQuickItem*>(m_view->rootObject());
    int num = lessons[currentLesson]->return_numLesson();

    QObject* les = window->findChild<QObject*>(numLesson[num]);
    QObject* rot = window->findChild<QObject*>(root[num]);

    les->setProperty("color", "#77DDE7");
    rot->setProperty("color", "#77DDE7");
}

void ControllerLessons::unselect()
{
    if (!m_view)
        return;
    else if (!m_view->rootObject())
        return;
    QQuickItem *window = qobject_cast<QQuickItem*>(m_view->rootObject());
    int num = lessons[currentLesson - 1]->return_numLesson();

    QObject* les = window->findChild<QObject*>(numLesson[num]);
    QObject* rot = window->findChild<QObject*>(root[num]);

    les->setProperty("color", "white");
    rot->setProperty("color", "white");
}

void ControllerLessons::display()
{
    for(int i = 0; i < numLessons; i++)
        lessons[i]->display();
}
