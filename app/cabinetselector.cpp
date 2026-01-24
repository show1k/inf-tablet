#include "cabinetselector.h"
#include "globalVar.h"

CabinetSelector::CabinetSelector(QObject *parent) : QObject(parent)
{
    m_selectedCabinet = "";
    m_cabinetModel = new QStringListModel(this);
    qDebug() << "CabinetSelector initialized";
}

CabinetSelector::~CabinetSelector()
{
    delete m_cabinetModel; // Очистка модели
}

QStringList CabinetSelector::get_add_cab()
{
    return admin_cabs;
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
    QStringList add_cab = get_add_cab();
    QStringList cabinets = collectInformationSchool::return_rooms();
    for (const QString &cab : add_cab)
        cabinets.append(cab);
    cabinets.sort();
    qDebug() << "Updating cabinet model with cabinets:" << cabinets;
    m_cabinetModel->setStringList(cabinets);
}

QStringListModel *CabinetSelector::cabinetModel() const
{
    return m_cabinetModel;
}
