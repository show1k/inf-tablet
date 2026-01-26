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

    // Хранилище для текстов полей
    property var fieldTexts: ["", "", ""] // 5 пустых строк
    property var placeholder: ["Номера админ кабиетов, через пробел", "Номер среднего урока в расписании", "Время начала среднего урока(12:25)"]

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
            text: "Дополнительные данные"
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: parent.height / 40
            color: "black"
            font.pixelSize: 40
        }

        Column {
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: parent.height / 10
            spacing: 40

            Repeater {
                model: 3
                Rectangle {
                    width: enter.width / 3 * 2.5
                    height: enter.height / 10
                    border.color: "black"
                    radius: 10
                    border.width: 1
                    color: "#5a5e5d"
                    opacity: 0.5

                    TextField {
                        // Привязываем к элементу массива
                        text: fieldTexts[index]
                        placeholderText: placeholder[index]
                        onTextChanged: fieldTexts[index] = text

                        width: parent.width
                        anchors.top: parent.top
                        font.pixelSize: 40
                        color: "#5a5e5d"
                        opacity: 1
                    }
                }
            }

            Item {
                width: parent.width
                height: parent.height / 10 + (errorLabel.visible ? errorLabel.height : 0) // если ошибка видна — добавляем высоту

                // Текст ошибки — позиционируем над кнопкой
                Text {
                    id: errorLabel
                    text: "Заполните все поля ввода"
                    color: "red"
                    font.pixelSize: 30
                    anchors.horizontalCenter: parent.horizontalCenter
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
                        for (var i = 0; i < fieldTexts.length; i++) {
                            if (fieldTexts[i].trim() === "") {
                                allFilled = false;
                                break;
                            }
                        }

                        if (allFilled) {
                            errorLabel.visible = false;
                            enterInf.get_inf_aboutSch(fieldTexts);
                        } else {
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

        onInfAbout_ready:
        {
            errorLabel.visible = false;
            pageStack.replaceAbove(null, Qt.resolvedUrl("Loading.qml"));
        }
    }
}
