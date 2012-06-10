#include <zimfilewrapper.h>
#include <QNetworkReply>
#include <QBuffer>
#include <QPainter>
#include <QImage>
#include <QColor>
#include <QThread>
#include <asynchronouszimreader.h>

class ZimReply : public QNetworkReply
{
    Q_OBJECT
private:
    static ZimFileWrapper* zimFileWrapper;

public:
    ZimReply(QObject* object, const QNetworkRequest& request)
        : QNetworkReply(object)
        , position(0)
    {        
        setRequest(request);
        setOperation(QNetworkAccessManager::GetOperation);
        open(ReadOnly|Unbuffered);
        setUrl(request.url());
        qDebug() <<"Creating AsynchronousZimReader";
        //TODO: check whether can be optimized reusing thread.
        // (Would require that requests are strictly sequential,
        // which is doubtful)
        AsynchronousZimReader *zimReader = new AsynchronousZimReader(this,ZimReply::zimFileWrapper);

        connect(zimReader, SIGNAL(readDone(QByteArray, QString)),
                SLOT(readFromZimFileDone(QByteArray, QString)));
        zimReader->readAsync(request.url());
    }

    qint64 readData(char* data, qint64 maxSize)
    {
        qDebug() << Q_FUNC_INFO << " for url. " << this->url();
        const qint64 readSize = qMin(maxSize, (qint64)(buffer.size() - position));
        memcpy(data, buffer.constData() + position, readSize);
        position += readSize;
        qDebug() << Q_FUNC_INFO << readSize << " bytes read.";
        return readSize;
    }

    virtual qint64 bytesAvailable() const
    {
        return buffer.size() - position;
    }

    virtual qint64 pos () const
    {
        return position;
    }

    virtual bool seek( qint64 pos )
    {
        if (pos < 0 || pos >= buffer.size())
            return false;
        position = pos;
        return true;
    }

    virtual qint64 size () const
    {
        return buffer.size();
    }


    static void setZimFileWrapper(ZimFileWrapper* zimFileWrapper) {
        ZimReply::zimFileWrapper = zimFileWrapper;        
    }

    static ZimFileWrapper* getZimFileWrapper() {
        return zimFileWrapper;
    }

public slots:
    void readFromZimFileDone(const QByteArray& data, const QString& mimeType)
    {
        qDebug() << Q_FUNC_INFO << " url: " << this->url() << ", mimeType: " <<mimeType;
        if (mimeType.startsWith(QLatin1String("image/png"))) {
            //TODO: If mimeType is correct (image/png) pngs are not displayed.
            // Replacing mimeType is just image (or even  image/jpeg) pngs are displayed.
            //Note: Detected with kiwix-qt-android. With WikiOnBoard android not tested.
            // (Except behavior with all set to text/html -> images displayed, but css not loaded)
            qDebug() << "Workaround for android: mimeType is "<< mimeType << ". -> set MimeType to image";
            setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("image"));
        } else {
            setHeader(QNetworkRequest::ContentTypeHeader, mimeType);
        }
        setHeader(QNetworkRequest::ContentLengthHeader,data.length());
        position = 0;
        buffer = data;
        emit readyRead();
        emit finished();
    }

    void abort()
    {
    }
public:
    QNetworkReply* rawReply;
private:
    QByteArray buffer;
    int position;


};

