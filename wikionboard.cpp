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

//TODO: Find some replacement for hourglass. (Not working anymore as mouse cursor disabled)
//TODO fix curso in textedit (don't move on left/right). Consider however before the
// the sync. lost during scrolling issue.
#include "wikionboard.h"
#include <QKeyEvent>
#include <QDebug>
#include <QProgressBar>
#include <QScrollBar>
#include <QMessageBox>
#include <QStringBuilder>
#include <QTextCodec>
//"Official" kinetic scrolling. (Backport from Qt 4.8) 
//	See http://qt.gitorious.org/qt-labs/kineticscroller/commits/solution and
//		http://bugreports.qt.nokia.com/browse/QTBUG-9054?focusedCommentId=130700&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#action_130700
#include <QtScroller>
#include <qtscrollevent>



//#include <QElapsedTimer>
//TODO: not necessary for symbian, why necessary on linux? (and anyway exception should be replaced
//  by something else
#include <stdexcept>

#if defined(Q_OS_SYMBIAN)
#include <eikbtgpc.h>       // symbian: LIBS += -lavkon -leikcoctl
#endif

//Get VERSION from qmake .pro file as string 
#define __VER1M__(x) #x
#define __VERM__(x) __VER1M__(x)
#define __APPVERSIONSTRING__ __VERM__(__APPVERSION__)


//To update article list during scrolling. FIXME not working very well yet
    bool ArticleListFilter::eventFilter(QObject *o, QEvent *e)
	{
	switch (e->type())
		{
		case QtScrollPrepareEvent::ScrollPrepare:
			{
			QtScrollPrepareEvent *se = static_cast<QtScrollPrepareEvent *> (e);
			qDebug() << " ScrollPrepare: " << se->startPos();
			QWidget *w = static_cast<QWidget *> (o);				
			return false;
			}
		case QtScrollEvent::Scroll:
			{
			QtScrollEvent *se = static_cast<QtScrollEvent *> (e);
			
			QWidget *w = static_cast<QWidget *> (o);
			
			if (w->parentWidget())
				{				
				if (QListWidget *lw = qobject_cast<QListWidget *>(w->parentWidget()))
					{
					qreal delta= 40.0; //FIXME don't use constant
								
				//	qreal delta= 840.0; //FIXME don't use constant
							// Verticalscrollbarmaxim=4289, 100 items => itemheight ~42
							// Asssume <20 left to end 20*42=840.
					if (lw->viewport() == w)
						{
						if (se->scrollState()==QtScrollEvent::ScrollStarted) {
							qDebug() << "New Scrolling activity started. ";
							qDebug() << " Scroller final position: " << QtScroller::scroller(w)->finalPosition();
						}						
						if (se->contentPos().y() > lw->verticalScrollBar()->maximum() -delta)
					//	if (QtScroller::scroller(w)->finalPosition().y()==lw->verticalScrollBar()->maximum())
						{
							if (QtScroller::scroller(w)->velocity().y() > 0.0) {
								qDebug() << " Scroller final position: " << QtScroller::scroller(w)->finalPosition();
								qDebug() << " ScrollEvent. ScrollState: " << se->scrollState()
												<< " contentPos: " << se->contentPos();
								qDebug() << " Velocity " << QtScroller::scroller(w)->velocity();
								qDebug() << "ArticleList: vminimum "	<< lw->verticalScrollBar()->minimum();
											
								qDebug() << "ArticleList: vmaximum "
																<< lw->verticalScrollBar()->maximum();
														
								QtScroller::scroller(w)->stop();				
														
							    approachingEndOfList(false);
								return true;												
							}
													
						}														
						 else if (se->contentPos().y() < lw->verticalScrollBar()->minimum()+delta)													
						//else if (QtScroller::scroller(w)->finalPosition().y()==lw->verticalScrollBar()->minimum())
							
							{
							if (QtScroller::scroller(w)->velocity().y() < 0.0) {
								qDebug() << " Scroller final position: " << QtScroller::scroller(w)->finalPosition();		
								qDebug() << " ScrollEvent. ScrollState: " << se->scrollState()
												<< " contentPos: " << se->contentPos();
							    qDebug() << " Velocity " << QtScroller::scroller(w)->velocity();
								qDebug() << "ArticleList: vminimum "	<< lw->verticalScrollBar()->minimum();											
							    qDebug() << "ArticleList: vmaximum "
							    								<< lw->verticalScrollBar()->maximum();
							    QtScroller::scroller(w)->stop();						
							    //TODO: Overshoot. Trial not working: if not items added return false (event further processed for overhoot) 							    								
							    approachingEndOfList(true);
							    return true;
							    //Return true to prevent that default QScroller eventfilter scrolls  
								// to beginning of list and stop scroller. (Nicer would be sure
								// if it would continue scrolling from the new position with 
								// the same speed). TODO try to do this (e.g. void QScroller::resendPrepareEvent () )
								// QtScroller::scroller(w)->stop(); //Does not really work. Sometimes it works. (Stops, and shows
								//   element focused by approachingEndOflist, but mostly it "jumps" to first element of list)
								//Interestingly behavior basically uncahnged with resendPr. 
								//TODO
								//QtScroller::scroller(w)->resendPrepareEvent();
								//Note:Without deleting elements in approachingEndOfList this worked better.
								//Note: Could this be a concurrency issue? (are events queued??)=> Try with stopping before.
								// => Looks like with stopping before it works fine. (Although a little rough, as it jumps the last
								// (use scrollTo of scroller for smooth scrolling??) do something clever with resendPrepareEvent()? (Diffuclt)
								// Perhaps also just reload when first element hit. (stopping before is useless if it cannot continue later)
								// (Or throw all away and use modelview framework)
								
								//TODO: knwn issues:
								//	Jumps some articles. (Because adding article is started before first article visible
								//		DONE Simple fix would be to start adding articles only when first/last article reached. 
								//		However of this starting earlier that adding is done in background. (More complex option)
								
							}
						}

						}
					}
				}

			return false;
			}
		default:
			return false;

		}

	return false;
	}


    

