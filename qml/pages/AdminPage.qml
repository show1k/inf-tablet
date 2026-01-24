import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.Layouts 1.0
import Nemo.KeepAlive 1.2

Page
{
    allowedOrientations: Orientation.LandscapeMask;
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
        anchors.fill: parent
        color: "#445e5f"
        z: 0
    }

    Item {
        id: name
        width: parent.width * 0.3
        height: parent.height * 0.25
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: parent.height * 0.7
        anchors.leftMargin: parent.width * 0.02
        z: 2
        ColumnLayout {
            anchors.fill: parent
            spacing: 0

            // контейнер с текстом текущего времени
            Text {
                id: _text
                text: qsTr("")
                objectName: "timeLabel"
                color: "white"
                font.pixelSize: 80 // размер шрифта
                Layout.fillWidth: true
                Layout.fillHeight: true
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignBottom
                font.family: uncage.name // приминяем добавленный шрифт

            }

            // контейнер с текстом текущей даты
            Text {
                id: text9
                objectName: "data"
                text: qsTr("")
                color: "white"
                font.pixelSize: 35
                Layout.fillWidth: true
                Layout.fillHeight: true
                horizontalAlignment: Label.AlignHCenter
                verticalAlignment: Label.AlignTop
                font.family: uncage.name


            }
        }

    }
        // контейнер для логотипа
    Item
    {
        width: parent.width * 0.3
        height: parent.height * 0.25
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: parent.height * 0.05
        anchors.leftMargin: parent.width * 0.02
        z: 2
        Image
        {
            id: image
            objectName: "image"
            source: "qrc:/image/logo.jpeg"
            anchors.fill: parent
            fillMode: Image.PreserveAspectCrop //  метод для масштабирования и обрезания изображения
            z: 2
        }
    }

    Image
    {
        id: image2
        objectName: "image_teacher"
        source: ""
        anchors.fill: parent
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectCrop
        asynchronous: true // Загружает изображение в фоновом потоке
        z: 1
    }
}
