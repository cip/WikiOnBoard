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
#include <QTime>
#include <QTextBlock>
#include <QtScroller>
#include <QKeyEvent>
#include <QApplication>

ArticleViewer::ArticleViewer(QWidget* parent, ZimFileWrapper* zimFileWrapper, bool hasTouchScreen)
    : QTextBrowser(parent),zimFileWrapper(zimFileWrapper),hasTouchScreen(hasTouchScreen)
 {
    showImages=false;
    articleTitle = QString();
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
    //  LeftMouseButtonGesture used, as use of TouchGesture together
    // with mouse click events (like link clicked) problematic.
    QtScroller::grabGesture(viewport(), QtScroller::LeftMouseButtonGesture);
    if (connect(this,SIGNAL(sourceChanged(QUrl)),this, SLOT(onSourceChanged(QUrl)))) {
        qDebug() << "Connected sourceChanged";
    } else {
        qWarning()<<"Could not connect sourceChanged";
    }
    if (connect(this,SIGNAL(anchorClicked(QUrl)),this, SLOT(onAnchorClicked(QUrl)))) {
        qDebug() << "Connected anchorClicked";
    } else {
        qWarning()<<"Could not connect anchorClicked";
    }
 }

//
 QSize ArticleViewer::getMaximumDisplaySizeInCurrentArticleForImage(QString imageUrl) {
    QSize size;
    for (QTextBlock it = document()->begin(); it != document()->end(); it = it.next()) {
        //          qDebug() << it.text();
        QTextBlock::iterator fragit;
        for (fragit = it.begin(); !(fragit.atEnd()); ++fragit) {
            QTextFragment currentFragment = fragit.fragment();
            if (currentFragment.isValid()) {
                QTextCharFormat charFormat= currentFragment.charFormat();
                if (charFormat.isImageFormat()) {
                    //qDebug() << "char format image name" <<charFormat.toImageFormat().name()<< "size: "<<charFormat.toImageFormat().width()<<" x "<< charFormat.toImageFormat().height();
                    if (charFormat.toImageFormat().name()==imageUrl) {
                        //TODO: Is this comparision really reliable?
                        QSize tmpSize = QSize(charFormat.toImageFormat().width(),charFormat.toImageFormat().height());
                        if (!size.isValid()) {
                            size =tmpSize;
                        } else {
                            qDebug() << "Same image referenced multiple times. Current image size "<< tmpSize << " maximum size up to now "<<size;
                            size = size.expandedTo(tmpSize);
                        }
                        qDebug() << " size of to be loaded image: "<<size;
                    }

                }
            }
        }
    }
    return size;
 }

 QVariant ArticleViewer::loadResource ( int type, const QUrl & name ) {
       if (type==QTextDocument::ImageResource) {
           if (showImages) {
               QString encodedPath = QString::fromUtf8(name.encodedPath().data(),name.encodedPath().length());
               QTime timer;
               qDebug() << "loadResource.: type is ImageResource and showImages =1 => load image from zim file. " << name.toString()<<"\nurl.path():"<<name.path() << "\nurl.encodedPath():"<< encodedPath;
               timer.start();
               QSize newSize = getMaximumDisplaySizeInCurrentArticleForImage(encodedPath);
               qDebug() << " Searching image size took " << timer.elapsed() << " milliseconds";
               return zimFileWrapper->getImageByUrl(encodedPath,newSize);
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

 void ArticleViewer::moveTextBrowserTextCursorToVisibleArea()
         {
         int position = cursorForPosition(QPoint(0, 0)).position();
         QTextCursor cursor = textCursor();
         cursor.setPosition(position, QTextCursor::MoveAnchor);
         setTextCursor(cursor);
         }

 void ArticleViewer::openArticleByUrl(QUrl url)
 {
     QString path = url.path();
     QString encodedPath = QString::fromUtf8(url.encodedPath().data(),url.encodedPath().length());

     qDebug() << "openArticleByUrl: " <<url.toString()<<"\nurl.path():"<<path << "\nurl.encodedPath():"<< encodedPath;

     /*FIXME if (url==welcomeUrl) {
        qDebug()  << "Url is welcome URL. Set article text to welcome text";
         QString zimDownloadUrl = QString(tr("https://github.com/cip/WikiOnBoard/wiki/Get-eBooks","Change link to page with localized zim files. (e.g https://github.com/cip/WikiOnBoard/wiki/Get-eBooks-DE"));
         QString getEBookLinkCaption = QString(tr("Download zimfile", "link"));
         QString zimDownloadUrlHtml = QString(tr("<a href=\"%1\">%2</a>", "DON'T translate this").arg(zimDownloadUrl,getEBookLinkCaption));

         QString informativeText = QString(tr("[TRANSLATOR] No zimfile selected. getEBook link  %1 opens url %3 with info where to get eBooks. Menu option %2 in option menu %4 opens zimfile on mobile", "Text is interpreted as HTML. Html for body and link (%1) automatically added. Other Html tags can be used if desired")).arg(zimDownloadUrlHtml,openZimFileDialogAction->text(),zimDownloadUrl, positiveSoftKeyActionMenuArticlePage->text());
         setHtml(informativeText);

     } else */ if (zimFileWrapper->isValid()) {

         //Only read article, if not same as currently
         //viewed article (thus don´t reload for article internal links)
         //TODO: this does not work as appearantly before calling changedSource
         // content is deleted. Therefore for now just reload in any case url.
         // Optimize (by handling in anchorClicked, but check what happens
         //	to history then)
         //if (!path.isEmpty() && (currentlyViewedUrl.path()!=url.path())) {

         articleTitle = zimFileWrapper->getArticleTitleByUrl(encodedPath);

         QTime timer;
         timer.start();
         QString articleText = zimFileWrapper->getArticleTextByUrl(encodedPath);
         qDebug() << "Reading article " <<path <<" from zim file took" << timer.restart() << " milliseconds";
         setHtml(articleText);
         qDebug() << "Loading article into textview (setHtml()) took" << timer.restart() << " milliseconds";
         if (url.hasFragment())
         {
             //Either a link within current file (if path was empty), or to   newly opened file
             QString fragment = url.fragment();
             scrollToAnchor(fragment);
             //Now text visible, but cursor not moved.
             //=> Move cursor to current visisble location.
             // TODO: no better way to achieve this. Furthermore, actually
             //	cursor reason for problem or something else?)
             moveTextBrowserTextCursorToVisibleArea();
         }
         else
         {
             QTextCursor cursor = textCursor();

             //Move cursor to start of file
             cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
             setTextCursor(cursor);
             if (hasTouchScreen == false)
             {
                 //FIXME: This is a really ugly hack for the nextprevious link problem
                 // described in keyEventHandler. Note that for links with anchor this
                 // does not work
                 // On touchscreen devices workaround is not performed.
                 QKeyEvent *remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
                                                             Qt::Key_Up, Qt::NoModifier, QString(), false, 1);
                 QApplication::sendEvent(this, remappedKeyEvent);
                 remappedKeyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down,
                                                  Qt::NoModifier, QString(), false, 1);
                 QApplication::sendEvent(this, remappedKeyEvent);

             }
         }
         qDebug() << "Loading article into textview (gotoAnchor/moveposition) took" << timer.restart() << " milliseconds";
     } else {
         qWarning() << "openArticleByUrl called with non welcome page url while no zim file open. Should not happen";
     }
 }

 void ArticleViewer::onAnchorClicked(QUrl url)
         {
         qDebug() << "onAnchorClicked: Url: " << url.toString();
         qDebug() << " Check  url.scheme(). " <<url.scheme();
         if ((QString::compare(url.scheme(), QLatin1String("http"), Qt::CaseInsensitive)==0)||
                 (QString::compare(url.scheme(), QLatin1String("https"), Qt::CaseInsensitive)==0))
                 {
                     qDebug() << "url scheme is http or https. Emit openExternalLink signal";
                     emit openExternalLink(url);
                 }
         else
                 {
                         qDebug() << "Url is not an external website => search in zim file";
                         setSource(url);
                 }
         }

 void ArticleViewer::onSourceChanged(QUrl url)
         {
         showWaitCursor();
         openArticleByUrl(url);
         emit articleOpened(articleTitle);
         hideWaitCursor();
         }


 void ArticleViewer::showWaitCursor()
         {
         //If mouse cursor in edge of screen (which is the case for non-touch
         // smartphones often, move it to the middle of main widget)
         if ((QCursor::pos().x() == 0) && (QCursor::pos().y()==0)) {
                 QCursor::setPos(this->mapToGlobal(QPoint(this->width()/2,this->height()/2)));
         }
         //Force cursor visible on all platforms
         #if defined(Q_OS_SYMBIAN)
             QApplication::setNavigationMode(Qt::NavigationModeCursorForceVisible);
         #endif
         //On Symbian^3 waitcursor not working for some reason.
         QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
         // processEvent leads clears article page while loading.
         // (At least some user feedback on S^3 that something is going on...)
         qApp->processEvents();
 }

 void ArticleViewer::hideWaitCursor()
         {
         QApplication::restoreOverrideCursor();
         #if defined(Q_OS_SYMBIAN)
             QApplication::setNavigationMode(Qt::NavigationModeNone);
         #endif
 }