WikiOnBoard::WikiOnBoard(void* bgc, QWidget *parent) :
	QMainWindow(parent), m_bgc(bgc)
	{			
	//For now assume that: S60 rd 3dition devices have no touch screen, all other devices have touch screen.
	// TODO Consider changing to QSystemDeviceInfo when Qt Mobility available for all supported devices
	hasTouchScreen = true;
	#ifdef Q_OS_SYMBIAN	
	if ((QSysInfo::s60Version()==QSysInfo::SV_S60_3_1) || (QSysInfo::s60Version()==QSysInfo::SV_S60_3_2)) {
		qDebug() << "S60 3rd edition device. Assume that no touchscreen is available.";
		hasTouchScreen = false;
	}
	#endif
	
	qDebug() << "WikiOnBoard::WikiOnBoard. Version: " << QString::fromLocal8Bit(__APPVERSIONSTRING__) << " QT Version: "<<qVersion();
	qDebug() << " hasTouchScreen: "<<hasTouchScreen;
	
	zimFile = NULL; //zimFile unitialized until,
	//file loaded (either stored filename from last run,
	// or user loads file). To allow test for == NULL, explicitlz
	// set to null. (at least in symbian emulator else 0xCCCCCCCC

	QSettings settings;
	settings.beginGroup(QLatin1String("ZimFile"));

	QString lastZimFile = settings.value(QLatin1String("lastZimFile")).toString();
	settings.endGroup();
	if (!lastZimFile.isEmpty())
		{
		openZimFile(lastZimFile);
		}

	ui.setupUi(this);
	setStatusBar(0); //Remove status bar to increase useable screen size.
	//TODO still not perfect, quite some distance between
	//  widgeht and menu.

	settings.beginGroup(QLatin1String("UISettings"));
	int zoomInit = settings.value(QLatin1String("zoomLevel"), -1).toInt();
	bool fullScreen = settings.value(QLatin1String("fullScreen"), false).toBool();
	settings.endGroup();
	QTextDocument* defaultStyleSheetDocument = new QTextDocument(this);
	//Override link color. At least on symbian per default textbrowser uses phone color scheme which is
	// typically not very ergonomical. (e.g. white text on green background with N97 standard scheme). 
	// Text and background color is changed in stylesheet property of textBrowser. Link color (white
	// on N97...) is changed here. 
	// Only do this one, and not on every article load as this
	// appearantly affects zoom level. 
	defaultStyleSheetDocument->setDefaultStyleSheet(QLatin1String("a:link{color: blue}"));	
	ui.textBrowser->setDocument(defaultStyleSheetDocument);		
	zoomLevel = 0;
	zoom(zoomInit);
	//  LeftMouseButtonGesture used, as use of TouchGesture together
	// with mouse click events (like link clicked) problematic.
	QtScroller::grabGesture(ui.textBrowser->viewport(), QtScroller::LeftMouseButtonGesture);
					
	// ScrollPerPixel required for kinetic scrolling
	ui.articleListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);	    
	QtScroller::grabGesture(ui.articleListWidget->viewport(), QtScroller::LeftMouseButtonGesture);
	ArticleListFilter *articleListFilter = new ArticleListFilter();
	ui.articleListWidget->viewport()->installEventFilter(articleListFilter);
	connect(articleListFilter,SIGNAL(approachingEndOfList(bool)),this, SLOT(approachingEndOfList(bool)));
	QtScrollerProperties properties = QtScroller::scroller(ui.textBrowser->viewport())->scrollerProperties();
	//properties.setScrollMetric(QtScrollerProperties::DragStartDistance,
	 //	                                QVariant(1.0/1000)); 
	//properties.setScrollMetric(QtScrollerProperties::DragVelocitySmoothingFactor,
		// 	 	                                QVariant(0.9)); 
		
//	properties.setScrollMetric(QtScrollerProperties::AcceleratingFlickMaximumTime,
	// 	 	 	                                QVariant(0.0));
	//Avoid scrolling right/left for up/down gestures. Higher value
	// would be better regarding this, but then finger scrolling is
	// is not working very well. 
	
	//properties.setScrollMetric(QtScrollerProperties::AxisLockThreshold,
	 //			 	 	 	                                QVariant(0.2));	 		 	 
	//properties.setScrollMetric(QtScrollerProperties::DecelerationFactor,
	 //		 	 	 	 	 	                                QVariant(0.400));			 		 	 
//	properties.setScrollMetric(QtScrollerProperties::MaximumVelocity,
  //               QVariant(200.0/1000.0));	
	//properties.setScrollMetric(QtScrollerProperties::MousePressEventDelay,
	  //               QVariant(0.2));	
			 
	QtScroller::scroller(ui.textBrowser->viewport())->setScrollerProperties(properties);
	
#ifdef Q_OS_SYMBIAN
	//Enable Softkeys in fullscreen mode. 
    //New Flag in Qt 4.6.3. 
	//Workaround used for 4.6.2 (see main.cpp for details) 
	//not working anymore with Qt 4.6.3
	//However, 4.6.2 workaround keep, as perhaps still
	// working/required for 4.6.2. 
	Qt::WindowFlags flags = windowFlags();
	flags |= Qt::WindowSoftkeysVisibleHint;
	setWindowFlags(flags);
#endif
	if (fullScreen)
		{
		toggleFullScreen();
		}
	else
		{
		showMaximized();
		}
	currentlyViewedUrl = QUrl(QLatin1String(""));

	openZimFileDialogAction = new QAction(tr("Open Zimfile"), this);
	connect(openZimFileDialogAction, SIGNAL(triggered()), this,
			SLOT(openZimFileDialog()));

	downloadZimFileAction = new QAction(tr("Download Zimfile"), this);
	connect(downloadZimFileAction, SIGNAL(triggered()), this,
			SLOT(downloadZimFile()));
	gotoHomepageAction = new QAction(tr("Goto Homepage"), this);
	connect(gotoHomepageAction, SIGNAL(triggered()), this,
			SLOT(gotoHomepage()));
	aboutCurrentZimFileAction = new QAction(tr("About current Zimfile"), this);
	connect(aboutCurrentZimFileAction, SIGNAL(triggered()), this, 
			SLOT(aboutCurrentZimFile()));
	aboutAction = new QAction(tr("About"), this);
	connect(aboutAction, SIGNAL(triggered()), this,
			SLOT(about()));
	aboutQtAction = new QAction(tr("About Qt"), this);
	connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
				
		
	//Define search action (populates article list view with articles found searching for article
	//name line edit.
	// 
	searchArticleAction = new QAction(tr("Search Article"), this);

	connect(searchArticleAction, SIGNAL(triggered()), this,
			SLOT(searchArticle()));
	//connect(ui.articleName, SIGNAL(textChanged(QString)), searchArticleAction,
	//		SIGNAL(triggered())); //Automatically search after text changed
	//TODO: this hangs, instead textEdited used. find out why not working 
	connect(ui.articleName, SIGNAL(textEdited(QString)), searchArticleAction,
			SIGNAL(triggered())); //Automatically search after text changed


	ui.articleName->addAction(searchArticleAction);
	//Capitalize first letter. In particular important as zimlib
	// search is case-sensitive and in wikipedia most articles start with
	// captial letter.
	// Somewhat strange that this work. (Actually defaullt should be Imhnone
	// anyway, but appearantly it is not on symbian. Just calling ImhNone,
	// does not do anything, because it thinks that nothing has changed. 
	// Setting something different (here ImhPreferUppercase) and then ImhNone,
	// sets it to the desired Abc mode.
	ui.articleName->setInputMethodHints(Qt::ImhPreferUppercase); 
	ui.articleName->setInputMethodHints(Qt::ImhNone);
	clearSearchAction = new QAction(tr("Clear"), this);
	connect(clearSearchAction, SIGNAL(triggered()), ui.articleName,
				SLOT(clear()));
	connect(clearSearchAction, SIGNAL(triggered()), searchArticleAction,
					SIGNAL(triggered())); //Automatically search after cleared.

	this->addAction(clearSearchAction);
		
	openArticleAction = new QAction(tr("Open Article"), this);

	connect(openArticleAction, SIGNAL(triggered()), this,
			SLOT(articleListOpenArticle()));
	connect(ui.articleListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this,
			SLOT(articleListOpenArticle(QListWidgetItem *))); //For touchscreen devices
	//slot just calls openArticlAction.trigger

	//connect(ui.articleListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(articleListOpenArticle(QListWidgetItem *)));
	ui.articleListWidget->addAction(openArticleAction);

	//Open article when clicked or return key clicked. Note that for keypad phones return (amongst others) is
	// forwarded to articleListWidget, so that this works even if it has not focus.

	switchToIndexPageAction = new QAction(tr("Switch to index page"), this);
	connect(switchToIndexPageAction, SIGNAL(triggered()), this,
			SLOT(switchToIndexPage()));
	this->addAction(switchToIndexPageAction);

	//	ui.actionSearch->setSoftKeyRole(QAction::NegativeSoftKey); //Right softkey: return to search page
	///	ui.articlePage->addAction(ui.actionSearch);

	backArticleHistoryAction = new QAction(tr("Back"), this);
	connect(backArticleHistoryAction, SIGNAL(triggered()), this,
			SLOT(backArticleHistoryOrIndexPage()));
	this->addAction(backArticleHistoryAction); 
	
	toggleFullScreenAction = new QAction(tr("Toggle Fullscreen"), this); //TODO shortcut
	toggleFullScreenAction->setShortcutContext(Qt::ApplicationShortcut); //Or Qt::WindowShortcut?
	connect(toggleFullScreenAction, SIGNAL(triggered()), this,
			SLOT(toggleFullScreen()));
	this->addAction(toggleFullScreenAction);

	exitAction = new QAction(tr("Exit"), this);
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
	this->addAction(exitAction);

	zoomInAction = new QAction(tr("Zoom in"), this);
	zoomOutAction = new QAction(tr("Zoom out"), this);
	connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
	connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

	// Set context menu policy for widgets to suppress the useless 'Actions' submenu
