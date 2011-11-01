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

#ifndef WIKIONBOARD_H
#define WIKIONBOARD_H

#include <QtGui/QMainWindow>
#include <QFileDialog>
#include <QSettings>
#include <QPushButton>
#include <QDesktopServices>
#include "ui_wikionboard.h"

#include <zim/zim.h>
#include <zim/fileiterator.h>
#include "zimfilewrapper.h"
#include "articleviewer.h"
#include "indexlist.h"

#include <QtDeclarative/QDeclarativeExtensionPlugin>
#include <QtDeclarative/qdeclarative.h>
#include <QtGui/QGraphicsProxyWidget>
#include <qdebug>

class WikiOnBoard : public QMainWindow
{
    Q_OBJECT
public:
    WikiOnBoard(void* bgc, QWidget *parent = 0);
    ~WikiOnBoard();
    QSize getMaximumDisplaySizeInCurrentArticleForImage(QString imageUrl);


protected:
    void keyPressEvent(QKeyEvent *event);    
    bool openZimFile(QString zimfilename);
private:     
    ZimFileWrapper* zimFileWrapper;
    Ui::WikiOnBoard ui;
    ArticleViewer* articleViewer;
    IndexList* indexList;
    void* m_bgc;
    QAction* positiveSoftKeyActionMenuIndexPage;
    QAction* positiveSoftKeyActionMenuArticlePage;
    QAction* positiveSoftKeyActionMenuArticlePageNoFileOpen;
    QMenu* menuIndexPage;
    QMenu* menuArticlePage;
    QMenu* menuArticlePageNoFileOpen;
    QAction* zoomInAction;
    QAction* zoomOutAction;
    QAction* searchArticleAction;
    QAction* clearSearchAction;

    QAction* openArticleAction;
    QAction* switchToIndexPageAction;
    QAction* backArticleHistoryAction;
    QAction* emptyAction;
    QAction* toggleFullScreenAction;
    QAction* toggleImageDisplayAction;

    QAction* exitAction;
    QAction* openZimFileDialogAction;
    QAction* showWelcomePageAction;
    QMenu *helpMenu;
    QAction* gotoHomepageAction;
    QAction* aboutCurrentZimFileAction;
    QAction* aboutAction;
    QAction* aboutQtAction;    

    bool hasTouchScreen;
    bool fullScreen;
    QString fromUTF8EncodedStdString(std::string s) {
    	return QString::fromUtf8(s.data(), int(s.size())); 
    }
    QString articleListItemToString(QListWidgetItem *);

    std::pair <bool, QListWidgetItem*> getArticleListItem(zim::File::const_iterator it);

    void showWaitCursor();
    void hideWaitCursor();
    QString byteArray2HexQString(const QByteArray &byteArray);
private slots:
    void switchToArticlePage();
    void switchToIndexPage();
    void switchToWelcomePage();
    void searchArticle();
    //FIXME void on_articleViewer_sourceChanged(QUrl url);
    void onArticleOpened(QString articleTitle);

    void backArticleHistoryOrIndexPage();

    void openZimFileDialog();
    void gotoHomepage();
    void aboutCurrentZimFile();
    void about();

    void articleListOpenArticle();
    void articleListOpenArticle(QListWidgetItem * item );

    void toggleFullScreen();
    void enableSplitScreen(); //Enable Split-screen virtual keyboard for symbian.
    void workAreaResized(int screen);
    bool openExternalLink(QUrl url);

};

class ArticleViewerQML : public QGraphicsProxyWidget
{
    Q_OBJECT
  //  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    ArticleViewerQML(QGraphicsItem* parent = 0)
        : QGraphicsProxyWidget(parent)
    {
        widget = new ArticleViewer(0,0);
        setWidget(widget);
        QObject::connect(widget, SIGNAL(clicked(bool)), this, SIGNAL(clicked(bool)));
    }

    /*QString text() const
    {
        return widget->text();
    }

    void setText(const QString& text)
    {
        if (text != widget->text()) {
            widget->setText(text);
            emit textChanged();
        }
    }*/
/*
Q_SIGNALS:
    void clicked(bool);
    void textChanged();
*/
public slots:    
    void setZimFileWrapper(ZimFileWrapper* zimFileWrapper) {
        widget->setZimFileWrapper(zimFileWrapper);
        widget->toggleImageDisplay(true); //TODO: use settings
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

private:
   // WikiOnBoard *wikionboard;
   // QPushButton *widget;
    ArticleViewer *widget;

};


class 	IndexListQML : public QGraphicsProxyWidget
{
    Q_OBJECT
  //  Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    IndexListQML(QGraphicsItem* parent = 0)
        : QGraphicsProxyWidget(parent)
    {

        widget = new IndexList(0, 0);
        widget->setAttribute(Qt::WA_NoSystemBackground);
        setWidget(widget);
        QObject::connect(widget, SIGNAL(itemClicked(QListWidgetItem*)), this,
                        SLOT(itemClicked(QListWidgetItem *)));

        //QObject::connect(widget, SIGNAL(clicked(bool)), this, SIGNAL(clicked(bool)));
    }

Q_SIGNALS:
      void openArticle(const QString articleUrl);

public slots:
    void setZimFileWrapper(ZimFileWrapper* zimFileWrapper) {
        widget->setZimFileWrapper(zimFileWrapper);
    }

    void searchArticle(QString articleTitle) {
        qDebug() << "IndexListQML.searchArticle: articleTitle: " <<articleTitle;
        widget-> populateArticleList(articleTitle);
    }

private slots:
      void itemClicked(QListWidgetItem* item) {
          QString url;
          if (item != NULL) {
              url = item->data(ArticleUrlRole).toUrl().toString();
          }
          qDebug() << "In IndexListQML itemclicked. emit openarticle(articleUrl="<<url<<")";
          emit openArticle(url);
      }

private:

   // WikiOnBoard *wikionboard;
   // QPushButton *widget;
    IndexList *widget;

};

#endif // WIKIONBOARD_H
