#include "iapwrapper.h"
#include <QTimer>

IAPWrapper::IAPWrapper(QObject *parent) :
    QObject(parent),
    m_isApplicationBusy(false),
    m_client(NULL)
{
    m_isApplicationBusy = true;    
}

IAPWrapper::~IAPWrapper()
{
    delete m_client;
}

int IAPWrapper::getProductData(QString productId)
{
    qDebug() << __PRETTY_FUNCTION__ << ": productId:"<<productId;
    return m_client->getProductData(productId);
}

int IAPWrapper::purchaseProduct(QString productId)
{
    qDebug() << __PRETTY_FUNCTION__ << ": productId:"<<productId;
    return m_client->purchaseProduct(productId,IAPClient::NoForcedRestoration);
}


void IAPWrapper::initIAP()
{
    if(m_client)
        return; //do it only once

    m_client = new IAPClient(this);
    qRegisterMetaType<IAPClient::ProductDataHash>("IAPClient::ProductDataHash");
    m_isApplicationBusy = false;
    qDebug() << __PRETTY_FUNCTION__ << ": "<<m_isApplicationBusy;
    connect(m_client, SIGNAL(productDataReceived( int, QString, IAPClient::ProductDataHash)),
            this, SLOT(productDataReceivedInt( int, QString, IAPClient::ProductDataHash)), Qt::QueuedConnection);
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

void IAPWrapper::productDataReceivedInt(int requestId, QString status,
                                              IAPClient::ProductDataHash productData)
{
    qDebug() << __PRETTY_FUNCTION__ << ": requestId:"<<requestId;
    emit productDataReceived(requestId,
                             status,
                             productData.value(QLatin1String("info")).toString(),
                             productData.value(QLatin1String("shortdescription")).toString(),
                             productData.value(QLatin1String("price")).toString(),
                             productData.value(QLatin1String("result")).toString());
}


