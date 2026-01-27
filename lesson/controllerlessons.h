#ifndef CONTROLLERLESSONS_H
#define CONTROLLERLESSONS_H
#include "time/timeofapp.h"
#include "lesson.h"
const int MAXLESSONS = 10;

class ControllerLessons : public QObject
{
    Q_OBJECT
private:
    QPointer<QQuickView> m_view;
    int numLessons;
    int currentLesson;
    const QStringList AboutSch;
    bool isAnySelectedLes;
    lesson* lessons[MAXLESSONS];
    QStringList nameTeacher;
    QStringList nameLesson;
    QStringList group;
    QStringList klass;
    QStringList root;
    QStringList time;
    QStringList numLesson;
    QJsonObject set_num_to_outLes(const QJsonObject& les, const QJsonArray& lesInCab);
    QJsonArray check_replace_lessons(const QJsonArray& lessons);
public:
    explicit ControllerLessons(const QStringList& About, QQuickView* view, QObject *parent = nullptr);
    ~ControllerLessons();
public slots:
    void get_lessons(const QJsonObject&, const QString&);
    void check_time_in_range_of_lesson();
    void display();
private slots:
    void handleLessonsReady(const QList<lesson*> &newLessons);
    void check_simillar_lessons(const QList<lesson *> &newLessons);
    void set_lessons();
    void find_next_lesson();
    void select();
    void unselect();
signals:
    void lessonsReady();
    void setLessonsFinished();
    void selectLesson();
    void unSelectLesson();
    void studyDayFinished();
    void findNextLessonFinished();
    void connectTimerToCheck();
    void setPictureTeacher(const lesson *les);
    void unsetPictureTeacher();
};
#endif // CONTROLLERLESSONS_H