#ifdef Q_OS_SYMBIAN
	QWidgetList widgets = QApplication::allWidgets();
	QWidget* w = 0;
	foreach( w, widgets )
			{
			w->setContextMenuPolicy(Qt::NoContextMenu);
			}
#endif

	switchToIndexPage();
	}

WikiOnBoard::~WikiOnBoard()
	{

	}

void WikiOnBoard::openZimFile(QString zimFileName)
	{
	try
		{
		QRegExp rx(QLatin1String("(.*\\.zim)\\D\\D"));
	    rx.setCaseSensitivity(Qt::CaseInsensitive);
	    zimFileName.replace(rx,QLatin1String("\\1"));
	    //TODO: Default is latin1 encoding. Correct? (at least zim files with
	    // special characters do not open correctly, but can this be fixed by other encoding?)
		std::string zimfilename = zimFileName.toStdString(); 
		zimFile = new zim::File(zimfilename);
		}
	catch (const std::exception& e)
		{
			QMessageBox::StandardButton reply;
		     reply = QMessageBox::critical(this, tr("Error on opening zim file"),
		                                     QString::fromStdString(e.what()),
		                                     QMessageBox::Ok);		 
		}
	}

QString WikiOnBoard::getArticleTextByIdx(QString articleIdx)
	{
	QString articleText = QLatin1String("ERROR");
	zim::Blob blob;
	try
		{
		bool ok;
		int idx = articleIdx.toInt(&ok, 10);
		zim::Article article = zimFile->getArticle(idx);
		if (article.isRedirect())
			{
			article = article.getRedirectArticle();
			}
		blob = article.getData();
		articleText = QString::fromUtf8(blob.data(), blob.size());
		}
	catch (const std::exception& e)
		{
		return QString::fromStdString(e.what());
		}

	return articleText;
	}

//Note: expects encoded URL (as used in articles). Therefore don't use
// this for decoded URL (as in zim file index)
QString WikiOnBoard::getArticleTextByUrl(QString articleUrl)
	{
	QString articleText = QLatin1String("ERROR");
	zim::Blob blob;
	try
		{
		QString strippedArticleUrl;
		//Supported article urls are:
		// A/Url  (Expected by zimlib find(Url) )
		// /A/Url  (Appearanlty used by
		// Url  (Without namespace /A/.), assume it is article. (Either relative or other namespace)
		if (articleUrl.startsWith(QLatin1String("/A/"))) {			
			strippedArticleUrl=articleUrl.remove(0, 3); //Remove /A/
			qDebug() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" starts with /A/./A/ refers to article name space."; 
			
		} else if (articleUrl.startsWith(QLatin1String("A/"))) {
			//TODO remove this when correct behavior clarified.
			strippedArticleUrl=articleUrl.remove(0, 2); //Remove /A
			qWarning() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" starts with A/. Assume A/ refers to article name space. ";
		} else {
			strippedArticleUrl=articleUrl; 
			qDebug() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" does not start with A/ or /A/. Assume it is a relative URL to A/";			
		}			
			
		std::string articleUrlStdStr = std::string(strippedArticleUrl.toUtf8());
		std::string articleUrlDecodedStdStr = zim::urldecode(articleUrlStdStr);
		qDebug() << "Open article by URL.\n QString: " << articleUrl
				<< "QString article namespace stripped:" << strippedArticleUrl
				<< "\n std:string: " << fromUTF8EncodedStdString(articleUrlStdStr)
				<< "\n decoded: " << fromUTF8EncodedStdString(
				articleUrlDecodedStdStr);
		
		zim::File::const_iterator it = zimFile->find('A', articleUrlDecodedStdStr);
		//TODO: Actually not really clean, because if URL not found just closest match displayed.
		if (it == zimFile->end())			
			throw std::runtime_error("article not found");
		if (it->isRedirect())
			{
			//Redirect stores decoded URLs. (as in index) 
			articleUrlDecodedStdStr = it->getRedirectArticle().getUrl();
			qDebug() << "Is redirect to url " << fromUTF8EncodedStdString(articleUrlDecodedStdStr);
			zim::File::const_iterator it1 = zimFile->find('A',
					articleUrlDecodedStdStr);
			blob = it1->getData();
			}
		else
			{
			blob = it->getData();
			}
                qDebug() << " Article (URL: "<< articleUrl << ", Size: "<<blob.size()<<") loaded from zim file";
                articleText = QString::fromUtf8(blob.data(), blob.size());
		}
	catch (const std::exception& e)
		{
		return QString::fromStdString(e.what());
		}

	return articleText;
	}

QString WikiOnBoard::getArticleTextByTitle(QString articleTitle)
	{
	QString articleText = QLatin1String("ERROR");
	zim::Blob blob;
	try
		{
		std::string articleNameStdStr = std::string(articleTitle.toUtf8());

		zim::File::const_iterator it = zimFile->findByTitle('A',
				articleNameStdStr);
		if (it == zimFile->end())
			throw std::runtime_error("article not found");
		if (it->isRedirect())
			{
			articleNameStdStr = it->getRedirectArticle().getTitle();
			zim::File::const_iterator it1 = zimFile->find('A',
					articleNameStdStr);
			blob = it1->getData();
			}
		else
			{
			blob = it->getData();
			}
		articleText = QString::fromUtf8(blob.data(), blob.size());
		}
	catch (const std::exception& e)
		{
		return QString::fromStdString(e.what());
		}

	return articleText;
	}

void WikiOnBoard::populateArticleList() {	
	populateArticleList(ui.articleName->text(), 0, false);
	ui.articleListWidget->setCurrentRow(0); //Select first found item
}

