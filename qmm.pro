#-------------------------------------------------
#
# Project created by QtCreator 2011-06-27T23:14:27
#
#-------------------------------------------------

QT       += core gui multimedia

TARGET = qmm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    multimon/costabf.c \
    audiobuffer.cpp \
    ssi.cpp \
    multimon/cafsk12.cpp

HEADERS  += mainwindow.h \
    multimon/filter.h \
    multimon/filter-i386.h \
    audiobuffer.h \
    ssi.h \
    multimon/cafsk12.h

FORMS    += mainwindow.ui
