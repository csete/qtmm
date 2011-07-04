#-------------------------------------------------
#
# Project created by QtCreator 2011-06-27T23:14:27
#
#-------------------------------------------------

QT       += core gui multimedia


TEMPLATE = app

macx {
    TARGET = "AFSK1200 Decoder"
} else {
    TARGET = afsk1200dec
}


# disable debug messages in release
CONFIG(debug, debug|release) {
    # Define version string (see below for releases)
    VER = $$system(git describe --abbrev=8)
} else {
    DEFINES += QT_NO_DEBUG
    DEFINES += QT_NO_DEBUG_OUTPUT
    VER = 1.1
}

# Tip from: http://www.qtcentre.org/wiki/index.php?title=Version_numbering_using_QMake
VERSTR = '\\"$${VER}\\"'          # place quotes around the version string
DEFINES += VERSION=\"$${VERSTR}\" # create a VERSION macro containing the version string


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
