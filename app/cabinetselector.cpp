#include "cabinetselector.h"

CabinetSelector::CabinetSelector(QStringList adminCab, QObject *parent) : QObject(parent), adminCab(adminCab)
{
    m_selectedCabinet = "";
    m_cabinetModel = new QStringListModel(this);
    qDebug() << "CabinetSelector initialized";
}

CabinetSelector::~CabinetSelector()
{
    delete m_cabinetModel; // Очистка модели
}

void CabinetSelector::setSelectedCabinet(const QString &cabinet) {
    if (m_selectedCabinet != cabinet) {
        m_selectedCabinet = cabinet;
        qDebug() << "Selected cabinet changed to:" << cabinet;
        emit selectedCabinetChanged(m_selectedCabinet);
        emit finished();
    }
}

void CabinetSelector::setCabinetModel() {
    QStringList cabinets = collectInformationSchool::return_rooms();
    for (const QString &cab : adminCab)
        cabinets.append(cab);
    cabinets.sort();
    qDebug() << "Updating cabinet model with cabinets:" << cabinets;
    m_cabinetModel->setStringList(cabinets);
}

QStringListModel *CabinetSelector::cabinetModel() const
{
    return m_cabinetModel;
}
