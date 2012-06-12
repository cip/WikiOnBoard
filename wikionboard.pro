#Set to 1 or 0. Changes UID used, and SwEvent Capability requested only if not selfsigned.
IS_SELFSIGNED = 1
#IS_SELFSIGNED = 0
DEFINES += "__IS_SELFSIGNED__=$$IS_SELFSIGNED"
#Set to 1 or 0. Set to 1 to enable split screen keyboard support (Symbian^3)
# Note that feature should not be used for released version (see https://github.com/cip/WikiOnBoard/issues/51 )
ENABLE_SPLITSCREENKEYBOARD = 0
DEFINES += "__ENABLE_SPLITSCREENKEYBOARD__=$$ENABLE_SPLITSCREENKEYBOARD"

VERSION = 2.1.60
DEFINES += "__APPVERSION__=$$VERSION" 
TEMPLATE = app

QT += core \
    gui \
    network \
    webkit

# Note that kinetic scroller should still been built with the CONFIG +=no-webkit
# as it is not used for webkit kinetic scrolling.

HEADERS += \
    zimfilewrapper.h \
    indexlist.h \
    indexlistqml.h \
    QsKineticScroller.h \
    mediakeycaptureitem.h \
    zimreply.h \
    asynchronouszimreader.h
SOURCES += main.cpp \
    zimfilewrapper.cpp \
    indexlist.cpp \
    QsKineticScroller.cpp \
    mediakeycaptureitem.cpp \
    zimreply.cpp \
    asynchronouszimreader.cpp

FORMS +=

simulator {
    INCLUDEPATH *= ../kineticscroller/include
    LIBS *= -L../simulatorlibs -lQtScroller
} else:!android {
    RESOURCES +=
    include(../kineticscroller/qtscroller.pri)
}
TRANSLATIONS = wikionboard_en.ts \
               wikionboard_de.ts \
               wikionboard_es.ts \
               wikionboard_it.ts

CONFIG *= link_prl


# TODO This is a workaround, final fix should rename zimlib to libzim for symbian as well.
# Note that unix includes symbian and therefore cannot be used.
linux-g++: {
    LIBS += -l:libzim.so
    LIBS += -l:liblzma.so
}    

symbian: {
    # TODO find out why this is not working on my linux. (actually it should)
    LIBS += -lzimlib
    LIBS += -lliblzma
#LIBS += -L../zimlib

#    LIBS += -l:libzimlib.a
 #   LIBS += -L../xz
  #  LIBS += -l:libliblzma.lib.a
} else:simulator {
   #Libs need to be copied to ../simulatorlibs manually
    LIBS *= -L../simulatorlibs -lzimlib
    LIBS *= -L../simulatorlibs -lliblzma
}  else:android {
    message("android")
    LIBS += -L$$_PRO_FILE_PWD_/libs/android -lzimlib
    LIBS += -L$$_PRO_FILE_PWD_/libs/android -lQtScroller
    LIBS += -L$$_PRO_FILE_PWD_/libs/android -lliblzma

}
else {    
    LIBS *= -L../zimlib -lzimlib
    LIBS *= -L../xz -lliblzma    
}


symbian: { 
    SOURCES += WikiOnBoard_reg.rss
    INCLUDEPATH += \epoc32\include
}

DEFINES += QT_NO_CAST_FROM_ASCII \
           QT_NO_CAST_TO_ASCII
#message(QMAKESPEC: $$QMAKESPEC)


win32: {
#Update and generate translations here (during simulator builds)
message("Generate translations (Symbian only. (Meego is omitted))")
system($$quote(lupdate  -no-recursive  .  -recursive qml/common -recursive qml/symbian  -ts $${TRANSLATIONS}))
system($$quote(lrelease $${TRANSLATIONS}))
}

