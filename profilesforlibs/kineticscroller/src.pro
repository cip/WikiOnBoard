TEMPLATE = lib
CONFIG *= staticlib create_prl
DEPENDPATH += ../include
INCLUDEPATH += ../include
TARGET = QtScroller
DESTDIR = ../lib

#Wikionboard should not depend on webkit.
# (Don't forget to keep this when updating)
CONFIG+=no-webkit

!no-webkit:QT += webkit
no-webkit:DEFINES += QTSCROLLER_NO_WEBKIT

# Input
HEADERS += qtflickgesture_p.h \
           qtscroller.h \
           qtscroller_p.h \
           qtscrollerfilter_p.h \
           qtscrollerproperties.h \
           qtscrollerproperties_p.h \
           qtscrollevent.h \
           qtscrollevent_p.h
SOURCES += qtflickgesture.cpp \
           qtscroller.cpp \
           qtscrollerfilter.cpp \
           qtscrollerproperties.cpp \
           qtscrollevent.cpp
macx {
  OBJECTIVE_SOURCES += qtscroller_mac.mm
  LIBS += -framework Cocoa
}

OTHER_FILES += \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog

unix:!symbian:!maemo5 {
    target.path = /opt/src/lib
    INSTALLS += target
}
