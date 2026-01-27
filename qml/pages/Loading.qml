import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.Layouts 1.0
import Nemo.KeepAlive 1.2

Page
{
    allowedOrientations: Orientation.LandscapeMask;

    Rectangle
    {
        anchors.fill: parent
        color: "#445e5f"
    }
    DisplayBlanking
    {
        id: displayBlanking
        preventBlanking: true
    }

    FontLoader
    {
        id: uncage
        source: "qrc:/fonts/ofont.ru_Uncage.ttf"
    }

    Rectangle
    {
        id: appName
        width: parent.width * 0.7
        height: parent.height * 0.125
        opacity : 0.5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height / 3
        border.color: "black"
        border.width: 2
        color: "#5a5e5d"
    }

    Text
    {
        text: "Информационый Планшет"
        font.pixelSize:100
        font.bold: true
        color: "#f1eded"
        anchors.horizontalCenter: appName.horizontalCenter
        anchors.verticalCenter: appName.verticalCenter
    }

    BusyIndicator
    {
        id: busyIndicator
        running: true
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height * 0.7
        width: 60
        height: 60
        color: "#f1eded"
    }
    Text {
        id: logi
        text: ""
        horizontalAlignment: Text.AlignHCenter
        anchors.top: busyIndicator.top
        anchors.topMargin: busyIndicator.height + 20
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#f1eded"
    }

    Connections
    {
        target: AppController

        onCheckInternetToQml:
        {
            logi.text = "Проверка интернета"
        }
        onNotInternetAndReserveInfOfSchool:
        {
            logi.text = "Нет интернета и резервных данных о школе\nПодключите интернет"
        }

        onNotInternetAndReserveScheduleToQml:
        {
            logi.text = "Нет интернета и резервного расписания\nПодключите интернет"
        }

        onNotReserveImagesToQml:
        {
            logi.text = "Нет интернета и резервных фоток\nПодключите интернет"
        }
        onStartDownloadScheduleToQml:
        {
            logi.text = "Загрузка расписания"
        }

        onStartDownloadPicturesToQml:
        {
            logi.text = "Загрузка изображений"
        }
        onStartDownloadReserveScheduleAndCheckReservePicturesToQml:
        {
            logi.text = "Загрузка резервного расписания и проверка резервных изображений"
        }

        onInformationReady:
        {
            busyIndicator.running = false
            pageStack.replaceAbove(null, Qt.resolvedUrl("../pages/InputCab.qml"));
        }
    }

    Connections
    {
        target: enterInf

        onOpen_window_eljur:
        {
             pageStack.replaceAbove(null, Qt.resolvedUrl("enter_eljur.qml"));
        }

        onOpen_window_yandex_disk:
        {
            pageStack.replaceAbove(null, Qt.resolvedUrl("enter_yandexDisk.qml"));
        }

        onOpen_window_about_school:
        {
            pageStack.replaceAbove(null, Qt.resolvedUrl("enter_about.qml"));
        }
    }
}
