#-------------------------------------------------
#
# Project created by QtCreator 2026-05-12T00:14:01
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LP
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    wall.cpp \
    wallview.cpp \
    gamecontroller.cpp \
    healthview.cpp \
    item.cpp \
    itemview.cpp \
    player.cpp \
    playerview.cpp \
    storyfragment.cpp \
    gamewindow.cpp \
    storyfragmentview.cpp \
    finalstoryview.cpp \
    startwindow.cpp \
    rulewindow.cpp \
    failview.cpp \

HEADERS += \
    wall.h \
    wallview.h \
    gamecontroller.h \
    healthview.h \
    item.h \
    itemview.h \
    player.h \
    playerview.h \
    storyfragment.h \
    gamewindow.h \
    storyfragmentview.h \
    finalstoryview.h \
    startwindow.h \
    rulewindow.h \
    failview.h \

RESOURCES += \
    resources.qrc
