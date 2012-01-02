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

#ifndef ARTICLEVIEWERQML_H
#define ARTICLEVIEWERQML_H

#include <QtGui/QGraphicsProxyWidget>
#include <QScrollBar>
#include <qdebug>
#include "zimfilewrapper.h"
#include "articleviewer.h"

class ArticleViewerQML : public QGraphicsProxyWidget
{
    Q_OBJECT
    Q_PROPERTY(bool showImages READ showImages WRITE setShowImages NOTIFY showImagesChanged)
    Q_PROPERTY(int zoomLevel READ zoomLevel WRITE setZoomLevel NOTIFY zoomLevelChanged)
    Q_PROPERTY(int contentHeight READ contentHeight NOTIFY contentHeightChanged)
public:

    ArticleViewerQML(QGraphicsItem* parent = 0)
        : QGraphicsProxyWidget(parent)
    {
        widget = new ArticleViewer(0,0);
        //Scrollbar implemented in QML
        widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        //TODO: Do only on meego, use constant.
        widget->zoomIn(8);        
        setWidget(widget);
        QObject::connect(widget, SIGNAL(backwardAvailable(bool)), this, SIGNAL(backwardAvailable(bool)));
        QObject::connect(widget, SIGNAL(forwardAvailable(bool)), this, SIGNAL(forwardAvailable(bool)));
        QObject::connect(widget, SIGNAL(openExternalLink(QUrl)), this, SIGNAL(openExternalLink(QUrl)));
        QScrollBar *vBar = widget->verticalScrollBar();
        QObject::connect(vBar, SIGNAL(rangeChanged(int,int)), this, SLOT(verticalScrollBarRangeChanged(int,int)));
    }

    bool showImages() const
    {
        return widget->showImages();
    }

    void setShowImages(const bool showImages)
    {
        if (showImages != widget->showImages()) {
            widget->setShowImages(showImages);
            emit showImagesChanged(showImages);
        }
    }

    int zoomLevel() const
    {
        return widget->zoomLevel();
    }

    void setZoomLevel(const int zoomLevel) {
        if (zoomLevel != widget->zoomLevel()) {
            widget->setZoomLevel(zoomLevel);
            emit zoomLevelChanged(zoomLevel);
        }
    }


    int contentHeight() const
    {
        qDebug() << "in contentHeight()";
        int cH = qMax(widget->verticalScrollBar()->maximum(), widget->height());
        qDebug() << "qMax(widget->verticalScrollBar()->maximum(), widget->height()); = qMax("<<widget->verticalScrollBar()->maximum()<<","<<widget->height();
        return cH;
    }

Q_SIGNALS:
     void showImagesChanged(bool showImages);
     void zoomLevelChanged(int zoomLevel);
     void backwardAvailable ( bool available);
     void forwardAvailable ( bool available);
     void openExternalLink( QUrl url);
     void contentHeightChanged(int contentHeight);

public slots:
    void setZimFileWrapper(ZimFileWrapper* zimFileWrapper) {
        widget->setZimFileWrapper(zimFileWrapper);
    }

    void openArticle(QString articleUrl) {
        //TODO use url in qml?
        QUrl url(articleUrl);
        QUrl urlDecoded =url.toString();
        QString urlEncoded = QString::fromUtf8(url.toEncoded().data(),url.toEncoded().length());
        qDebug() << "ArticleViewerQML.openArticle: url (decoded): " <<urlDecoded<<"\nurl (encoded):"<<urlEncoded ;
        widget->setSource(url);
    }

    void backward() {
        qDebug() << "ArticleViewerQML.backward()";
        widget->backward();
    }

    void forward() {
        qDebug() << "ArticleViewerQML.forward()";
        widget->forward();
    }

    bool isBackwardAvailable() {
        qDebug() << "ArticleViewerQML.isBackwardAvailable()";
        return widget->isBackwardAvailable();
    }

    bool isForwardAvailable() {
        qDebug() << "ArticleViewerQML.isForwardAvailable()";
        return widget->isForwardAvailable();
    }

    void verticalScrollBarRangeChanged(int min, int max) {
        qDebug() << "In verticalScrollBarRangeChanged("<<min<<","<<max<<").";
        qDebug() << "Emit contentheightchanged. : " << contentHeight();

        emit contentHeightChanged(contentHeight());
    }

private:
    ArticleViewer *widget;

};

#endif // ARTICLEVIEWERQML_H
