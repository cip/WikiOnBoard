TEMPLATE = lib
TARGET = zimlib
CONFIG += staticlib
HEADERS +=  include/zim/article.h \
        include/zim/articlesearch.h \
        include/zim/blob.h \
        include/zim/cache.h \
        include/zim/cluster.h \
        include/zim/deflatestream.h \
        include/zim/dirent.h \
        include/zim/endian.h \
        include/zim/error.h \
        include/zim/file.h \
        include/zim/fileheader.h \
        include/zim/fstream.h \
        include/zim/fileimpl.h \
        include/zim/fileiterator.h \
        include/zim/indexarticle.h \
        include/zim/inflatestream.h \
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
        symbian/config.h
SOURCES += src/article.cpp src/articlesearch.cpp src/cluster.cpp src/deflatestream.cpp src/dirent.cpp src/envvalue.cpp src/file.cpp src/fileheader.cpp src/fstream.cpp src/fileimpl.cpp src/indexarticle.cpp src/inflatestream.cpp src/ptrstream.cpp src/search.cpp src/template.cpp src/unicode.cpp src/uuid.cpp src/zintstream.cpp src/lzmastream.cpp src/unlzmastream.cpp src/md5.c src/md5stream.cpp

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
    qtc_packaging/debian_harmattan/changelog

unix:!symbian:!maemo5 {
    target.path = /opt/zimlib/lib
    INSTALLS += target
}
