#include "iapwrapper.h"
#include <QTimer>

IAPWrapper::IAPWrapper(QObject *parent) :
    QObject(parent),
    m_isApplicationBusy(false),
    m_client(NULL)
{
    m_isApplicationBusy = true;
    //NOTE from profiling data, instantiating IAP client takes 0.5223 seconds
    // that is why it should be created in a worker thread
    // another point is the following : we cannot create child object for parent in separate thread
    // and even worse, IAPclient implements a dialog thus it must be created in GUI, main thread
    // Taking all above into account we decided to use timer to execute 'initIAP' slot in future
    // TODO: 10 and 100 too low. (gui not updated before iap loaded, which takes quite long
    //        However, 500 seems a little much, try to reduce, or find alternative. (like progress indicator)
    //        (Perhaps use QDeclarativeParserStatus?)
    QTimer::singleShot(500, this, SLOT(initIAP()));

}

IAPWrapper::~IAPWrapper()
{
    delete m_client;
}

int IAPWrapper::purchaseProduct(QString productId)
{
    m_client->purchaseProduct(productId,IAPClient::NoForcedRestoration);
}


void IAPWrapper::initIAP()
{
    if(m_client)
        return; //do it only once

    m_client = new IAPClient(this);
    m_isApplicationBusy = false;
    qDebug() << __PRETTY_FUNCTION__ << ": "<<m_isApplicationBusy;
    connect(m_client, SIGNAL(purchaseFlowFinished(int)),
            this, SIGNAL(purchaseFlowFinished(int)), Qt::QueuedConnection);
    connect(m_client, SIGNAL(purchaseCompleted(int,QString,QString)),
            SIGNAL(purchaseCompleted(int,QString,QString)), Qt::QueuedConnection);
    emit applicationBusyChanged();
}

bool IAPWrapper::isApplicationBusy() const
{
    qDebug() << __PRETTY_FUNCTION__ << ": "<<m_isApplicationBusy;
    return m_isApplicationBusy;
}
