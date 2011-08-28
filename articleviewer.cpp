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

#include "articleviewer.h"
#include <QDebug>
#include <QSettings>

ArticleViewer::ArticleViewer(QWidget* parent, ZimFileWrapper* zimFileWrapper) : QTextBrowser(parent),zimFileWrapper(zimFileWrapper)
 {
    showImages=false;
    //QTextBrowser settings
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setOpenLinks(false);
    //TODO: Check whether text should be selectable. (For copy, may however infere with scrolling)
    setTextInteractionFlags(Qt::LinksAccessibleByKeyboard|Qt::LinksAccessibleByMouse);
    // Textbrowser style.
    setStyleSheet(QLatin1String("QTextBrowser {background-color: white;color: black; border:0px; margin: 0px}"));
    QTextDocument* defaultStyleSheetDocument = new QTextDocument(this);
    //Override link color. At least on symbian per default textbrowser uses phone color scheme which is
    // typically not very ergonomical. (e.g. white text on green background with N97 standard scheme).
    // Text and background color is changed in stylesheet property of textBrowser. Link color (white
    // on N97...) is changed here.
    // Only do this one, and not on every article load as this
    // appearantly affects zoom level.
    defaultStyleSheetDocument->setDefaultStyleSheet(QLatin1String("a:link{color: blue}"));
    setDocument(defaultStyleSheetDocument);

    QSettings settings;
    settings.beginGroup(QLatin1String("UISettings"));
    int zoomInit = settings.value(QLatin1String("zoomLevel"), -1).toInt();
    settings.endGroup();
    zoomLevel = 0;
    zoom(zoomInit);
 }

 QVariant ArticleViewer::loadResource ( int type, const QUrl & name ) {
       if (type==QTextDocument::ImageResource) {
           if (showImages) {
               QString encodedPath = QString::fromUtf8(name.encodedPath().data(),name.encodedPath().length());

               qDebug() << "loadResource.: type is ImageResource and showImages =1 => load image from zim file. " << name.toString()<<"\nurl.path():"<<name.path() << "\nurl.encodedPath():"<< encodedPath;
               return zimFileWrapper->getImageByUrl(encodedPath);
           } else {
              qDebug() << "loadResource: type is ImageResource but showImages=0. Returns 1x1 pixel image. ";
              //Returning one pixel image leads to much faster scrolling than returning empty variant or not handling it at all.
              // (Fix for issue 57).
              QPixmap emptyImage = QPixmap(1,1);
              emptyImage.fill();
              return emptyImage;
           }
       }
       return QVariant();
 }

 void ArticleViewer::toggleImageDisplay(bool checked) {

     showImages = checked;
     QSettings settings;
     settings.beginGroup(QLatin1String("UISettings"));
     if ((!settings.contains(QLatin1String("showImages"))) || (settings.value(QLatin1String("showImages"),
                     true).toBool() != showImages))
             {
             settings.setValue(QLatin1String("showImages"), showImages);
             }
     settings.endGroup();
     reload();
 }


 void ArticleViewer::zoom(int zoomDelta)
         {
         //Limit zoom to allow fixing  an incorrect inifile entry
         // manually by just zooming in or out manually.
         // (In particular as zoom does not saturate, but
         // just do nothing when zoomDelta is out of range.)
         if (zoomDelta > 5)
                 zoomDelta = 5;
         if (zoomDelta < -5)
                 zoomDelta = -5;
         if (abs(zoomLevel + zoomDelta) > 5)
                 {
                 return;
                 }
         if (zoomDelta < 0)
                 {
                 zoomOut(abs(zoomDelta));
                 }
         else
                 {
                 zoomIn(zoomDelta);
                 }
         zoomLevel += zoomDelta;
         QSettings settings;
         settings.beginGroup(QLatin1String("UISettings"));
         if ((!settings.contains(QLatin1String("zoomLevel")))
                         || (settings.value(QLatin1String("zoomLevel")).toInt() != zoomLevel))
                 {
                 settings.setValue(QLatin1String("zoomLevel"), zoomLevel);
                 }
         settings.endGroup();
         }

 void ArticleViewer::zoomOutOneStep()
         {
         zoom(-1);
         }

 void ArticleViewer::zoomInOneStep()
         {
         zoom(1);
         }
