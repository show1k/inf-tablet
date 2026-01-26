#ifndef CABINETSELECTOR_H
#define CABINETSELECTOR_H
#include <QStringListModel>
#include "schedule/collectinformationschool.h"

class CabinetSelector : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringListModel* cabinetModel READ cabinetModel CONSTANT)

private:
    QString m_selectedCabinet; // Текущий выбранный кабинет
    QStringListModel *m_cabinetModel; // Модель для списка кабинетов
    QStringList adminCab;


public:
    explicit CabinetSelector(QStringList adminCab, QObject *parent = nullptr);
    ~CabinetSelector();

    QStringListModel* cabinetModel() const;

public slots:
    void setCabinetModel(); // Обновление модели из schedule
    void setSelectedCabinet(const QString &cabinet);


signals:
    void selectedCabinetChanged(const QString &cabinet); // Сигнал с выбранным кабинетом
    void finished();
};
#endif // CABINETSELECTOR_H