message(QT_VERSION: $$QT_VERSION)
symbian: { 
    message(S60_VERSION: $$S60_VERSION)
    message(SYMBIAN_VERSION: $$SYMBIAN_VERSION)
    TARGET.EPCSTACKSIZE = 0x14000
    TARGET.EPOCHEAPSIZE = 0x20000 \
        0x2000000 \
        // \
        Min \
        128kb, \
        Max \
        32Mb
    LIBS += -lavkon \
    	-leikcoctl \
    	-lcone

    # FOR ENABLING SPLIT SCREEN VIRTUAL KEYBOARD
    LIBS += -lfepbase

    ICON = wikionboard.svg
    #Instead of using default sis header definition define our own.
    # Difference: Default package name= $$TARGET, and so includes the UID on symbian.
    #   Package name is displayed in installer, and in program manager.
    # => Define package name to be WikiOnBoard (without UID)
    version_string_pkg = $$replace(VERSION,"\.",",")

    contains(IS_SELFSIGNED,0): {	
 		#Nokia assigned UID 
	 	TARGET.UID3 = 0x20045592
 		#Required to let browser load page and switch to foreground if browser already open
   		# on QDesktopServices::openUrl call
    	#Note that capability not available for self-signed apps. 
                TARGET.CAPABILITY+= SwEvent
                #For QML Webview based implementation
                TARGET.CAPABILITY+= NetworkServices
                # WikionBoard instead of WikiOnBoard_<UID> sis package name. (See above)
                pkgname.pkg_prerules = "$${LITERAL_HASH}{\"WikiOnBoard\"},($$TARGET.UID3),$$version_string_pkg"
                # Use correct UID for wrapper (wikionboard_installer.sis) package.
                DEPLOYMENT.installer_header += "$${LITERAL_HASH}{\"WikiOnBoard Installer\"},(0x2002CCCF),$$version_string_pkg"
                message(Configuring for nokia signed app. UID3 = $$TARGET.UID3 $$CAPABILITY)
 	} else {
 		#UID for self-signing
                symbian:TARGET.UID3 = 0xA89FA6F6
                # WikionBoard Self-Signed instead of WikiOnBoard_<UID> sis package name. (See above)
                pkgname.pkg_prerules = "$${LITERAL_HASH}{\"WikiOnBoard Self-Signed\"},($$TARGET.UID3),$$version_string_pkg"
                # Use correct UID for wrapper (wikionboard_installer.sis) package, and use package name WikiOnboard Installer Self-Signed.
                DEPLOYMENT.installer_header += "$${LITERAL_HASH}{\"WikiOnBoard Installer Self-Signed\"},(0xA000D7CE),$$version_string_pkg"

		message(Configuring for self-signed app. UID3 = $$TARGET.UID3 $$CAPABILITY)
	}	
        DEPLOYMENT += pkgname
 	#Include Vendorinfo in sis. (Note umlaut does not work in makesis)
 	vendorinfo = "%{\"Christian Puehringer\"}" ":\"Christian Puehringer\""
        my_deployment.pkg_prerules = vendorinfo

        DEPLOYMENT += my_deployment
        message($$DEPLOYMENT) 	
}

symbian: {
    # Fix for Issue 47
    TARGET = WikiOnBoard_$${TARGET.UID3}
    TARGET.NAME = WikiOnBoard
    message("Modify loc file")
    modify_caption.target = $${TARGET}.loc.~
    modify_caption.depends = $${TARGET}.loc
    modify_caption.commands = perl -i.~ -p -e \"s/$${TARGET}/$${TARGET.NAME}/g;\" $${TARGET}.loc

    QMAKE_EXTRA_TARGETS += modify_caption
    PRE_TARGETDEPS += $${modify_caption.target}
    QMAKE_DISTCLEAN += $${modify_caption.target}
}
!symbian: {
    TARGET = WikiOnBoard
}


#Maemo
#INCLUDEPATH += ../zimlib/include
#TODO trial for mingw build

