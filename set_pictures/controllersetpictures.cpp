#include "controllersetpictures.h"
#include <QDir>
#include <QTimer>
#include <QDebug>
#include <QQuickItem>
#include <QStandardPaths>
ControllerSetPictures::ControllerSetPictures(QQuickView *view, QObject *parent) : QObject(parent), m_view(view)
{
    m_timer = new QTimer(this);
    m_isRunning = false;
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    pathDirImages = homePath + "/Public/DataInfTablet";
    connect(this, &ControllerSetPictures::setPictureTeacher, this, &ControllerSetPictures::set_picture_teacher);
    connect(m_timer, &QTimer::timeout, this, &ControllerSetPictures::updateImage);
}

ControllerSetPictures::~ControllerSetPictures()
{
    m_timer->stop();
    clear_image();
}

void ControllerSetPictures::get_name_file_teacher(const lesson *les)
{
    QString path_to_dir = pathDirImages + "/yandeximageteachers/";
    QString nameTeacher = les->return_nameTeacher();
    QString filter = nameTeacher + ".*";

    QDir dir(path_to_dir);
    QStringList filters;
    filters << filter;
    QStringList files = dir.entryList(filters, QDir::Files);

    if (!files.empty())
    {
        QString path_to_file = path_to_dir + files.first();
        QFileInfo fileInfo(path_to_file);
        if (fileInfo.exists())
            emit setPictureTeacher(path_to_file);
        else
            qDebug() << "file not exist" << path_to_file;
    }
    else
        qDebug() << "file with name" << nameTeacher << "not found in dir" << path_to_dir;
}

void ControllerSetPictures::set_admin_image(const QString &name)
{
    QString path_to_dir = pathDirImages + "/yandeximageteachers/";
    QString filter = name + ".*";

    QDir dir(path_to_dir);
    QStringList filters;
    filters << filter;
    QStringList files = dir.entryList(filters, QDir::Files);
    if (!files.empty())
    {
        QString path_to_file = path_to_dir + files.first();
        QFileInfo fileInfo(path_to_file);
        if (fileInfo.exists())
            emit setPictureTeacher(path_to_file);
        else
            qDebug() << "file not exist" << path_to_file;
    }
    else
        qDebug() << "file with name" << name << "not found in dir" << path_to_dir;
}

void ControllerSetPictures::clear_image()
{
    if (!m_view)
        return;
    else if (!m_view->rootObject())
        return;

    QQuickItem *window = qobject_cast<QQuickItem*>(m_view->rootObject());
    QObject* image = window->findChild<QObject*>("image_teacher");
    if (image)
    {
        qDebug() << "image clear";
        image->setProperty("source", "");
        emit
    }
    else
        qDebug() << image << "not found";
}

void ControllerSetPictures::set_picture_teacher(const QString& path_to_file)
{
    if (!m_view)
        return;
    else if (!m_view->rootObject())
        return;

    QQuickItem *window = qobject_cast<QQuickItem*>(m_view->rootObject());
    QObject* image = window->findChild<QObject*>("image_teacher");
    if (image)
    {
        qDebug() << "picture is seted" << path_to_file;
        image->setProperty("source", "file:///" + path_to_file);
    }
    else
        qDebug() << image << "not found";
}

void ControllerSetPictures::setImagePaths(const QList<QString>& paths)
{
    m_imagePaths = paths;
    qDebug() << m_imagePaths;
    m_currentIndex = 0;
    m_previousIndex = -1;
}

void ControllerSetPictures::startImageRotation()
{
    if (!m_imagePaths.isEmpty() && !m_isRunning) {
        m_isRunning = true;
        m_shownIndices.clear();
        m_currentIndex = qrand() % m_imagePaths.size();
        m_shownIndices.append(m_currentIndex);
        set_picture_break(m_imagePaths[m_currentIndex]);
        m_timer->start(15000); // 15 секунд
    }
}

void ControllerSetPictures::stopImageRotation()
{
    m_isRunning = false;
    m_timer->stop();
    emit stopFadeAnimation(); // Вызываем сигнал для остановки анимации
}

void ControllerSetPictures::updateImage()
{
    if (!m_isRunning || m_imagePaths.isEmpty())
        return;

    qDebug() << "updateImage";
    int newIndex;

    QList<int> unshownIndices;
    for (int i = 0; i < m_imagePaths.size(); i++)
    {
        if (!m_shownIndices.contains(i))
            unshownIndices.append(i);
    }


    if (unshownIndices.isEmpty())
    {
        m_previousIndex = m_currentIndex;
        m_shownIndices.clear();
        for (int i = 0; i < m_imagePaths.size(); i++)
            unshownIndices.append(i);
    }
    int r;
    do
    {
        r = qrand() % unshownIndices.size();
    }while(r == m_previousIndex);

    newIndex = unshownIndices[r];

    m_currentIndex = newIndex;
    m_shownIndices.append(m_currentIndex);
    m_previousIndex = -1;
    set_picture_break(m_imagePaths[m_currentIndex]);
}

void ControllerSetPictures::set_picture_break(const QString& name_file)
{
    if (m_view)
        if (m_view->rootObject())
        {
            QObject* image = m_view->rootObject()->findChild<QObject*>("image_teacher");
            if (image)
            {
                qDebug() << "image break seted" << name_file;
                image->setProperty("source", "file:///" + name_file);
                emit triggerFadeAnimation(); // Вызываем сигнал для анимации
            }
        }
}
