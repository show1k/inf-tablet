################################################################################
##
## Copyright (C) 2022 ru.auroraos
## 
## This file is part of the Информационный Планшет project.
##
## Redistribution and use in source and binary forms,
## with or without modification, are permitted provided
## that the following conditions are met:
##
## * Redistributions of source code must retain the above copyright notice,
##   this list of conditions and the following disclaimer.
## * Redistributions in binary form must reproduce the above copyright notice,
##   this list of conditions and the following disclaimer
##   in the documentation and/or other materials provided with the distribution.
## * Neither the name of the copyright holder nor the names of its contributors
##   may be used to endorse or promote products derived from this software
##   without specific prior written permission.
##
## THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
## AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
## THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
## FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
## IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
## FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
## OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
## PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
## LOSS OF USE, DATA, OR PROFITS;
## OR BUSINESS INTERRUPTION)
## HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
## WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
## (INCLUDING NEGLIGENCE OR OTHERWISE)
## ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
## EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
################################################################################

QT += \
        quick \
        network \
        gui

TARGET = ru.auroraos.InfTablet

CONFIG += \
    auroraapp \
    с++17

PKGCONFIG += \

SOURCES += \
    app/appcontroller.cpp \
    app/cabinetselector.cpp \
    internet_connection/internetconnection.cpp \
    lesson/controllerlessons.cpp \
    lesson/lesson.cpp \
    download_pictures/yandexdirectorychecker.cpp \
    download_pictures/yandexdownloadcontroller.cpp \
    download_pictures/yandeximagebreakdownloader.cpp \
    download_pictures/yandeximageteachersdownloader.cpp \
    schedule/classroomschedulereader.cpp \
    schedule/collectinformationschool.cpp \
    schedule/controlleruploadschedule.cpp \
    app/main.cpp \
    schedule/school.cpp \
    set_pictures/controllersetpictures.cpp \
    time/timeofapp.cpp \
    schedule/uploadlastschedule.cpp \
    schedule/uploadweekschedule.cpp

HEADERS += \
    app/appcontroller.h \
    app/cabinetselector.h \
    app/globalVar.h \
    internet_connection/internetconnection.h \
    lesson/controllerlessons.h \
    lesson/lesson.h \
    download_pictures/yandexdirectorychecker.h \
    download_pictures/yandexdownloadcontroller.h \
    download_pictures/yandeximagebreakdownloader.h \
    download_pictures/yandeximageteachersdownloader.h \
    schedule/classroomschedulereader.h \
    schedule/collectinformationschool.h \
    schedule/controlleruploadschedule.h \
    schedule/school.h \
    set_pictures/controllersetpictures.h \
    time/timeofapp.h \
    schedule/uploadlastschedule.h \
    schedule/uploadweekschedule.h

DISTFILES += \
    qml/InfTablet.qml \
    qml/pages/AdminPage.qml \
    qml/pages/InputCab.qml \
    qml/pages/ScrollingText.qml \
    rpm/ru.auroraos.InfTablet.spec \
    AUTHORS.md \
    CODE_OF_CONDUCT.md \
    CONTRIBUTING.md \
    LICENSE.BSD-3-CLAUSE.md \
    README.md \
    qml/cover/DefaultCoverPage.qml \
    qml/pages/MainPage.qml \

AURORAAPP_ICONS = 86x86 108x108 128x128 172x172

CONFIG += auroraapp_i18n

TRANSLATIONS += \
    translations/ru.auroraos.InfTablet.ts \
    translations/ru.auroraos.InfTablet-ru.ts \

RESOURCES += \
    qml.qrc
