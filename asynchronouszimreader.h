#ifndef ASYNCHRONOUSZIMREADER_H
#define ASYNCHRONOUSZIMREADER_H

#include <QThread>
#include <zimfilewrapper.h>
#include <QUrl>

class AsynchronousZimReader : public QThread
{
    Q_OBJECT
public:
    explicit AsynchronousZimReader(QObject *parent = 0, ZimFileWrapper* zimFileWrapper = 0);
    void readAsync(const QUrl& url);

signals:
    void readRequested(const QUrl& url);
    void readDone(const QByteArray& data);
protected:
    void run();
private:
    bool m_ready;
    ZimFileWrapper* zimFileWrapper;
};

class AsynchronousZimReaderWorker: public QObject {
  Q_OBJECT
public:
  explicit AsynchronousZimReaderWorker(QObject *parent = 0, ZimFileWrapper* zimFileWrapper = 0):
        QObject(parent), zimFileWrapper(zimFileWrapper) {}

signals:
  void readDone(const QByteArray &data);
private:
  ZimFileWrapper* zimFileWrapper;
public slots:
  void readFromZimFile(const QUrl& url) {
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId(); // Worker Thread

    QByteArray data;
    if (zimFileWrapper->isValid()) {
        //TODO: path probably not correct
        data = zimFileWrapper->getDataByUrl(url.path());
    } else {
        qDebug() << "Warning: Attempt to open article while no zim file open. Article URL: TODO";
        QString errorString = QLatin1String("<html><head></head><body>No zim file open</body></html>");
        data = errorString.toUtf8();
    }
    emit readDone(data);
  }
};

#endif // ASYNCHRONOUSZIMREADER_H
