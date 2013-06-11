TEMPLATE = lib
TARGET = zimlib
#To support shadowbuilds. (TODO store libs dependend on target)
DESTDIR = ../zimlib
CONFIG += staticlib create_prl

# Static link LZMA Lib. (Define appearantly only affects mingw32 builds)
DEFINES += LZMA_API_STATIC

#CIP trial. (not working)
#DEFINES +=SYMBIAN_OE_LARGE_FILE_SUPPORT
#DEFINES +=HAVE_LSEEK64
#DEFINES +=HAVE_OPEN64


#2011/08/15 Removed inflatestream/deflatestream files. Appearantly not required, and introduce
#  dependency on zlib. (Problem with qt simulator builds)
HEADERS +=  include/zim/article.h \
        include/zim/articlesearch.h \
        include/zim/blob.h \
        include/zim/cache.h \
        include/zim/cluster.h \
        #include/zim/deflatestream.h \
        include/zim/dirent.h \
        include/zim/error.h \
        include/zim/file.h \
        include/zim/fileheader.h \
        include/zim/fstream.h \
        include/zim/fileimpl.h \
        include/zim/fileiterator.h \
        include/zim/indexarticle.h \
        #include/zim/inflatestream.h \
        include/zim/lzmastream.h \
        include/zim/noncopyable.h \
        include/zim/refcounted.h \
        include/zim/search.h \
        include/zim/smartptr.h \
        include/zim/template.h \
        include/zim/unicode.h \
        include/zim/unlzmastream.h \
        include/zim/uuid.h \
        include/zim/zim.h \
        include/zim/zintstream.h \
        symbian/config.h \
    include/zim/endianzim.h
#TODO: For meego harmattan renamed endian.h to endianzim.h (and changed
 #  includes accordingly. (else does not compile). Try to find other solution
#SOURCES += src/article.cpp src/articlesearch.cpp src/cluster.cpp src/deflatestream.cpp src/dirent.cpp src/envvalue.cpp src/file.cpp src/fileheader.cpp src/fstream.cpp src/fileimpl.cpp src/indexarticle.cpp src/inflatestream.cpp src/ptrstream.cpp src/search.cpp src/template.cpp src/unicode.cpp src/uuid.cpp src/zintstream.cpp src/lzmastream.cpp src/unlzmastream.cpp src/md5.c src/md5stream.cpp
SOURCES += src/article.cpp src/articlesearch.cpp src/cluster.cpp src/dirent.cpp src/envvalue.cpp src/file.cpp src/fileheader.cpp src/fstream.cpp src/fileimpl.cpp src/indexarticle.cpp src/ptrstream.cpp src/search.cpp src/template.cpp src/unicode.cpp src/uuid.cpp src/zintstream.cpp src/lzmastream.cpp src/unlzmastream.cpp src/md5.c src/md5stream.cpp

INCLUDEPATH += include include/zim symbian
#Maemo (instead of installing liblzma include files, reference them directly)
INCLUDEPATH += ../xz/src/common ../xz/src/liblzma/api

OTHER_FILES += \
    qtc_packaging/debian_fremantle/rules \
    qtc_packaging/debian_fremantle/README \
    qtc_packaging/debian_fremantle/copyright \
    qtc_packaging/debian_fremantle/control \
    qtc_packaging/debian_fremantle/compat \
    qtc_packaging/debian_fremantle/changelog \
    qtc_packaging/debian_harmattan/rules \
    qtc_packaging/debian_harmattan/README \
    qtc_packaging/debian_harmattan/copyright \
    qtc_packaging/debian_harmattan/control \
    qtc_packaging/debian_harmattan/compat \
    qtc_packaging/debian_harmattan/changelog \
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
    android/src/eu/licentia/necessitas/mobile/QtSystemInfo.java

unix:!symbian:!maemo5 {
    target.path = /opt/zimlib/lib
    INSTALLS += target
}