android {
     INCLUDEPATH += ../kineticscroller/include
}
INCLUDEPATH += ../zimlib/include ../xz

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/src/eu/licentia/necessitas/industrius/QtActivity.java \
    android/src/eu/licentia/necessitas/industrius/QtApplication.java \
    android/src/eu/licentia/necessitas/industrius/QtLayout.java \
    android/src/eu/licentia/necessitas/industrius/QtSurface.java \
    android/src/eu/licentia/necessitas/ministro/IMinistro.aidl \
    android/src/eu/licentia/necessitas/ministro/IMinistroCallback.aidl \
    android/src/eu/licentia/necessitas/mobile/QtAndroidContacts.java \
    android/src/eu/licentia/necessitas/mobile/QtCamera.java \
    android/src/eu/licentia/necessitas/mobile/QtFeedback.java \
    android/src/eu/licentia/necessitas/mobile/QtLocation.java \
    android/src/eu/licentia/necessitas/mobile/QtMediaPlayer.java \
    android/src/eu/licentia/necessitas/mobile/QtSensors.java \
    android/src/eu/licentia/necessitas/mobile/QtSystemInfo.java \
    android/AndroidManifest.xml \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/src/eu/licentia/necessitas/industrius/QtActivity.java \
    android/src/eu/licentia/necessitas/industrius/QtApplication.java \
    android/src/eu/licentia/necessitas/industrius/QtLayout.java \
    android/src/eu/licentia/necessitas/industrius/QtSurface.java \
    android/src/eu/licentia/necessitas/ministro/IMinistro.aidl \
    android/src/eu/licentia/necessitas/ministro/IMinistroCallback.aidl \
    android/src/eu/licentia/necessitas/mobile/QtAndroidContacts.java \
    android/src/eu/licentia/necessitas/mobile/QtCamera.java \
    android/src/eu/licentia/necessitas/mobile/QtFeedback.java \
    android/src/eu/licentia/necessitas/mobile/QtLocation.java \
    android/src/eu/licentia/necessitas/mobile/QtMediaPlayer.java \
    android/src/eu/licentia/necessitas/mobile/QtSensors.java \
    android/src/eu/licentia/necessitas/mobile/QtSystemInfo.java \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/manifest.aegis \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
    WikiOnBoard_harmattan.desktop \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml


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
message(QT Variable: $$QT)
message(Libs Variable: $$LIBS)
#QML Qt Components for Android
    android {
        qmlcomponents.source = qml/imports
        qmlcomponents.target = /imports
        DEPLOYMENTFOLDERS += qmlcomponents
        qmlplugins.files = \
            qml/plugins/libandroidplugin_1_1.so \
            qml/plugins/libqtcomponentsplugin_1_1.so
            x86 {
                qmlplugins.path = /libs/x86
            } else: armeabi-v7a {
                qmlplugins.path = /libs/armeabi-v7a
            } else {
                qmlplugins.path = /libs/armeabi
            }
        INSTALLS += qmlplugins
    }
#QML (Common)
common_qml.source = qml/common/WikiOnBoardComponents
common_qml.target = qml

symbian {
# QML (Symbian components) related
platform_qml.source = qml/symbian/WikiOnBoardComponents
platform_qml.target = qml
}  else:android {
    # QML (Android components) related
    platform_qml.source = qml/symbian/WikiOnBoardComponents
    platform_qml.target = qml/WikiOnBoardComponents
    #Override target here. (else qml files deployed into qml dir directly)
    common_qml.target = qml/WikiOnBoardComponents

} else:simulator {
# Edit here to switch between simulator for meego or symbian
  #platform_qml.source = qml/meego/WikiOnBoardComponents
  platform_qml.source = qml/symbian/WikiOnBoardComponents
platform_qml.target = qml
} else {
message("neither symbian nor simulator, assume symbian")
platform_qml.source = qml/symbian/WikiOnBoardComponents
platform_qml.target = qml
# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
CONFIG += qdeclarative-boostable
}
DEPLOYMENTFOLDERS += common_qml platform_qml
message(DEPLOYMENTFOLDERS: $$DEPLOYMENTFOLDERS)
message(platform_qml.source: $$platform_qml.source)

# Additional import path ussed to resolve QML modules in Creator's code model
QML_IMPORT_PATH =
CONFIG += qt-components
# Please do not modify the following two lines. Required for deployment.
include(qmlapplicationviewer/qmlapplicationviewer.pri)
qtcAddDeployment()

symbian {
    # Copied from QTCREATORBUG-5589
    # Required for S^3 SDK, else linking fails
    LIBS += -lusrt2_2.lib

    # Required for using volume key for article scrolling
    # Note that for symbian 5th compile it may be required
    # to copy libs manully to qt sdk.
    LIBS += -L\epoc32\release\armv5\lib -lremconcoreapi
    LIBS += -L\epoc32\release\armv5\lib -lremconinterfacebase
}

RESOURCES += \
    translations.qrc



