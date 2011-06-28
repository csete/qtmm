#-------------------------------------------------
#
# Project created by QtCreator 2011-06-27T23:14:27
#
#-------------------------------------------------

QT       += core gui

TARGET = qmm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    multimon/hdlc.c \
    multimon/demod_afsk12.c \
    multimon/costabf.c

HEADERS  += mainwindow.h \
    multimon/multimon.h \
    multimon/filter.h \
    multimon/filter-i386.h

FORMS    += mainwindow.ui
