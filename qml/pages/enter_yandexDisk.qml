import QtQuick 2.0
import Sailfish.Silica 1.0
import Nemo.KeepAlive 1.2

Page {
    allowedOrientations: Orientation.LandscapeMask

    DisplayBlanking {
        id: displayBlanking
        preventBlanking: true
    }

    Rectangle {
        anchors.fill: parent
        color: "#445e5f"
    }

    FontLoader {
        id: uncage
        source: "qrc:/fonts/ofont.ru_Uncage.ttf"
    }

    property string textField: ""

    Rectangle {
        id: appName
        width: parent.width * 0.7
        height: parent.height * 0.125
        opacity: 0.5
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: parent.height / 20
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

    Rectangle {
        id: enter
        width: parent.width / 3 * 1.3
        height: parent.height / 3 * 2
        anchors.top: appName.bottom
        anchors.topMargin: parent.height / 20
        anchors.horizontalCenter: parent.horizontalCenter
        color: "#f1eded"
        radius: 60
        border.color: "black"
        border.width: 2

        Text {
            text: "Данные для Яндекс Диска"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height / 40
            color: "black"
            font.pixelSize: 40
        }

        Column {
            anchors.top: parent.top
            anchors.topMargin: parent.height / 10
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 40

            Rectangle {
                    id: field
                    width: enter.width / 3 * 2.5
                    height: enter.height / 10
                    anchors.horizontalCenter: enter.horizontalCenter
                    border.color: "black"
                    radius: 10
                    border.width: 1
                    color: "#5a5e5d"
                    opacity: 0.5

                    TextField {
                        text: textField
                        onTextChanged: textField = text
                        placeholderText: "Токен"
                        width: parent.width
                        anchors.top: parent.top
                        font.pixelSize: 40
                        color: "#5a5e5d"
                        opacity: 1
                    }
            }

            Item {
                width: enter.width / 3 * 2.5
                height: enter.height / 10 + (errorLabel.visible ? errorLabel.height : 0)

                // Текст ошибки — позиционируем над кнопкой
                Text {
                    id: errorLabel
                    text: "Заполните все поля ввода"
                    color: "red"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 30
                    anchors.bottom: button.top
                    visible: false
                }

                Text {
                    id: errorLabel2
                    text: "Неправильный токен"
                    color: "red"
                    anchors.horizontalCenter: parent.horizontalCenter
                    font.pixelSize: 30
                    anchors.bottom: button.top
                    visible: false
                }

                // Кнопка — фиксируем её внизу контейнера
                Button {
                    id: button
                    text: "Продолжить"
                    width: enter.width / 2
                    height: enter.height / 10
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom

                    onClicked: {
                        var allFilled = true;
                        if (textField.trim() === "")
                             allFilled = false;

                        if (allFilled) {
                            errorLabel.visible = false;
                            enterInf.get_inf_yanDisk(textField);
                        } else {
                            errorLabel2.visible = false;
                            errorLabel.visible = true;
                        }
                    }
                }
            }
        }
    }

    Connections
    {
        target: enterInf

        onInfYanDisk_Correct:
        {
            errorLabel2.visible = false;
            pageStack.replaceAbove(null, Qt.resolvedUrl("enter_about.qml"));
        }

        onInfYanDisk_InCorrect:
        {
            errorLabel2.visible = true;
        }
    }
}