void WikiOnBoard::populateArticleList(QString articleName, int ignoreFirstN,
		bool direction_up, bool noDelete)
	{
	qDebug() << "in populateArticleList. articleName:  " << articleName
			<< ". ignoreFirstN: " << ignoreFirstN << ". direction_up:"
			<< direction_up << ".noDelete: " << noDelete; 
	if (zimFile != NULL)
		{
		try
			{ 
			//Zim index is UTF8 encoded. Therefore use utf8 functions to access
			// it.
			std::string articleNameStdStr = std::string(articleName.toUtf8());
			//Find article, if not an exact match,  Iterator may point to end, or to 
			// element of other namespace. (like image (I) or metadata (M))		
			//FIXME
			zim::File::const_iterator it = zimFile->findByTitle('A',
					articleNameStdStr);
			
			if (!direction_up) {
				// If populating in reverse direction, don´t clear items now
				// but instead each time a new item is added. This avoids
				// that cannot be fully filled if the beginning of the zim file
				// is reached. 
				if (noDelete==false) {
					ui.articleListWidget->clear();
				}
			}
			int i = 0;
			int insertedItemsCount = 0;
			while (true)
				{

				QString articleTitle = fromUTF8EncodedStdString(it->getTitle());
				QUrl articleUrl(fromUTF8EncodedStdString(it->getUrl()));
				QString articleIdx = QString::number(it->getIndex());
				if (it->getNamespace() != 'A')
					{
					qDebug()
							<< " Next index entry is not in article namespace. Stop adding titles. \n\tArticle Title: ["
							<< articleIdx << "] " << articleTitle
							<< "\n\tArticle Namespace: " << it->getNamespace();
					break;
					}
				QListWidgetItem* articleItem = new QListWidgetItem();
				articleItem->setText(articleTitle);
				articleItem->setData(ArticleTitleRole, articleTitle);
				articleItem->setData(ArticleIndexRole, articleIdx);
				articleItem->setData(ArticleUrlRole, articleUrl);
				ui.articleListWidget->addItem(articleItem);
																
				if (direction_up)
					{
					if (i >= ignoreFirstN)
						{
						ui.articleListWidget->insertItem(0, articleItem);
						insertedItemsCount++;
						if (noDelete==false) {
							QListWidgetItem *lastItem = ui.articleListWidget->takeItem(ui.articleListWidget->count() - 1);
							delete lastItem;
						}
						}
					//Note: zimFile->begin() actually does not necessarily point 
					// to same article as it if the are equal. (Because it is in title
					// order while zimFile->begin() uses url order. However, it is fine to detect
					// that it is entry 0, because only the index is compared.
					// there is also beginByTitle(), but end() has the same behavior,
					// and there is not title order equivalent, therefore for both 
					// the url order is used. 
					if (it == zimFile->begin())
						break;
					--it;
					}
				else
					{
					if (i >= ignoreFirstN)
						{
						ui.articleListWidget->addItem(articleItem);
						insertedItemsCount++;
						}
					if (it == zimFile->end())
										break;							
					++it;
					}
				i++;
				if (hasTouchScreen)
					{
					if (insertedItemsCount >= 100)
						{
						break;
						}
					}
				else if (insertedItemsCount > 0)
					{
					//Non-Touch: Only fill visible area, no scrolling
					//TODO: Consider to enable for non-touch
					//Calculate height of all inserted items, and stop
					//insertion when visible area of list is full.
					int itemHeight = ui.articleListWidget->visualItemRect(
							ui.articleListWidget->item(0)).height();
					int
							articleListWidgetHeight =
									ui.articleListWidget->maximumViewportSize().height();
					QListWidgetItem *bottomItem = ui.articleListWidget->item(
							insertedItemsCount - 1);
					int
							bottomItemBottom =
									ui.articleListWidget->visualItemRect(
											bottomItem).bottom();
					if ((bottomItemBottom + itemHeight)
							>= articleListWidgetHeight)
						{
						break;
						}
					}
				}				
			} 

		catch (const std::exception& e)
			{
			ui.articleListWidget->addItem(QLatin1String("Error occured"));
			}

		}

	}

void WikiOnBoard::articleListSelectPreviousEntry()
	{
	//Actually forwarding key basically worked,
	// as well, but crashing when trying to select item outside of list. (pretty strange)
	//Anyway, for in future TODO planned to reload list when moving out of list.
	if (ui.articleListWidget->count() > 0)
		{
		if (ui.articleListWidget->currentRow() == 0)
			{

			QListWidgetItem *item = ui.articleListWidget->currentItem();
			if (item->data(ArticleIndexRole).toInt() > 0)
				{
				populateArticleList(item->data(ArticleTitleRole).toString(), 1,
						true);
				ui.articleListWidget->setCurrentRow(
						ui.articleListWidget->count() - 1);
				}
			}
		else
			{
			ui.articleListWidget->setCurrentRow(
					ui.articleListWidget->currentRow() - 1);
			}
		}
	}

void WikiOnBoard::articleListSelectNextEntry()
	{
	//Actually forwarding key basically worked,
	// as well, but crashing when trying to select item outside of list. (pretty strange)
	//Anyway, for in future TODO planned to reload list when moving out of list.
	if (ui.articleListWidget->count() > 0)
		{
		if (ui.articleListWidget->currentRow() == ui.articleListWidget->count()
				- 1)
			{
			//TODO check outof bounds
			QListWidgetItem *item = ui.articleListWidget->currentItem();
			populateArticleList(item->data(ArticleTitleRole).toString(), 1,
					false);
			ui.articleListWidget->setCurrentRow(0);
			}
		else
			{
			ui.articleListWidget->setCurrentRow(
					ui.articleListWidget->currentRow() + 1);
			}
		}
	}
void WikiOnBoard::articleListOpenArticle(QListWidgetItem * item)
	{
	openArticleAction->trigger();
	}

void WikiOnBoard::articleListOpenArticle()
	{
	QListWidgetItem *item = ui.articleListWidget->currentItem();
	if (item != NULL)
		{		
		ui.textBrowser->setSource(item->data(ArticleUrlRole).toUrl());
		switchToArticlePage();
		}
	}

void WikiOnBoard::openArticleByUrl(QUrl url)
	{
	QString path = url.path();
	//Only read article, if not same as currently
	//viewed article (thus don´t reload for article internal links)
	//TODO: this does not work as appearantly before calling changedSource
	// content is deleted. Therefore for now just reload in any case url.
	// Optimize (by handling in anchorClicked, but check what happens
	//	to history then)
	//if (!path.isEmpty() && (currentlyViewedUrl.path()!=url.path())) {	
	ui.articleName->setText(path);
	
        //QElapsedTimer timer;
        //timer.start();
        QString articleText = getArticleTextByUrl(path);
        //qDebug() << "Reading article " <<path <<" from zim file took" << timer.elapsed() << " milliseconds";
        //timer.start();

        ui.textBrowser->setHtml(articleText);
        //qDebug() << "Loading article into textview (setHtml()) took" << timer.restart() << " milliseconds";
	if (url.hasFragment())
		{
		//Either a link within current file (if path was empty), or to   newly opened file
		QString fragment = url.fragment();
		ui.textBrowser->scrollToAnchor(fragment);
		//Now text visible, but cursor not moved.
		//=> Move cursor to current visisble location. 
		// TODO: no better way to achieve this. Furthermore, actually
		//	cursor reason for problem or something else?)
		moveTextBrowserTextCursorToVisibleArea();
		}
	else
		{
		QTextCursor cursor = ui.textBrowser->textCursor();

		//Move cursor to start of file
		cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
		ui.textBrowser->setTextCursor(cursor);
		if (hasTouchScreen == false)
			{
			//FIXME: This is a really ugly hack for the nextprevious link problem
			// described in keyEventHandler. Note that for links with anchor this 
			// does not work
			// On touchscreen devices workaround is not performed.
			QKeyEvent *remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
					Qt::Key_Up, Qt::NoModifier, QString(), false, 1);
			QApplication::sendEvent(ui.textBrowser, remappedKeyEvent);
			remappedKeyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down,
					Qt::NoModifier, QString(), false, 1);
			QApplication::sendEvent(ui.textBrowser, remappedKeyEvent);

			}
		}
	/// ui.stackedWidget->setCurrentWidget(ui.articlePage);
        //qDebug() << "Loading article into textview (gotoAnchor/moveposition) took" << timer.restart() << " milliseconds";

	}

