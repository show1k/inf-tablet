
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
    enter_inf/enter_inf.cpp \
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
    enter_inf/enter_inf.h \
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
    qml/pages/enter_about.qml \
    qml/pages/enter_eljur.qml \
    qml/pages/enter_yandexDisk.qml \
    rpm/ru.auroraos.InfTablet.spec \
    qml/cover/DefaultCoverPage.qml \
    qml/pages/MainPage.qml \

AURORAAPP_ICONS = 86x86 108x108 128x128 172x172

CONFIG += auroraapp_i18n

TRANSLATIONS += \
    translations/ru.auroraos.InfTablet.ts \
    translations/ru.auroraos.InfTablet-ru.ts \

RESOURCES += \
    qml.qrc
