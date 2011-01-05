VERSION = 0.0.19
TEMPLATE = app
TARGET = WikiOnBoard
QT += core \
    gui
HEADERS += wikionboard.h
SOURCES += main.cpp \
    wikionboard.cpp
FORMS += wikionboard.ui
RESOURCES += 
include(../kineticscroller/qtscroller.pri)

# TODO This is a workaround, final fix should rename zimlib to libzim for symbian as well.
# Note that unix includes symbian and therefore cannot be used.
linux-g++: {
    LIBS += -l:libzim.so
    LIBS += -l:liblzma.so
}
else { 
    # TODO find out why this is not working on my linux. (actually it should)
    LIBS += -lzimlib
    LIBS += -lliblzma
#LIBS += -L../zimlib

#    LIBS += -l:libzimlib.a
 #   LIBS += -L../xz
  #  LIBS += -l:libliblzma.lib.a
}
win32::INCLUDEPATH += e:\cygwin\usr\local\include
symbian: { 
    SOURCES += WikiOnBoard_reg.rss
    INCLUDEPATH += \epoc32\include
}
symbian: { 
    TARGET.EPCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x2000000 \
        // \
        Min \
        128kb, \
        Max \
        32Mb
}
symbian:LIBS += -lavkon \
    -leikcoctl \
    -lcone
symbian:TARGET.UID3 = 0xA89FA6F6

#Maemo
INCLUDEPATH += ../zimlib/include

OTHER_FILES += \
    debian/changelog \
    debian/compat \
    debian/control \
    debian/copyright \
    debian/README \
    debian/rules \
    WikiOnBoard.desktop

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/bin
    } else {
        target.path = /usr/local/bin
    }
    INSTALLS += target
}

unix:!symbian {
    desktopfile.files = $${TARGET}.desktop
    maemo5 {
        desktopfile.path = /usr/share/applications/hildon
    } else {
        desktopfile.path = /usr/share/applications
    }
    INSTALLS += desktopfile
}
