#ifndef YANDEXDIRECTORYCHECKER_H
#define YANDEXDIRECTORYCHECKER_H
#include <QObject>

class YandexDirectoryChecker : public QObject
{
    Q_OBJECT
private:
    QString teacherDirPath;
    QString breakDirPath;
public:
    explicit YandexDirectoryChecker(QObject *parent = nullptr);
public slots:
    void check_directories();
signals:
    void allDirectoriesReady(const QStringList &breakPaths);
    void directoryCheckFailed();
};
#endif // YANDEXDIRECTORYCHECKER_H