bool WikiOnBoard::openExternalLink(QUrl url)
	{
	
	QMessageBox msgBox;
	msgBox.setText(tr("Open link in browser"));
	QString bugText = QString(tr("[TRANLATOR]Explain that may not work if browser running.", "only displayed if self_signed or QT<4.7.0"));
#if defined(Q_OS_SYMBIAN)
	#if __IS_SELFSIGNED__==0
		QString qtVersion = QLatin1String(qVersion());
		// A little dirty, but there is not earlier version than 4.6 for symbian.
		if (!qtVersion.startsWith(QLatin1String("4.6"))) {
			bugText = QLatin1String("");
		}		
	#endif
#endif
#if !defined(Q_OS_SYMBIAN)
	bugText = QLatin1String("");
#endif
	QString
			informativeText =
					QString(
							tr(
									"[TRANSLATOR] Explain that link %1 clicked in article is not contained in ebook and needs to be opened in webrowser. Ask if ok.%2")).arg(
							url.toString(), bugText);
	msgBox.setInformativeText(informativeText);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Ok);
#if defined(Q_OS_SYMBIAN)
	QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
#endif
	//Enable virtual mouse cursor on non-touch devices, as
	// else no scrolling possible. (TODO: change this so that
	// scrolling works with cursor keys. Unclear why not working out of the box)
	int ret = msgBox.exec();
#if defined(Q_OS_SYMBIAN)
	QApplication::setNavigationMode(Qt::NavigationModeNone);
#endif
	switch (ret)
		{
		case QMessageBox::Ok:
			QDesktopServices::openUrl(url);
			return true;
			break;
		default:
			return false;
			break;
		}

	//External link, open browser
	QDesktopServices::openUrl(url);
	}

void WikiOnBoard::on_textBrowser_anchorClicked(QUrl url)
	{
	if (!QString::compare(url.scheme(), QLatin1String("http"), Qt::CaseInsensitive))
		{
			openExternalLink(url);
		}
	else
		{	
			ui.textBrowser->setSource(url);
		}
	}

void WikiOnBoard::on_textBrowser_sourceChanged(QUrl url)
	{
	showWaitCursor();
	openArticleByUrl(url);
	hideWaitCursor();
	currentlyViewedUrl = url;
	}

void WikiOnBoard::backArticleHistoryOrIndexPage()
	{
	if (ui.textBrowser->isBackwardAvailable())
		{
		ui.textBrowser->backward();
		}
	else
		{
		switchToIndexPage();
		}
	}

void WikiOnBoard::openZimFileDialog()
	{
	QString path;
	if (zimFile == NULL)
		{
		path = QDir::homePath();
		}
	else
		{
	//TODO: Default is latin1 encoding. Correct?
		path = QString::fromStdString(zimFile->getFilename());
		}
        #if defined(Q_OS_SYMBIAN)
            QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
        #endif
	//Enable virtual mouse cursor on non-touch devices, as 
	// else file dialog not useable
            
    //Extension is .zim for single file zim files,
    // or .zima for splitted zim files. (.zima is extension
    // of first file)
	QString file = QFileDialog::getOpenFileName(this,
			tr("Choose eBook in zim format to open"), path,
			tr("eBooks (*.zim *.zima)"));
        #if defined(Q_OS_SYMBIAN)
            QApplication::setNavigationMode(Qt::NavigationModeNone);
        #endif
        if (!file.isNull())
		{
		openZimFile(file);
		QSettings settings;
		// Store this file to settings, and automatically open next time app is started
		settings.beginGroup(QLatin1String("ZimFile"));
		settings.setValue(QLatin1String("lastZimFile"), file);
		settings.endGroup();
		ui.textBrowser->clearHistory();
		switchToIndexPage(); //In case currently viewing an article.
		populateArticleList();
		}

	}

void WikiOnBoard::downloadZimFile()
	{
	QString zimDownloadUrl(tr("http://openzim.org/ZIM_File_Archive","Change link to page with localized zim files if available."));
	QMessageBox msgBox;
	msgBox.setText(tr("Download ZIM file"));
	QString informativeText = QString(tr("Open a webbrowser to download zim files from %1?\n"
								"Note that zim files may be very large and thus it can be"
								"expensive to download one over the mobile network. "
								"You should consider download from a desktop system"
								"and transfer the file later to the memory card of your phone.\n"
								"Furthermore, note that current Symbian phones do not support files which"
								"are larger than 2 GB. You cannot download such files directly on the phone,"
								"but you have to download on a PC and follow the instructions on"
								"to split them.")).arg(zimDownloadUrl,tr("http://wiki.github.com/cip/WikiOnBoard/","Change link to localized webpage if/when available."));
	msgBox.setInformativeText(informativeText);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Ok);
        #if defined(Q_OS_SYMBIAN)
            QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
        #endif
        //Enable virtual mouse cursor on non-touch devices, as
	// else no scrolling possible. (TODO: change this so that
	// scrolling works with cursor keys. Unclear why not working out of the box)
	int ret = msgBox.exec();
        #if defined(Q_OS_SYMBIAN)
            QApplication::setNavigationMode(Qt::NavigationModeNone);
        #endif
        switch (ret)
		{
		case QMessageBox::Ok:
			QDesktopServices::openUrl(zimDownloadUrl);
			break;
		default:
			break;
		}
	}

void WikiOnBoard::gotoHomepage()
	{
	QString homepageUrl(tr("http://wiki.github.com/cip/WikiOnBoard"));
	QMessageBox msgBox;
	msgBox.setText(tr("Goto homepage"));
	msgBox.setInformativeText(
			tr("Open a webbrowser to show WikiOnBoard's homepage.")
					);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
    switch (ret)
    	{
		case QMessageBox::Ok:
			QDesktopServices::openUrl(homepageUrl);
			break;
		default:
			break;
		}
	}

std::pair<bool, QString> WikiOnBoard::getMetaData(QString key) {
	std::string keyStdStr = std::string(key.toUtf8());				
	if (zimFile!=NULL) {
		std::pair<bool, zim::File::const_iterator> r = zimFile->findxByTitle('M', keyStdStr);
		if (r.first) {
			zim::Blob blob;					
			zim::Article a =*r.second;
			blob = a.getData();			
			return std::pair<bool, QString>(true,  QString::fromUtf8(blob.data(), blob.size()));
		} else {			
			return std::pair<bool, QString>(false, QLatin1String(""));
		}			
	} else {
		return std::pair<bool, QString>(false, QLatin1String(""));
	}
}

