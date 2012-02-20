#include "MediaKeyCaptureItem.h"
#ifdef Q_WS_S60

// A private class to access Symbian RemCon API
class MediakeyCaptureItemPrivate : public QObject, public MRemConCoreApiTargetObserver
{
public:
    MediakeyCaptureItemPrivate(MediakeyCaptureItem *parent);
    ~MediakeyCaptureItemPrivate();
    virtual void MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                            TRemConCoreApiButtonAction aButtonAct);
private:
    CRemConInterfaceSelector* iInterfaceSelector;
    CRemConCoreApiTarget*     iCoreTarget;
    MediakeyCaptureItem *d_ptr;
};

// Consructor
MediakeyCaptureItem::MediakeyCaptureItem(QDeclarativeItem *parent): QDeclarativeItem(parent)
{
    d_ptr = new MediakeyCaptureItemPrivate(this);
}

// The paint method
void MediakeyCaptureItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    // This item has no visual
}

// Constructor
MediakeyCaptureItemPrivate::MediakeyCaptureItemPrivate(MediakeyCaptureItem *parent): d_ptr(parent)
{
    QT_TRAP_THROWING(iInterfaceSelector = CRemConInterfaceSelector::NewL());
    QT_TRAP_THROWING(iCoreTarget = CRemConCoreApiTarget::NewL(*iInterfaceSelector, *this));
    iInterfaceSelector->OpenTargetL();
}

// Destructor
MediakeyCaptureItemPrivate::~MediakeyCaptureItemPrivate(){
    delete iInterfaceSelector;
    delete  iCoreTarget;
}

// Callback when media keys are pressed
void MediakeyCaptureItemPrivate::MrccatoCommand(TRemConCoreApiOperationId aOperationId,
                                        TRemConCoreApiButtonAction aButtonAct)
{
    //TRequestStatus status;
    switch( aOperationId )
    {
    case ERemConCoreApiVolumeUp:
        emit d_ptr->volumeUpPressed();
        break;
    case ERemConCoreApiVolumeDown:
        emit d_ptr->volumeDownPressed();
        break;
    default:
        break;
    }
}
#endif // Q_WS_S60
