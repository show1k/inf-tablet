/*******************************************************************************
**
** Copyright (C) 2022 ru.auroraos
**
** This file is part of the Информационный Планшет project.
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
import Nemo.KeepAlive 1.2

Page {
    id: page
    allowedOrientations: Orientation.Landscape
    property var adminCabinetList: ["46", "53а", "66", "71", "76"]
    // Отключение погашения экрана
    DisplayBlanking {
        id: displayBlanking
        preventBlanking: true
    }

    FontLoader
    {
        id: uncage
        source: "qrc:/fonts/ofont.ru_Uncage.ttf"
    }

    // Фон
    Rectangle {
        color: "#445e5f"
        anchors.fill: parent

        // Колонка с элементами
        Column {
            anchors.centerIn: parent
            spacing: 20

            // Поле для выбранного кабинета
            TextField {
                id: cabinetInput
                placeholderText: "Выберите номер кабинета"
                font.pixelSize: 40
                horizontalAlignment: TextInput.AlignHCenter
                readOnly: true
                width: 600
                anchors.horizontalCenter: parent.horizontalCenter
                color: "#f1eded" // Для видимости на тёмном фоне
            }

            // Кнопка открытия диалога
            Button {
                text: "Выбрать кабинет из списка"
                width: 600
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: dialog.open()
            }

            // Кнопка перехода к расписанию
            Button {
                text: "Перейти к расписанию"
                width: 600
                anchors.horizontalCenter: parent.horizontalCenter
                onClicked: {
                    if (cabinetInput.text.trim() !== "") {
                        CabinetSelector.setSelectedCabinet(cabinetInput.text)
                        if (adminCabinetList.indexOf(cabinetInput.text.trim()) !== -1)
                        {
                            pageStack.replaceAbove(null, Qt.resolvedUrl("AdminPage.qml"))
                        }
                        else
                        {
                            pageStack.replaceAbove(null, Qt.resolvedUrl("MainPage.qml"))
                        }
                    }
                }
            }
        }

        // Диалоговое окно выбора кабинета
        Dialog {
            id: dialog
            anchors.centerIn: parent
            width: parent.width * 0.8
            height: parent.height * 0.5
            backgroundColor: "#445e5f"
            allowedOrientations: Orientation.Landscape

            SilicaListView {
                anchors.fill: parent
                model: CabinetSelector.cabinetModel

                delegate: ListItem {
                    width: parent.width
                    contentHeight: Theme.itemSizeMedium

                    Label {
                        text: model.display
                        anchors.verticalCenter: parent.verticalCenter
                        font.pixelSize: 40
                        x: Theme.horizontalPageMargin
                        color: "#f1eded" // Для видимости
                    }

                    onClicked: {
                        cabinetInput.text = model.display
                        dialog.close()
                    }
                }

                VerticalScrollDecorator {
                    width: 15
                    radius: 30
                    color: "white"
                }
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                text: "Закрыть"
                onClicked: dialog.close()
            }
        }
    }
}