QString WikiOnBoard::getMetaDataString(QString key) {
	std::pair<bool, QString> metaData = getMetaData(key);  
	return metaData.first ? metaData.second : QString(tr("Not available"));
}

QString WikiOnBoard::byteArray2HexQString(const QByteArray & byteArray)
{
    QString hexString;
    QTextStream textStream(&hexString);
    textStream << QLatin1String("0x") 
             << hex << qSetFieldWidth(2) << qSetPadChar(QLatin1Char('0')) << left;
    for (int i = 0; i < byteArray.size(); i++)
    {
        textStream << static_cast<unsigned char>(byteArray.at(i));
    }
    return hexString;
}

void WikiOnBoard::aboutCurrentZimFile()
	{
	QMessageBox msgBox;
		
	msgBox.setText(tr("About Current Zimfile"));
	QString informativeText;
	if (zimFile==NULL) {
		informativeText = QString(tr("No zim file is currently opened"));	
	} else {
	
	QByteArray uuidBA =QByteArray(zimFile->getFileheader().getUuid().data, zimFile->getFileheader().getUuid().size());
	informativeText = QString(tr(""
			 "Current Zim File: %1\n"
			 "Articles : %2, Images: %3, Categories: %4\n"
			 )).arg(
					 QString::fromStdString(zimFile->getFilename()),
					 QString::number(zimFile->getNamespaceCount('A')), //Including redirects
					 QString::number(zimFile->getNamespaceCount('I')),
					 QString::number(zimFile->getNamespaceCount('U'))
			 );
	informativeText.append(
			 QString(tr(""
			 "Title: %1\n"
			 "Creator: %2\n"
			 "Date: %3\n"
			 "Source: %4\n"
			 "Description: %5\n"					 			 
			 "Language: %6\n" 
			 "Relation: %7\n")).arg(
					 getMetaDataString(QLatin1String("Title")),
					 getMetaDataString(QLatin1String("Creator")),
					 getMetaDataString(QLatin1String("Date")),
					 getMetaDataString(QLatin1String("Source")),
					 getMetaDataString(QLatin1String("Description")),
					 getMetaDataString(QLatin1String("Language")),
					 getMetaDataString(QLatin1String("Relation"))
			  )			  
	);	
	informativeText.append(QString(tr(""
			"UUID: %1\n"			
			).arg(
					byteArray2HexQString(uuidBA)
			)));				 
	 //TODO: check hash? (Should be separate function)
 	}	
	msgBox.setInformativeText(informativeText);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);
	#if defined(Q_OS_SYMBIAN)
		QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
	#endif
	//Enable virtual mouse cursor on non-touch devices, as
	// else no scrolling possible. (TODO: change this so that
	// scrolling works with cursor keys. Unclear why not working out of the box)
	int ret = msgBox.exec();
	#if defined(Q_OS_SYMBIAN)
		QApplication::setNavigationMode(Qt::NavigationModeNone);
	#endif
  }


void WikiOnBoard::about()
	{
	QMessageBox msgBox;
	msgBox.setText(tr("About"));
	QString text = QString (tr(""
			"WikiOnBoard %1\n"
			"Author: %2\n"
			"Uses zimlib (openzim.org) and liblzma.\n"
			"Build date: %3\n")).arg(
					QString::fromLocal8Bit(__APPVERSIONSTRING__),
					tr("Christian Puehringer"), 
					QString::fromLocal8Bit(__DATE__)
	);
	//TODO: Why are all other msgBox basically fullscreen, but this one is
	//not even large enough to display complete text without scrolling?
	msgBox.setInformativeText(text);
	msgBox.setStandardButtons(QMessageBox::Ok);
	msgBox.setDefaultButton(QMessageBox::Ok);    
	#if defined(Q_OS_SYMBIAN)
		QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
	#endif
	//Enable virtual mouse cursor on non-touch devices, as
	// else no scrolling possible. (TODO: change this so that
	// scrolling works with cursor keys. Unclear why not working out of the box)
	int ret = msgBox.exec();
	#if defined(Q_OS_SYMBIAN)
		QApplication::setNavigationMode(Qt::NavigationModeNone);
	#endif
    }

//Remove all actions from menu, required for switching
// (Appearantly on stacked widget items not symbian style menu
//	can be created)
void WikiOnBoard::clearMenu()
	{
	QList<QAction *> al = menuBar()->actions();
	while (!al.isEmpty())
		{
		ui.menuBar->removeAction(al.takeFirst());
		}
	}

void WikiOnBoard::switchToArticlePage()
	{
	clearMenu();
	menuBar()->addAction(switchToIndexPageAction);
	menuBar()->addAction(openZimFileDialogAction);
	menuBar()->addAction(downloadZimFileAction);
	
	optionsMenu = new QMenu(tr("Options", "Option menu"));
	optionsMenu->addAction(zoomInAction);
	optionsMenu->addAction(zoomOutAction);
	optionsMenu->addAction(toggleFullScreenAction);
	helpMenu = new QMenu(tr("Help", "Help menu"));
	helpMenu->addAction(gotoHomepageAction);
	helpMenu->addAction(aboutCurrentZimFileAction);
	helpMenu->addAction(aboutAction);
	helpMenu->addAction(aboutQtAction);

	menuBar()->addMenu(optionsMenu);
	menuBar()->addMenu(helpMenu);
	menuBar()->addAction(exitAction);

	backArticleHistoryAction->setSoftKeyRole(QAction::NegativeSoftKey);
	clearSearchAction->setSoftKeyRole(QAction::NoSoftKey);
	
	ui.stackedWidget->setCurrentWidget(ui.articlePage);

	ui.textBrowser->setFocus();
	}
void WikiOnBoard::switchToIndexPage()
	{
	clearMenu();
	//	menuBar()->addAction(searchArticleAction);
	menuBar()->addAction(openArticleAction);
	menuBar()->addAction(openZimFileDialogAction);
	menuBar()->addAction(downloadZimFileAction);
	optionsMenu = new QMenu(tr("Options", "Option menu"));
	optionsMenu->addAction(toggleFullScreenAction);
	
	helpMenu = new QMenu(tr("Help", "Help menu"));
	helpMenu->addAction(gotoHomepageAction);
	helpMenu->addAction(aboutCurrentZimFileAction);
	helpMenu->addAction(aboutAction);
	helpMenu->addAction(aboutQtAction);
			
	menuBar()->addMenu(optionsMenu);
	menuBar()->addMenu(helpMenu);
	menuBar()->addAction(exitAction);

	backArticleHistoryAction->setSoftKeyRole(QAction::NoSoftKey);
	clearSearchAction->setSoftKeyRole(QAction::NegativeSoftKey);

	ui.stackedWidget->setCurrentWidget(ui.indexPage);
	ui.articleName->setFocus();
	populateArticleList();
	}

void WikiOnBoard::searchArticle()
	{
	populateArticleList();	
	}

void WikiOnBoard::moveTextBrowserTextCursorToVisibleArea()
	{
	int position = ui.textBrowser->cursorForPosition(QPoint(0, 0)).position();
	QTextCursor cursor = ui.textBrowser->textCursor();
	cursor.setPosition(position, QTextCursor::MoveAnchor);
	ui.textBrowser->setTextCursor(cursor);
	}

