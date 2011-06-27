
#Set to 1 or 0. SwEvent Capability requested only if not selfsigned.
IS_SELFSIGNED = 1
DEFINES += "__IS_SELFSIGNED__=$$IS_SELFSIGNED"
VERSION = 0.0.38
DEFINES += "__APPVERSION__=$$VERSION" 
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

TRANSLATIONS = wikionboard_en.ts \
			   wikionboard_de.ts \
               wikionboard_es.ts 
               
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

DEFINES += QT_NO_CAST_FROM_ASCII \
           QT_NO_CAST_TO_ASCII

symbian: { 
    TARGET.EPCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x2000000 \
        // \
        Min \
        128kb, \
        Max \
        32Mb
    #Required to let browser load page and switch to foreground if browser already open
    # on QDesktopServices::openUrl call
    #Note that capability not available for self-signed apps. 
    contains(IS_SELFSIGNED,0): {	
 	 TARGET.CAPABILITY+= SwEvent 	 
 	}
 		
 #Deploy files for translation (qm extension) to application's private directory    
    translationfiles.sources = *.qm    
    DEPLOYMENT +=translationfiles 
}
symbian:LIBS += -lavkon \
    -leikcoctl \
    -lcone
symbian:TARGET.UID3 = 0xA89FA6F6
symbian:
{
    ICON = wikionboard.svg
}
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
