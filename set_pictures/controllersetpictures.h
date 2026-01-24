#ifndef CONTROLLERSETPICTURES_H
#define CONTROLLERSETPICTURES_H
#include <QQuickView>
#include <QPointer>
#include "lesson/lesson.h"

class ControllerSetPictures : public QObject
{
    Q_OBJECT
private:
    QPointer<QQuickView> m_view;
    QList<QString> m_imagePaths;
    QPointer<QTimer> m_timer;
    QList<int> m_shownIndices;
    int m_currentIndex;
    QString pathDirImages;
    int m_previousIndex;
    bool m_isRunning;
public:
    explicit ControllerSetPictures(QQuickView* view, QObject *parent = nullptr);
    ~ControllerSetPictures();
    void setImagePaths(const QList<QString>& paths);
public slots:
    void get_name_file_teacher(const lesson *les);
    void set_admin_image(const QString& name);
    void clear_image();
    void startImageRotation();
    void stopImageRotation();
private slots:
    void set_picture_teacher(const QString& name_file);
    void set_picture_break(const QString& name_file);
    void updateImage();
signals:
    void setPictureTeacher(const QString&);
    void triggerFadeAnimation();
    void stopFadeAnimation();
};

#endif // CONTROLLERSETPICTURES_H
