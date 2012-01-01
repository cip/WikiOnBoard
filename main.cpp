/*  WikiOnBoard
    Copyright (C) 2011  Christian Puehringer

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.*/

#include <QtGui/QApplication>
#include <qdeclarative.h>
#include "qmlapplicationviewer.h"
#include "articleviewerqml.h"
#include "indexlistqml.h"
#include <QTranslator>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDeclarativePropertyMap>

#include <QNetworkReply>
#include <QBuffer>
#include <QtConcurrentRun>
#include <QFutureWatcher>

#include <QNetworkAccessManager>
#include <QDeclarativeNetworkAccessManagerFactory>

//Get VERSION from qmake .pro file as string
#define __VER1M__(x) #x
#define __VERM__(x) __VER1M__(x)
#define __APPVERSIONSTRING__ __VERM__(__APPVERSION__)


class ZimReply : public QNetworkReply
{
    Q_OBJECT
public:
    ZimReply(QObject* object, const QNetworkRequest& request)
        : QNetworkReply(object)
        , position(0)
    {
        setRequest(request);
        setOperation(QNetworkAccessManager::GetOperation);
        setHeader(QNetworkRequest::ContentTypeHeader,QVariant(QLatin1String("image/png")));
        open(ReadOnly|Unbuffered);
        setUrl(request.url());
        QString pattern = request.url().queryItemValue(QLatin1String("pattern"));
        Qt::BrushStyle brushStyle = Qt::SolidPattern;
        if (pattern == QLatin1String("dense"))
            brushStyle = Qt::Dense2Pattern;
        else if (pattern == QLatin1String("cross"))
            brushStyle = Qt::CrossPattern;
        else if (pattern == QLatin1String("diagonal"))
            brushStyle = Qt::FDiagPattern;
        const QString radiusString = request.url().queryItemValue(QLatin1String("radius"));
        const QString widthString = request.url().queryItemValue(QLatin1String("width"));
        const QString heightString = request.url().queryItemValue(QLatin1String("height"));
        const QString colorString = request.url().queryItemValue(QLatin1String("color"));
        QColor color(colorString);
        bool ok;
        int radius = radiusString.toInt(&ok);
        if (!ok)
            radius = 5;
        QSize size;
        size.setWidth(widthString.toInt(&ok));
        if (!ok)
            size.setWidth(100);
        size.setHeight(heightString.toInt(&ok));
        if (!ok)
            size.setHeight(100);
        connect(&watcher, SIGNAL(finished()), SLOT(generateDone()));
        QFuture<QByteArray> future = QtConcurrent::run<QByteArray>(generate, size, brushStyle, radius, color);
        watcher.setFuture(future);
    }

    qint64 readData(char* data, qint64 maxSize)
    {
        const qint64 readSize = qMin(maxSize, (qint64)(buffer.size() - position));
        memcpy(data, buffer.constData() + position, readSize);
        position += readSize;
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

    static QByteArray generate(const QSize& size, const Qt::BrushStyle style, int radius, const QColor& color)
    {
        QImage image(size, QImage::Format_ARGB32_Premultiplied);
        image.fill(0);
        QPainter painter(&image);
        QPainterPath path;
        path.addRoundedRect(QRectF(QPoint(0, 0), size), radius, radius);
        painter.fillPath(path, QBrush(color, style));
        QByteArray saveData;
        QBuffer b(&saveData);
        image.save(&b, "PNG");
        return saveData;
    }

public slots:
    void generateDone()
    {
        setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("image/png"));
        position = 0;
        buffer = watcher.result();
        emit readyRead();
        emit finished();
    }

    void abort()
    {
    }
public:
    QNetworkReply* rawReply;
private:
    QFutureWatcher<QByteArray> watcher;
    QByteArray buffer;
    int position;
};

class ZimNetworkAccessManager : public QNetworkAccessManager
{
public:
    ZimNetworkAccessManager(QObject* parent = 0) : QNetworkAccessManager(parent)
    {
    }

