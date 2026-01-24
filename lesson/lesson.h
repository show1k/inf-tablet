#ifndef LESSON_H
#define LESSON_H
#include <QObject>
#include <QTime>

class lesson : public QObject
{
    Q_OBJECT
private:
    QString nameLesson;
    QString nameReplace;
    QString nameTeacher;
    QString nameClass;
    QString group;
    bool cancel;
    int numLesson;
    QTime startLesson;
    QTime endLesson;
public:
    explicit lesson(QObject *parent = nullptr);
    void set_nameLesson(QString);
    void set_nameTeacher(QString);
    void set_nameClass(QString);
    void set_group(QString);
    void set_numLesson(int);
    void set_startLesson(QTime);
    void set_endLesson(QTime);
    void set_cancel(bool c);
    void set_information_of_lesson(const QJsonObject&);
    QString return_nameLesson() const {return nameLesson;}
    QString return_nameReplace() const {return nameReplace;}
    QString return_nameTeacher() const {return nameTeacher;}
    QString return_nameClass() const {return nameClass;}
    QString return_group() const {return group;}
    int return_numLesson() const {return numLesson;}
    QTime return_startLesson() const {return startLesson;}
    QTime return_endLesson() const {return endLesson;}
    bool return_cancel() const {return cancel;}
    static lesson* sum_lessons(const QList<lesson*>);
    void display();
signals:

};

#endif // LESSON_H
