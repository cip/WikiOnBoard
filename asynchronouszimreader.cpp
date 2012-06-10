#include <QMetaType>
#include <QDebug>
#include "asynchronouszimreader.h"



AsynchronousZimReader::AsynchronousZimReader(QObject *parent, ZimFileWrapper *zimFileWrapper) :
    QThread(parent), zimFileWrapper(zimFileWrapper), m_ready(false)
{
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId(); // Main Thread
    // Start the worker thread
    start();
    // Wait for the worker thread to be ready;
    while(!m_ready) msleep(50);
}

void AsynchronousZimReader::readAsync(const QUrl &url)
{
    qDebug() << Q_FUNC_INFO << QThread::currentThreadId(); // Main Thread
    emit readRequested(url);
}


void AsynchronousZimReader::run()
{
  qDebug() << Q_FUNC_INFO << QThread::currentThreadId(); // Worker Thread
  // This QObject lives in the worker thread
  AsynchronousZimReaderWorker worker(0, zimFileWrapper); // DO NOT define 'this' pointer as parent

  // Pass read requests to AsynchronousZimReaderWorker in the worker thread
  connect(this, SIGNAL(readRequested(QUrl)),
  &worker, SLOT(readFromZimFile(QUrl))/*, Qt::QueuedConnection*/);
  // Forward the signal to the clients
  connect(&worker, SIGNAL(readDone(QByteArray, QString)), this,
  SIGNAL(readDone(QByteArray, QString))/*, Qt::QueuedConnection*/);
  // Mark the worker thread as ready
  m_ready = true;
  // Event loop (necessary to process signals)
  exec();
}