void WikiOnBoard::keyPressEvent(QKeyEvent* event)
	{
	QEvent *remappedKeyEvent;
	qDebug() << event->nativeVirtualKey();
	//TODO: actually not really understood why working:
	// E.g. not working to redefine left/right in articlepage (which is stupid, as default
	//		behavior is to supidly move the cursors.
	//	while woring for index page (up/down affects non focused widget)
	if (ui.stackedWidget->currentWidget() == ui.articlePage)
		{
		switch (event->key())
			{
			//Goto next link or scroll down if not link in page or Goto previous link or scrollup if no link in page. 
			//Done by remapping key to Key_Up key. This only works if QT_KEYPAD_NAVIGATION is defined
			// (Uses QTextBrowserPrivate::keypadMove(bool next)
			//Note that using tab, which works on all platforms, is not a good alterantive, as it always jumps to the next
			//link, while keyPadMove jumps to the next visible link.remappedKeyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier,false, 1);		
			//Can be pretty slow, in particular in lists, etc..., TODO consider reimplementing more efficiently. (e.g.
			// 	store list of links + locations while idle.
			// FIXME sometimes not working, thus scrolling although links are available. 
			// Problem is appearantly at least in "changed article" that prevFocus (a textcursor)
			// cannot be moved to begiinning of document, as document is always empty, because
			// this is done in setsource before the text is set. 
			// However, actually after move to start prevCursour should be at zero anyway. (It is,
			// but on  next event (e.g. key but also source changed) it has some other value)
			// perhaps does setHtml somehow change it.  (at least in sourcechanged it is appearantly clearing it)
	
			//The following scroll up/down are done using the scrollbar and not by remapping keys to allow better control
			// Would be basically easier and more efficient to just change the pageStep,
			// 	sideffcts regarding screen orientation and resolution changes could occur.
			//Volume key mapping appearantly does not work,
			// See http://bugreports.qt.nokia.com/browse/QTBUG-4415
			case Qt::Key_VolumeDown:
			case Qt::Key_2: //Scroll one page up (-one line as else one line may never be visible entirely)
				ui.textBrowser->verticalScrollBar()->triggerAction(
						QAbstractSlider::SliderPageStepSub);
				if (ui.textBrowser->verticalScrollBar()->value()!=ui.textBrowser->verticalScrollBar()->minimum()) {
					//Only scroll down again a little, if not at beginning of article
					ui.textBrowser->verticalScrollBar()->triggerAction(
										QAbstractSlider::SliderSingleStepAdd);								
				}
				break;
			case Qt::Key_VolumeUp:							
			case Qt::Key_8: //Scroll one page down (-one line as else one line may never be visible entirely)
				ui.textBrowser->verticalScrollBar()->triggerAction(
						QAbstractSlider::SliderPageStepAdd);
				if (ui.textBrowser->verticalScrollBar()->value()!=ui.textBrowser->verticalScrollBar()->maximum()) {
					//Only scroll back again a little, if not at end of article
					ui.textBrowser->verticalScrollBar()->triggerAction(
						QAbstractSlider::SliderSingleStepSub);
				}
				break;
			default:
				QMainWindow::keyPressEvent(event);
			}
		}
	else if (ui.stackedWidget->currentWidget() == ui.indexPage)
		{		
		//Behavior for keypad on index page:
		//   text entry and curso left/right to articleName
		//   up/down: select article in article list
		//   
		switch (event->key())

			{
			case Qt::Key_Up:
				/*
				 remappedKeyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up,
				 Qt::NoModifier, false, 1);
				 QApplication::sendEvent(ui.articleListWidget, remappedKeyEvent);
				 */
				articleListSelectPreviousEntry();
				break;
			case Qt::Key_Down:
				/*remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
				 Qt::Key_Down, Qt::NoModifier, false, 1);
				 QApplication::sendEvent(ui.articleListWidget, remappedKeyEvent);*/
				articleListSelectNextEntry();
				break;
			case Qt::Key_Left:
				remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Left, Qt::NoModifier, QString(), false, 1);
				QApplication::sendEvent(ui.articleName, remappedKeyEvent);
				break;
			case Qt::Key_Right:
				remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Right, Qt::NoModifier, QString(), false, 1);
				QApplication::sendEvent(ui.articleName, remappedKeyEvent);
				break;
			case Qt::Key_Select:
				//This appearantly is the select key of the mobile phone. (it is not return or select)
				//Don't just remap key for better control
				openArticleAction->trigger();
				break;
			case Qt::Key_Enter:
				//For emulator and windows/linux
				openArticleAction->trigger();
				break;

			default:
				//QApplication::sendEvent(ui.articleName, event);
				QMainWindow::keyPressEvent(event);
			}
		}

	}

void WikiOnBoard::resizeEvent(QResizeEvent * event)
	{
	
	if (ui.stackedWidget->currentWidget() == ui.indexPage)
		{
		if (ui.articleListWidget->count() > 0)
			{
			//Current item (if none first) is new first item of list. 
			// TODO: Keeping offset would be nicer, but more complex 
			//      in case new size is smaller than offset.
			int itemIndex = ui.articleListWidget->currentRow();
			if (itemIndex < 0) {
				itemIndex = 0;
				}
			QListWidgetItem *item = ui.articleListWidget->item(itemIndex);
			if (item->data(ArticleIndexRole).toInt() > 0)
				{
				populateArticleList(item->data(ArticleTitleRole).toString(), 0,
						false);
				ui.articleListWidget->setCurrentRow(0);
				}
			}

		}
	}

void WikiOnBoard::toggleFullScreen()
	{
	if (isFullScreen())
		{
		showMaximized();
		}
	else
		{
		showFullScreen();

		//Workaround for softkeys in fullscreen mode.see main.cpp for details
#if defined(Q_OS_SYMBIAN)
		CEikButtonGroupContainer* bgc = (CEikButtonGroupContainer*) m_bgc;
		if (bgc)
			{
			bgc->MakeVisible(ETrue);
			bgc->DrawNow();
			}
#endif
		}
	QSettings settings;
	settings.beginGroup(QLatin1String("UISettings"));
	if ((!settings.contains(QLatin1String("fullScreen"))) || (settings.value(QLatin1String("fullScreen"),
			false).toBool() != isFullScreen()))
		{
		settings.setValue(QLatin1String("fullScreen"), isFullScreen());
		}
	settings.endGroup();
	}

void WikiOnBoard::zoom(int zoomDelta)
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
		ui.textBrowser->zoomOut(abs(zoomDelta));
		}
	else
		{
		ui.textBrowser->zoomIn(zoomDelta);
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

void WikiOnBoard::zoomOut()
	{
	zoom(-1);
	}

void WikiOnBoard::zoomIn()
	{
	zoom(1);
	}

void WikiOnBoard::showWaitCursor() 
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
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
	}

void WikiOnBoard::hideWaitCursor()
	{	
	QApplication::restoreOverrideCursor();
        #if defined(Q_OS_SYMBIAN)
            QApplication::setNavigationMode(Qt::NavigationModeNone);
        #endif
}

