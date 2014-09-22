#-------------------------------------------------
#
# Project created by QtCreator 2014-06-19T14:39:26
#
#-------------------------------------------------

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MCServerAdmin
TEMPLATE = app

SOURCES += \
    src/main.cpp \
    src/mainwindow.cpp \
    src/serverconfig.cpp \
    src/mapconfig.cpp \
    src/server.cpp \
    src/map.cpp \
    src/servertree.cpp \
    src/servertreedelegate.cpp \
    src/configwidget.cpp \
    src/superflatsettings.cpp

INCLUDEPATH += include

HEADERS += \
    include/mainwindow.h \
    include/serverconfig.h \
    include/mapconfig.h \
    include/server.h \
    include/map.h \
    include/servertree.h \
    include/servertreedelegate.h \
    include/configwidget.h \
    include/superflatsettings.h

FORMS += \
    ui/mainwindow.ui \
    ui/serverconfig.ui \
    ui/mapconfig.ui

TRANSLATIONS += \
    res/translations/mcserveradmin.es.ts \
    res/translations/mcserveradmin.fr.ts \
    res/translations/mcserveradmin.it.ts \
    res/translations/mcserveradmin.de.ts \
    res/translations/mcserveradmin.pt.ts

RESOURCES += \
    res/translations.qrc
