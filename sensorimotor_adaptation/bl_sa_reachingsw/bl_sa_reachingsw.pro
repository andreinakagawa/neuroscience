#-------------------------------------------------
#
# Project created by QtCreator 2016-07-18T19:16:06
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bl_sa_reachingsw
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    datafilecontroller.cpp \
    reachingwindow.cpp \
    cursorcontroller.cpp \
    protocolcontroller.cpp

HEADERS  += mainwindow.h \
    datafilecontroller.h \
    reachingwindow.h \
    cursorcontroller.h \
    protocolcontroller.h

FORMS    += mainwindow.ui \
    reachingwindow.ui
