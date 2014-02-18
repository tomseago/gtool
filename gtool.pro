#-------------------------------------------------
#
# Project created by QtCreator 2014-02-15T12:36:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = gtool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    job.cpp \
    straightmove.cpp \
    position.cpp \
    move.cpp \
    gcodeparser.cpp \
    codeblock.cpp \
    cgcode.cpp \
    word.cpp \
    ui_errorslistmodel.cpp \
    parseerror.cpp \
    parametermapper.cpp

HEADERS  += mainwindow.h \
    job.h \
    straightmove.h \
    position.h \
    move.h \
    gcodeparser.h \
    codeblock.h \
    cgcode.h \
    word.h \
    ui_errorslistmodel.h \
    parseerror.h \
    parametermapper.h

FORMS    += mainwindow.ui

# Mac stuff
ICON = app_icon.icns