//TODO Consider merging with regular populate Article List 
bool WikiOnBoard::approachingEndOfList(bool up)
	{
	qDebug() << "WikiOnBoard::approachingEndOfList";
	if (zimFile != NULL)
		{
		try
			{
			if (up)
				{
				//populateArticleList(ui.articleListWidget->item(0)->data(ArticleTitleRole).toString(),0,true, true);
				qDebug()
						<< "Approaching end of article list while scrolling up";
				QString
						titleFirstArticleInCurrentList =
								ui.articleListWidget->item(0)->data(
										ArticleTitleRole).toString();
				QString
										idxFirstArticleInCurrentList =
												ui.articleListWidget->item(0)->data(
														ArticleIndexRole).toString();
								
				//Zim index is UTF8 encoded. Therefore use utf8 functions to access
				// it.
				std::string articleTitleStdStr = std::string(
						titleFirstArticleInCurrentList.toUtf8());
				qDebug() << " Title of first article in current list is: ["<< idxFirstArticleInCurrentList  << "] "
						<< fromUTF8EncodedStdString(articleTitleStdStr);

				std::pair<bool, zim::File::const_iterator> r =
						zimFile->findxByTitle('A', articleTitleStdStr);
				if (!r.first)
					{
					qWarning()
							<< " No exact match found. (Only possible if bug in wikionboard)";
					}
				zim::File::const_iterator it = r.second;
				if (it == zimFile->beginByTitle())
					{
					qDebug()
							<< " Current entry is first entry in index => Do nothing";
					return false;
					}

				int insertedItemsCount = 0;
				//TODO: use constants (for this and for delete limit)
				while (insertedItemsCount < 100)
					{
					--it;
					QString articleTitle = fromUTF8EncodedStdString(
							it->getTitle());
					QUrl articleUrl(fromUTF8EncodedStdString(it->getUrl()));
					QString articleIdx = QString::number(it->getIndex());
					if (it->getNamespace() != 'A')
						{
						qDebug()
								<< " Previous index entry is not in article namespace. Stop adding titles. \n\tArticle Title: ["<< articleIdx  << "] "
								<< articleTitle << "\n\tArticle Namespace: "
								<< it->getNamespace();
						break;
						}
					QListWidgetItem* articleItem = new QListWidgetItem();
					articleItem->setText(articleTitle);
					articleItem->setData(ArticleTitleRole, articleTitle);
					articleItem->setData(ArticleIndexRole, articleIdx);
					articleItem->setData(ArticleUrlRole, articleUrl);
					ui.articleListWidget->insertItem(0, articleItem);
					insertedItemsCount++;
					//Remove last item to avoid eating up to much memory. 
					//(But ensure that that there enough "old" items left
					// to fill the article list. (first added new item 
					// should be on top of list at the end)
					if (ui.articleListWidget->count()>120) {
						QListWidgetItem *lastItem = ui.articleListWidget->takeItem(
							ui.articleListWidget->count() - 1);
						delete lastItem;
					}
					//order is different					
					if (it == zimFile->beginByTitle())
						{
						qDebug()
								<< "Beginning of title index reached. Stop adding titles. Last added title :["<< articleIdx  << "] " 
								<< articleTitle;
						break;
						}
					} //End while
				if (insertedItemsCount>0) {
					QListWidgetItem *firstNewItem = ui.articleListWidget->item(insertedItemsCount-1);
					QString	titleFirstNewItem =
						firstNewItem->data(
								ArticleTitleRole).toString();
					QString	idxFirstNewItem=
						firstNewItem->data(
										ArticleIndexRole).toString();
											
					qDebug() << insertedItemsCount <<" items inserted in beginning of list. Scroll so that firstly newly added article " << titleFirstNewItem << " ["<< idxFirstNewItem <<"] is at top of list. ";
					ui.articleListWidget->scrollToItem(firstNewItem,QAbstractItemView::PositionAtTop);
					return true;
				}	else {
					qDebug() << "No items inserted";
					return false;
				}
				}
			else
				{// end  (if up()). => up=false
				qDebug()
						<< "Approaching end of article list while scrolling down";
				QString titleLastArticleInCurrentList =
						ui.articleListWidget->item(
								ui.articleListWidget->count() - 1)->data(
								ArticleTitleRole).toString();
				QString  idxLastArticleInCurrentList =
										ui.articleListWidget->item(
												ui.articleListWidget->count() - 1)->data(
												ArticleIndexRole).toString();
								
				//Zim index is UTF8 encoded. Therefore use utf8 functions to access
				// it.
				std::string articleTitleStdStr = std::string(
						titleLastArticleInCurrentList.toUtf8());
				qDebug() << " Title of last article in current list is: ["<< idxLastArticleInCurrentList  << "] "
						<< fromUTF8EncodedStdString(articleTitleStdStr);

				std::pair<bool, zim::File::const_iterator> r =
						zimFile->findxByTitle('A', articleTitleStdStr);
				if (!r.first)
					{
					qWarning()
							<< " No exact match found. (Only possible if bug in wikionboard)";
					}
				zim::File::const_iterator it = r.second;
				if (it == zimFile->end())
					{
					qDebug()
							<< " Current entry is last entry in index => Do nothing";
					return false;
					}
				int insertedItemsCount = 0;
				while (insertedItemsCount < 100)
					{
					++it;
					QString articleTitle = fromUTF8EncodedStdString(
							it->getTitle());
					QUrl articleUrl(fromUTF8EncodedStdString(it->getUrl()));
					QString articleIdx = QString::number(it->getIndex());
					if (it->getNamespace() != 'A')
						{
						qDebug()
								<< " Next index entry is not in article namespace. Stop adding titles. \n\tArticle Title: ["<< articleIdx  << "] "
								<< articleTitle << "\n\tArticle Namespace: "
								<< it->getNamespace();
						break;
						}
					QListWidgetItem* articleItem = new QListWidgetItem();
					articleItem->setText(articleTitle);
					articleItem->setData(ArticleTitleRole, articleTitle);
					articleItem->setData(ArticleIndexRole, articleIdx);
					articleItem->setData(ArticleUrlRole, articleUrl);
					ui.articleListWidget->addItem(articleItem);
					//Remove first item to avoid eating up to much memory. 
					//TODO: increase overlap. (User scrolls one direction than other else leads directly to reload)
					if (ui.articleListWidget->count()>120) {
						QListWidgetItem *firstItem =
							ui.articleListWidget->takeItem(0);
						delete firstItem;
					}
					insertedItemsCount++;
					if (it == zimFile->end())
						{
						qDebug()
								<< "End of title index reached. Stop adding titles. Last added title: ["<< articleIdx  << "] "
								<< articleTitle;
						break;
						}
					} //End while
					int indexFirstNewItem = ui.articleListWidget->count()-insertedItemsCount;
					if ((indexFirstNewItem >= 0) && (indexFirstNewItem
						< ui.articleListWidget->count()))
					{
						QListWidgetItem *firstNewItem = ui.articleListWidget->item(
							indexFirstNewItem);
						QString titleFirstNewItem = firstNewItem->data(
							ArticleTitleRole).toString();
						QString idxFirstNewItem = firstNewItem->data(
							ArticleIndexRole).toString();

						qDebug() << insertedItemsCount
							<< " items appended to end of list. Scroll so that firstly newly added article "
							<< titleFirstNewItem << " [" << idxFirstNewItem
							<< "] is at bottom of list. ";
						ui.articleListWidget->scrollToItem(firstNewItem,
							QAbstractItemView::PositionAtBottom);
						return true;
					}	else {
						qDebug() << "No items inserted";
						return false;
					}
				} // End else (up=false)
			} //End try
		catch (const std::exception& e)
			{
			ui.articleListWidget->addItem(QLatin1String("Error occured"));
			}
		} //End if (zimFile!=NULL)
		return false;
	}	


