#-------------------------------------------------
#
# Project created by QtCreator 2010-08-01T22:04:38
#
#-------------------------------------------------

QT       += core gui opengl

TARGET = serial-osc
#CONFIG += console
TEMPLATE = app

PRECOMPILED_HEADER = pch.h

SOURCES += main.cpp\
        mainwindow.cpp \
    dataprocessor.cpp \
    realtimereader.cpp \
    glwidget.cpp \
    digitalfilter.cpp

HEADERS  += mainwindow.h \
    pch.h \
    exception.h \
    dataprocessor.h \
    realtimereader.h \
    glwidget.h \
    digitalfilter.h

FORMS    += mainwindow.ui
