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
#include <QDesktopServices>
#include <QtGui/QTextBrowser>
#include "ui_wikionboard.h"

#include <zim/zim.h>
#include <zim/fileiterator.h>

enum ArticleListItemDataRole {
	ArticleUrlRole=Qt::UserRole,
	ArticleIndexRole,
	ArticleTitleRole
};

class ArticleViewer : public QTextBrowser
{
    Q_OBJECT
public:
    ArticleViewer(QWidget* parent = 0);
  //  ~ArticleViewer() {//TODO};
    QVariant loadResource ( int type, const QUrl & name );

};

class WikiOnBoard : public QMainWindow
{
    Q_OBJECT

public:
	WikiOnBoard(void* bgc, QWidget *parent = 0);
        ~WikiOnBoard();


protected:
    void keyPressEvent(QKeyEvent *event);    
    void resizeEvent ( QResizeEvent * event );  
private:     
    Ui::WikiOnBoard ui;
    ArticleViewer* articleViewer;
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
    QAction* exitAction;
    QAction* openZimFileDialogAction;
    QAction* showWelcomePageAction;
    QMenu *helpMenu;
    QAction* gotoHomepageAction;
    QAction* aboutCurrentZimFileAction;
    QAction* aboutAction;
    QAction* aboutQtAction;    

    zim::File* zimFile;
    int zoomLevel;
    bool hasTouchScreen;
    bool fullScreen;
    const QUrl welcomeUrl;
    QString fromUTF8EncodedStdString(std::string s) {
    	return QString::fromUtf8(s.data(), int(s.size())); 
    }
    zim::File::const_iterator getArticleByUrl(QString articleUrl);
    QString getArticleTitleByUrl(QString articleUrl);           
    QString getArticleTextByUrl(QString articleUrl);   
    QString getArticleTextByIdx(QString articleIdx);         
    QString getArticleTextByTitle(QString articleTitle);
    std::pair<bool, QString> getMetaData(QString key);
    QString getMetaDataString(QString key);
    QString byteArray2HexQString(const QByteArray & byteArray);
    QUrl currentlyViewedUrl;    
    void openArticleByUrl(QUrl url);
    QString articleListItemToString(QListWidgetItem *);

    std::pair <bool, QListWidgetItem*> getArticleListItem(zim::File::const_iterator it);
    void populateArticleList(); 
    void populateArticleList(QString articleName, int ignoreFirstN, bool direction_up, bool noDelete=false);
    void articleListSelectPreviousEntry();
    void articleListSelectNextEntry();
        
    
    bool openZimFile(QString zimFileName);
    void moveTextBrowserTextCursorToVisibleArea();
    void showWaitCursor();
    void hideWaitCursor();
    bool openExternalLink(QUrl url);
private slots:
	 void switchToArticlePage();
	 void switchToIndexPage();
         void switchToWelcomePage();
	 void searchArticle();
     
    //void on_textBrowser_anchorClicked(QUrl url);  
     void backArticleHistoryOrIndexPage();
          
     void on_articleViewer_anchorClicked(QUrl url);
     void on_articleViewer_sourceChanged(QUrl url);
     

     
     void openZimFileDialog();
     void gotoHomepage();
     void aboutCurrentZimFile();
     void about();
     
     void articleListOpenArticle(); 	  
     void articleListOpenArticle(QListWidgetItem * item ); 	  
          
     //void on_articleListWidget_itemClicked ( QListWidgetItem * item ); 	  
     void toggleFullScreen();
     void zoom(int zoomDelta);
     void zoomOut();
     void zoomIn();
     int addItemsToArticleList(bool up, int addCount=100, int maxCount=120);
     void enableSplitScreen(); //Enable Split-screen virtual keyboard for symbian.
     void workAreaResized(int screen);
};

class ArticleListFilter : public QObject {
	Q_OBJECT
public:
	ArticleListFilter() {};

protected:
	
    bool eventFilter(QObject *o, QEvent *e);
signals: 
	bool approachingEndOfList(bool up);
};


#endif // WIKIONBOARD_H
