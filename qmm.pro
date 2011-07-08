#-------------------------------------------------
#
# Project created by QtCreator 2011-06-27T23:14:27
#
#-------------------------------------------------

QT       += core gui

#
# Ubuntu 10.10 and 11.04: changes due to some strangeness in Debian Qt packages
# Thanks to Andrea IW0HDV
#
linux-g++ {
    #
    # Try to catch distro informations
    #
    DISTRO=$$system(cat /etc/issue | head -n1 | cut -f1 -d\' \')
    DISTRO_MAJ=$$system(cat /etc/issue | head -n1 | cut -f2 -d\' \' | cut -f1 -d\\.)
    DISTRO_MIN=$$system(cat /etc/issue | head -n1 | cut -f2 -d\' \' | cut -f2 -d\\.)

    # specific to Ubuntu 10.10
    equals(DISTRO,Ubuntu):equals(DISTRO_MAJ,10):equals(DISTRO_MIN,10) {
       message(Linux $$DISTRO $$DISTRO_MAJ $$DISTRO_MIN)
       INCLUDEPATH += $$quote(/usr/include/QtMultimediaKit)
       LIBS += $$quote(-lQtMultimediaKit)
    } else {
       # specific to Ubuntu 11.04
       equals(DISTRO,Ubuntu):equals(DISTRO_MAJ,11):equals(DISTRO_MIN,04) {
          message(Linux $$DISTRO $$DISTRO_MAJ $$DISTRO_MIN)
          INCLUDEPATH += $$quote(/usr/include/QtMobility)
          INCLUDEPATH += $$quote(/usr/include/QtMultimediaKit)
          LIBS += $$quote(-lQtMultimediaKit)
       } else {
          QT += multimedia
       }
    }
} else {
    QT += multimedia
}

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
    VER = 1.0.36
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


win32 {
    # application icon on Windows
    RC_FILE = qmm.rc
} else:macx {
    # app icon on OSX
    ICON = icons/qmm.icns
}

OTHER_FILES += \
    README.txt

RESOURCES += \
    qmm.qrc