    virtual QNetworkReply *createRequest(Operation op, const QNetworkRequest &request, QIODevice *outgoingData)
    {
        if (op != GetOperation || request.url().scheme() != QLatin1String("qt-render") || request.url().host() != QLatin1String("button"))
            return QNetworkAccessManager::createRequest(op, request, outgoingData);
        return new ZimReply(this, request);
    }
};



class MyNetworkAccessManagerFactory : public QDeclarativeNetworkAccessManagerFactory
 {
 public:
     virtual QNetworkAccessManager *create(QObject *parent);
 };

QNetworkAccessManager *MyNetworkAccessManagerFactory::create(QObject *parent)
 {
     QNetworkAccessManager *nam = new ZimNetworkAccessManager(parent);
     return nam;
 }

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication>
           app(createApplication(argc, argv));
    //Load translation
    QTranslator translatorDefault;
    QTranslator translatorLocale;

    qDebug() << "current path:" << QDir::currentPath();    
    qDebug() << "application dir path: "<<QCoreApplication::applicationDirPath();
    //TODO: applicationDirPath is actually default for load anyway, but for other targets
    // than symbian probably necessary to change path.
    QString translationsDir =  QCoreApplication::applicationDirPath();
    qDebug() << "searching translation files in directory: "<<translationsDir;
    //First load english tranlsations. (replaces translation from source code, to separate text and code more)
    if (translatorDefault.load(QLatin1String("wikionboard_en"), translationsDir)) {
        qDebug() <<"Installing translator for english. (Replaces (some) texts from source code)";
        app->installTranslator(&translatorDefault);
    } else {
        qWarning() << "Loading translation file for english failed. Will use text from source code instead, which may be less complete/polished";
    }
    QString locale = QLocale::system().name();
    //Load tranlsation for local language, if available.
    //Note: If translator.load does not find locale file, it automatically strips local name and tries again.
    //  E.g. If wikionboard_de_AT.qsf does not exist, it tries wikionboard_de.qsf next.
    //Note: In case of english locale the same translator is installed twice. This should however
    // not have a negative impact.
    if (translatorLocale.load(QLatin1String("wikionboard_") + locale, translationsDir)) {
        qDebug() << "Installing transloator for locale: "<<locale;
        app->installTranslator(&translatorLocale);
    } else {
        qDebug() << "Loading translation file for locale " << locale << " failed. (english translator is used instead)";
    }
    qmlRegisterType<ZimFileWrapper>("WikiOnBoardModule", 1, 0, "ZimFileWrapper");
    qmlRegisterType<ArticleViewerQML>("WikiOnBoardModule", 1, 0, "ArticleViewerQML");
    qmlRegisterType<IndexListQML>("WikiOnBoardModule", 1, 0, "IndexListQML");

    QScopedPointer<QmlApplicationViewer>
          viewer(QmlApplicationViewer::create());
    QDeclarativeContext *context = viewer->rootContext();
    QDeclarativePropertyMap appInfo;
    appInfo.insert(QLatin1String("version"), QVariant(QString::fromLocal8Bit(__APPVERSIONSTRING__)));
    bool isSelfSigned= false;
    #if __IS_SELFSIGNED__==1
        isSelfSigned = true;
    #endif
    appInfo.insert(QLatin1String("isSelfSigned"), QVariant(isSelfSigned));
    appInfo.insert(QLatin1String("buildDate"), QVariant(QString::fromLocal8Bit(__DATE__)));

    context->setContextProperty(QLatin1String("appInfo"), &appInfo);

    viewer->engine()->setNetworkAccessManagerFactory(new MyNetworkAccessManagerFactory);

    viewer->setMainQmlFile(QLatin1String("qml/WikiOnBoardComponents/main.qml"));
    viewer->showExpanded();

    return app->exec();
}

#include <main.moc>
