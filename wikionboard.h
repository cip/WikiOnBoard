/****************************************************************************
**
** Trolltech hereby grants a license to use the Qt/Eclipse Integration
** plug-in (the software contained herein), in binary form, solely for the
** purpose of creating code to be used with Trolltech's Qt software.
**
** Qt Designer is licensed under the terms of the GNU General Public
** License versions 2.0 and 3.0 ("GPL License"). Trolltech offers users the
** right to use certain no GPL licensed software under the terms of its GPL
** Exception version 1.2 (http://trolltech.com/products/qt/gplexception).
**
** THIS SOFTWARE IS PROVIDED BY TROLLTECH AND ITS CONTRIBUTORS (IF ANY) "AS
** IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
** TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
** PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
** OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
** EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
** PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** Since we now have the GPL exception I think that the "special exception
** is no longer needed. The license text proposed above (other than the
** special exception portion of it) is the BSD license and we have added
** the BSD license as a permissible license under the exception.
**
****************************************************************************/

#ifndef WIKIONBOARD_H
#define WIKIONBOARD_H

#include <QtGui/QMainWindow>
#include <QFileDialog>
#include <QSettings>
#include <QDesktopServices>

#include "ui_wikionboard.h"

#include <zim/zim.h>
#include <zim/fileiterator.h>

enum ArticleListItemDataRole {
	ArticleUrlRole=Qt::UserRole,
	ArticleIndexRole,
	ArticleTitleRole
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
    void* m_bgc;
    /*QAction* scrollUpAction;
    QAction* scrollDownAction;*/
    QMenu *optionsMenu;
    QAction* zoomInAction;
    QAction* zoomOutAction;
    QAction* searchArticleAction;
    QAction* clearSearchAction;
        
    QAction* openArticleAction;
    QAction* switchToIndexPageAction;
    QAction* backArticleHistoryAction;
    QAction* toggleFullScreenAction;
    QAction* exitAction;
    QAction* openZimFileDialogAction;
    QAction* downloadZimFileAction;
    QMenu *helpMenu;
    QAction* gotoHomepageAction;
    QAction* aboutCurrentZimFileAction;
    QAction* aboutAction;
    QAction* aboutQtAction;
    zim::File* zimFile;
    int zoomLevel;
    bool hasTouchScreen;
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
        
    
    void openZimFile(QString zimFileName);   
    void clearMenu();
    void moveTextBrowserTextCursorToVisibleArea();
    void showWaitCursor();
    void hideWaitCursor();
    bool openExternalLink(QUrl url);
private slots:
	 void switchToArticlePage();
	 void switchToIndexPage();
 
	 void searchArticle();
     
    //void on_textBrowser_anchorClicked(QUrl url);  
     void backArticleHistoryOrIndexPage();
          
     void on_textBrowser_anchorClicked(QUrl url);
     void on_textBrowser_sourceChanged(QUrl url);  
     
    	 
     
     void openZimFileDialog();
     void downloadZimFile();
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
