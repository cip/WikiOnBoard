#ifndef MediakeyCaptureItem_H
#define MediakeyCaptureItem_H

#include <QDeclarativeItem>

#ifdef Q_OS_SYMBIAN
#include <remconcoreapitargetobserver.h>    // link against RemConCoreApi.lib
#include <remconcoreapitarget.h>            // and
#include <remconinterfaceselector.h>        // RemConInterfaceBase.lib
class MediakeyCaptureItemPrivate;
class MediakeyCaptureItem : public QDeclarativeItem
{
    Q_OBJECT
public:
    MediakeyCaptureItem(QDeclarativeItem *parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);


signals:
    void volumeDownPressed();
    void volumeUpPressed();
private:
    MediakeyCaptureItemPrivate *d_ptr;
private:    // Friend class definitions
    friend class MediakeyCaptureItemPrivate;
};
#else
//Dummy class to allow compiling on simulator
class MediakeyCaptureItem : public QDeclarativeItem
{
    Q_OBJECT
public:
    MediakeyCaptureItem(QDeclarativeItem *parent = 0) : QDeclarativeItem(parent) {}
signals:
    void volumeDownPressed();
    void volumeUpPressed();
};
#endif

#endif // MediakeyCaptureItem_H
