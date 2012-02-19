#ifndef IAPWRAPPER_H
#define IAPWRAPPER_H

#include <QObject>
#include <QDebug>
#if defined(Q_OS_SYMBIAN)
//IAP only available in symbian
#include <iapclient.h>
#else
//Dummy IAPClient to allow at least basic testing in simulator
//Note that for other productive targets (e.g. harmattan) obviously
// iap should be disabled completely, which is currently not done.
class IAPClient : public QObject {
    Q_OBJECT
public:
    enum  	ForceRestorationFlag { NoForcedRestoration = 0, ForcedAutomaticRestoration };
    explicit IAPClient(QObject *parent = 0) {}
    int purchaseProduct (QString productId, ForceRestorationFlag restoration) {
        qDebug() << __PRETTY_FUNCTION__ << " (Dummy function for simulator)";
        return -1;}
signals:
    void purchaseCompleted( int requestId, QString status, QString purchaseTicket );
    void purchaseFlowFinished( int requestId );
    void productDataReceived( int requestId, QString status, IAPClient::ProductDataHash productData );

};
#endif

class IAPWrapper : public QObject
{
    Q_OBJECT
public:
    //TODO rename
    Q_PROPERTY(bool isApplicationBusy READ isApplicationBusy NOTIFY applicationBusyChanged)

    explicit IAPWrapper(QObject *parent = 0);
    ~IAPWrapper();

    Q_INVOKABLE int purchaseProduct(QString productId);
    bool isApplicationBusy() const;

signals:
    /*!
      The signal supports ProductCatalogModel::isApplicationBusy property notification
    */
    void applicationBusyChanged();    
    void purchaseCompleted( int requestId, QString status, QString purchaseTicket );
    void purchaseFlowFinished( int requestId );

public slots:
private slots:
    void initIAP();
private:
    // In-Application Purchase API
    IAPClient                       *m_client;
    // enables the whole application busy indicator
    bool                            m_isApplicationBusy;


};

#endif // IAPWRAPPER_H
