
QT += core gui widgets multimedia

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
#    VER = 1.0
    VER = $$system(git describe --abbrev=8)
}

# Tip from: http://www.qtcentre.org/wiki/index.php?title=Version_numbering_using_QMake
VERSTR = '\\"$${VER}\\"'          # place quotes around the version string
DEFINES += VERSION=\"$${VERSTR}\" # create a VERSION macro containing the version string

SOURCES += \
    audiobuffer.cpp \
    main.cpp\
    mainwindow.cpp \
    multimon/cafsk12.cpp \
    multimon/costabf.c \
    ssi.cpp

HEADERS += \
    audiobuffer.h \
    mainwindow.h \
    multimon/cafsk12.h \
    multimon/filter.h \
    multimon/filter-i386.h \
    ssi.h

FORMS += mainwindow.ui

win32 {
    # application icon on Windows
    RC_FILE = qtmm.rc
} else:macx {
    # app icon on OSX
    ICON = icons/qtmm.icns
}

OTHER_FILES += \
    README.txt

RESOURCES += \
    qtmm.qrc
