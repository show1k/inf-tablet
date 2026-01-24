/*******************************************************************************
**
** Copyright (C) 2022 ru.auroraos
**
** This file is part of the Моё приложение для ОС Аврора project.
**
** Redistribution and use in source and binary forms,
** with or without modification, are permitted provided
** that the following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice,
**   this list of conditions and the following disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice,
**   this list of conditions and the following disclaimer
**   in the documentation and/or other materials provided with the distribution.
** * Neither the name of the copyright holder nor the names of its contributors
**   may be used to endorse or promote products derived from this software
**   without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
** FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
** IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
** OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
** LOSS OF USE, DATA, OR PROFITS;
** OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
** EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
*******************************************************************************/

import QtQuick 2.0
import Sailfish.Silica 1.0
import QtQuick.Layouts 1.0
import Nemo.KeepAlive 1.2
import QtGraphicalEffects 1.0

// реализация окна с расписанием кабинетов
Page {
    objectName: "mainPage"

    // доступные положения интерфейса
    allowedOrientations: Orientation.LandscapeMask;

    // отключение погашения экрана при открытом приложении
    DisplayBlanking {
            id: displayBlanking
            preventBlanking: true
        }
    // создание нового шрифта в окне
    FontLoader {
        id: uncage
        source: "qrc:/fonts/ofont.ru_Uncage.ttf"
    }

    // объявление шрифта для дальнейшего испозования
    Binding {
            target: Theme
            property: "fontFamily"
            value: uncage.name
        }
    // фон окна
    Rectangle {
        anchors.fill: parent
        color: "#63666A"

        // положение элементов в строку
        RowLayout {
            anchors.fill: parent

            // отступы от краёв экрана
            anchors.bottomMargin: 20
            anchors.topMargin: 20
            anchors.leftMargin: 20
            anchors.rightMargin: 20
            spacing: 20

            // создание бокового меню
            Rectangle {
                id: cont
                Layout.fillHeight: true
                Layout.fillWidth: true
                radius:60
                Layout.preferredWidth: parent.width * 0.3 // занимает 0.3 ширины от всего экрана
                color: "#445e5f"

                // размещение элементов в колонну
                ColumnLayout {
                    id: column0
                    anchors.fill: parent
                    spacing: 10

                    // Логотип Лицея
                    Rectangle {
                        id: imageContainer
                        Layout.fillWidth: true
                        Layout.preferredHeight: parent.height * 0.25
                        radius: 60
                        color: "transparent"
                        clip: false // отключение обрезания контейнера по умолчанию

                        // контейнер для изображения
                        Image {
                             id: image
                             objectName: "image"
                             source: "qrc:/image/logo.jpeg"
                             anchors.fill: parent
                             fillMode: Image.PreserveAspectCrop //  метод для масштабирования и обрезания изображения
                             visible: false
                           }
                        // создаем маску для обрезания  изображения
                        Rectangle {
                             id: mask
                             anchors.fill: parent
                             radius: 60
                             visible: false
                           }
                        // подключаем и настраиваем маску
                        OpacityMask {
                             anchors.fill: image
                             source: image
                             maskSource: mask
                          }
                    }

                    // контейрнер с изображением учителя
                    Image {
                        id: image2
                        objectName: "image_teacher"
                        source: ""
                        Layout.preferredHeight: parent.height * 0.5
                        Layout.preferredWidth: parent.width * 0.7
                        Layout.alignment: Qt.AlignHCenter
                        fillMode: Image.PreserveAspectCrop
                        asynchronous: true // Загружает изображение в фоновом потоке
                        cache: false

                        NumberAnimation {
                                id: fadeAnim
                                target: image2
                                property: "opacity"
                                from: 0
                                to: 1
                                duration: 1000
                                running: false
                            }

                            Connections {
                                target: AppController
                                onTriggerFadeAnimation: {
                                    fadeAnim.start()
                                }
                                onStopFadeAnimation: {
                                    fadeAnim.stop() // Останавливаем анимацию
                                    image2.opacity = 1 // Устанавливаем полную непрозрачность
                                }
                            }
                    }
                    // копмновка элементов
                    Item {
                        id: name
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Layout.preferredHeight: parent.height * 0.25
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
                }
            }
            // уроки в кабинете
            Rectangle {
                id: cont2
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.preferredWidth: parent.width * 0.75
                Layout.preferredHeight: parent.height
                radius: 60
                color: "#445e5f"
                ColumnLayout {
                    id: column
                    anchors.fill: parent

                    // отступы между элементами
                    anchors.topMargin:20
                    anchors.bottomMargin:20
                    anchors.leftMargin:20
                    anchors.rightMargin:20
                    spacing: 10

                    // 2 контейнера с номером и информацией о уроке
                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        // контейнер с номером урока
                        Rectangle {
                            objectName: "les0"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                               id: _text9
                               text: qsTr("0")
                               objectName: "num0"
                               font.pixelSize: 42
                               anchors.fill:parent
                               verticalAlignment: Label.AlignVCenter
                               horizontalAlignment: Label.AlignHCenter
                               font.family: uncage.name
                            }
                        }

                        // контейнер с информацией о уроке
                        Rectangle{
                            id: root0
                            objectName: "root0"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60

                            // переменная с значением на сколько строк растенуть элемент
                            property int klassRowSpan: 1

                            // расположение элементов в сетке
                            GridLayout{
                                anchors.fill:parent
                                rows: 2 // сколько строк
                                columns: 3 // сколько колоннок
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                // контейнер с именем учителя
                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher0"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok0"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time0"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root0.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass0"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group0"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les1"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                                id: _text12
                                text: qsTr("1")
                                objectName: "num1"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root1
                            objectName: "root1"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60
                            property int klassRowSpan: 1
                            GridLayout{
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher1"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok1"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time1"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root1.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass1"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group1"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les2"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            border.width: 0
                            radius: 180
                            Text {
                                text: qsTr("2")
                                objectName: "num2"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root2
                            objectName: "root2"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60
                            property int klassRowSpan: 1
                            GridLayout{
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher2"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok2"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time2"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root2.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass2"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group2"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les3"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                                text: qsTr("3")
                                objectName: "num3"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root3
                            objectName: "root3"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60
                            property int klassRowSpan: 1
                            GridLayout{
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher3"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok3"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time3"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root3.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass3"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group3"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les4"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                                text: qsTr("4")
                                objectName: "num4"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root4
                            objectName: "root4"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            property int klassRowSpan: 1
                            radius: 60
                            GridLayout{
                                id: grid
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher4"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok4"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time4"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root4.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass4"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group4"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les5"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                                text: qsTr("5")
                                objectName: "num5"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root5
                            objectName: "root5"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60
                            property int klassRowSpan: 1
                            GridLayout{
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher5"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok5"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time5"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root5.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass5"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group5"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les6"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                                text: qsTr("6")
                                objectName: "num6"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root6
                            objectName: "root6"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60
                            property int klassRowSpan: 1
                            GridLayout{
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher6"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok6"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time6"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root6.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass6"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group6"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les7"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                                text: qsTr("7")
                                objectName: "num7"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root7
                            objectName: "root7"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60
                            property int klassRowSpan: 1
                            GridLayout{
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher7"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok7"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time7"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root7.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass7"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group7"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les8"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                                text: qsTr("8")
                                objectName: "num8"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root8
                            objectName: "root8"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60
                            property int klassRowSpan: 1
                            GridLayout{
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher8"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok8"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time8"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root8.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass8"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group8"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }


                            }

                        }

                    }

                    RowLayout {
                        spacing: 10
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        Rectangle {
                            objectName: "les9"
                            Layout.fillHeight: true
                            Layout.preferredWidth: 104
                            radius: 180
                            Text {
                                text: qsTr("9")
                                objectName: "num9"
                                font.pixelSize: 42
                                anchors.fill:parent
                                verticalAlignment: Label.AlignVCenter
                                horizontalAlignment: Label.AlignHCenter
                                font.family: uncage.name
                            }
                        }
                        Rectangle{
                            id: root9
                            objectName: "root9"
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            radius: 60
                            property int klassRowSpan: 1
                            GridLayout{
                                anchors.fill:parent
                                rows: 2
                                columns: 3
                                anchors.leftMargin: 43
                                anchors.rightMargin: 20

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 1
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "teacher9"
                                        text: qsTr("")
                                        fontSize: 30
                                        fontFamily: "uncage"
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 0
                                    Layout.row: 0
                                    Layout.preferredWidth: 250
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "yrok9"
                                        text: qsTr("")
                                        fontSize: 33
                                        fontFamily: "uncage"
                                    }
                                }

                                Text {
                                    text: qsTr("")
                                    font.pixelSize: 30
                                    objectName: "time9"
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    Layout.preferredWidth: 50
                                    Layout.column: 1
                                    Layout.rowSpan: 2
                                    verticalAlignment: Label.AlignVCenter
                                    font.family: uncage.name

                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 0
                                    Layout.rowSpan: root9.klassRowSpan
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "klass9"
                                        text: ""
                                        fontSize: 42
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }

                                Rectangle {
                                    Layout.fillHeight: true
                                    Layout.fillWidth: true
                                    Layout.column: 2
                                    Layout.row: 1
                                    Layout.preferredWidth: 104
                                    Layout.preferredHeight: 30
                                    clip: true
                                    color: "transparent"

                                    ScrollingText {
                                        objectName: "group9"
                                        text: ""
                                        fontSize: 35
                                        fontFamily: "uncage"
                                        leftAlign: false
                                    }
                                }
                            }

                        }

                    }
                }



            }
        }
    }

}
