#include "yandexdirectorychecker.h"
#include <QDir>
#include <QThread>
#include <QDebug>
#include <QStandardPaths>

YandexDirectoryChecker::YandexDirectoryChecker(QObject *parent) : QObject(parent)
{
    QString homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString publicPath = homePath + "/Public/DataInfTablet";
    teacherDirPath = publicPath + "/yandeximageteachers";
    breakDirPath = publicPath + "/yandeximagesbreak";
    QThread *thread = new QThread(); // No parent
    moveToThread(thread);
    thread->start();
}

void YandexDirectoryChecker::check_directories()
{
    QDir dirteacher(teacherDirPath);
    if (!dirteacher.exists()) {
        qDebug() << "teacher folder not exist";
        emit directoryCheckFailed();
        return;
    }

    QDir dirbreak(breakDirPath);
    if (!dirteacher.exists()) {
        qDebug() << "break folder not exist";
        emit directoryCheckFailed();
        return;
    }

    QStringList breakFiles = dirbreak.entryList(QDir::Files);
    if (breakFiles.isEmpty())
    {
        qDebug() << "break folder empty";
        emit allDirectoriesReady(QStringList()); // Empty list if no files
    }
    else
    {
        QStringList breakPaths;
        for (const QString &filename : breakFiles)
        {
            breakPaths.append(breakDirPath + "/" + filename);
        }
        emit allDirectoriesReady(breakPaths);
    }
}
