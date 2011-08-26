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

//TODO: Find some replacement for hourglass. (Not working anymore as mouse cursor disabled)
//TODO fix curso in textedit (don't move on left/right). Consider however before the
// the sync. lost during scrolling issue.
#include "wikionboard.h"
#include <QKeyEvent>
#include <QDebug>
#include <QFile>
#include <QProgressBar>
#include <QScrollBar>
#include <QMessageBox>
#include <QStringBuilder>
#include <QTextCodec>
#include <QTextBlock>
#include <QDesktopWidget>
#include <QBuffer>
#include <QImageReader>
#include <QPixmap>
#include <QSize>
//"Official" kinetic scrolling. (Backport from Qt 4.8) 
//	See http://qt.gitorious.org/qt-labs/kineticscroller/commits/solution and
//		http://bugreports.qt.nokia.com/browse/QTBUG-9054?focusedCommentId=130700&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#action_130700
#include <QtScroller>
#include <qtscrollevent>

//For split-screen software keyboard
#ifdef Q_OS_SYMBIAN
    #include <aknedsts.h>
    #include <coeaui.h>
    #include <coemain.h>
    #include <w32std.h>
    #define EAknEditorFlagEnablePartialScreen 0x200000
#endif

#include <QTime>
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
								
					if (lw->viewport() == w)
						{
						qreal delta = lw->visualItemRect(
															lw->item(0)).height();
								
						if (se->scrollState()==QtScrollEvent::ScrollStarted) {
							qDebug() << "New Scrolling activity started. ";
							qDebug() << " Scroller final position: " << QtScroller::scroller(w)->finalPosition();
							qDebug() << "delta: " <<delta;
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
                                                            emit approachingEndOfList(true);
							    return true;
							    //Return true to prevent that default QScroller eventfilter scrolls  
                                                                // to beginning of list and stop scroller. (Nicer would be for sure
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
                                                                //TODO bounce not working anymore.(Probably necessary to not stop (return true) if nothing can be added.


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


    

WikiOnBoard::WikiOnBoard(QWidget *parent) :
    QMainWindow(parent), welcomeUrl(QUrl(QLatin1String("wikionboard://welcome")))

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
        //Used by QSettings
        QCoreApplication::setOrganizationName(QLatin1String("Christian Puehringer"));
        QCoreApplication::setApplicationName(QLatin1String("WikiOnBoard"));

        #ifdef Q_OS_SYMBIAN
        // Save settings files to private application directory, to ensure
        // that uninstaller removes settings. (see also
        //  http://bugreports.qt.nokia.com/browse/QTBUG-16229)
            QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
            QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::applicationDirPath());
            QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, QCoreApplication::applicationDirPath());
            QSettings::setPath(QSettings::IniFormat, QSettings::SystemScope, QCoreApplication::applicationDirPath());
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

	ui.setupUi(this);
        articleViewer = new ArticleViewer(ui.articlePage,this);
        ui.gridLayout_3->addWidget(articleViewer);
        setStatusBar(0); //Remove status bar to increase useable screen size.
	//TODO still not perfect, quite some distance between
	//  widgeht and menu.

	settings.beginGroup(QLatin1String("UISettings"));
	int zoomInit = settings.value(QLatin1String("zoomLevel"), -1).toInt();
        fullScreen = settings.value(QLatin1String("fullScreen"), false).toBool();
	settings.endGroup();
        zoomLevel = 0;
	zoom(zoomInit);
        if (connect(articleViewer,SIGNAL(sourceChanged(QUrl)),this, SLOT(on_articleViewer_sourceChanged(QUrl)))) {
            qDebug() << "Connected sourceChanged";
        } else {
            qWarning()<<"Could not connect sourceChanged";
        }
        if (connect(articleViewer,SIGNAL(anchorClicked(QUrl)),this, SLOT(on_articleViewer_anchorClicked(QUrl)))) {
            qDebug() << "Connected anchorClicked";
        } else {
            qWarning()<<"Could not connect anchorClicked";
        }
        connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(workAreaResized(int)));
        //  LeftMouseButtonGesture used, as use of TouchGesture together
	// with mouse click events (like link clicked) problematic.
        //TODO QtScroller::grabGesture(articleViewer->viewport(), QtScroller::LeftMouseButtonGesture);
					
	// ScrollPerPixel required for kinetic scrolling
	ui.articleListWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);	    
        //TODO QtScroller::grabGesture(ui.articleListWidget->viewport(), QtScroller::LeftMouseButtonGesture);
	ArticleListFilter *articleListFilter = new ArticleListFilter();
	ui.articleListWidget->viewport()->installEventFilter(articleListFilter);
        connect(articleListFilter,SIGNAL(approachingEndOfList(bool)),this, SLOT(addItemsToArticleList(bool)));
        //TODO QtScrollerProperties properties = QtScroller::scroller(articleViewer->viewport())->scrollerProperties();
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
			 
        //TODO QtScroller::scroller(articleViewer->viewport())->setScrollerProperties(properties);
	
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
        //Hide toolbar to increase useable screen space (toolbar may be used for meego).
        ui.toolBar->hide();
#endif
	if (fullScreen)
		{
                showFullScreen();
		}
	else
		{
		showMaximized();
		}
	currentlyViewedUrl = QUrl(QLatin1String(""));
        openZimFileDialogAction = new QAction(tr("Open Zimfile"), this);
	connect(openZimFileDialogAction, SIGNAL(triggered()), this,
			SLOT(openZimFileDialog()));

        showWelcomePageAction = new QAction(tr("show Welcomepage"), this);
        connect(showWelcomePageAction, SIGNAL(triggered()), this,
                        SLOT(switchToWelcomePage()));
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
        emptyAction = new QAction( tr( "", "Empty. Displayed as right soft key if nothing opened"), this );
        addAction( emptyAction );

	toggleFullScreenAction = new QAction(tr("Toggle Fullscreen"), this); //TODO shortcut
	toggleFullScreenAction->setShortcutContext(Qt::ApplicationShortcut); //Or Qt::WindowShortcut?
	connect(toggleFullScreenAction, SIGNAL(triggered()), this,
			SLOT(toggleFullScreen()));
	this->addAction(toggleFullScreenAction);

        toggleImageDisplayAction = new QAction(tr("Show Images"), this);
        toggleImageDisplayAction->setCheckable(true);
        connect(toggleImageDisplayAction, SIGNAL(toggled(bool)), articleViewer,
                        SLOT(toggleImageDisplay(bool)));
        this->addAction(toggleImageDisplayAction);
        settings.beginGroup(QLatin1String("UISettings"));
        toggleImageDisplayAction->setChecked(settings.value(QLatin1String("showImages"), true).toBool());
        settings.endGroup();


	exitAction = new QAction(tr("Exit"), this);
	connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));
	this->addAction(exitAction);

	zoomInAction = new QAction(tr("Zoom in"), this);
	zoomOutAction = new QAction(tr("Zoom out"), this);
	connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
	connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));

        //Sub menus
        QMenu* optionsMenuIndexPage = new QMenu(tr("Options", "Option menu"),this);
        optionsMenuIndexPage->addAction(toggleFullScreenAction);
        optionsMenuIndexPage->addAction(toggleImageDisplayAction);
        QMenu* optionsMenuArticlePage = new QMenu(tr("Options", "Option menu"),this);
        optionsMenuArticlePage->addAction(zoomInAction);
        optionsMenuArticlePage->addAction(zoomOutAction);
        optionsMenuArticlePage->addAction(toggleFullScreenAction);
        optionsMenuArticlePage->addAction(toggleImageDisplayAction);
        QMenu* helpMenu = new QMenu(tr("Help", "Help menu"),this);
        helpMenu->addAction(showWelcomePageAction);
        helpMenu->addAction(gotoHomepageAction);
        helpMenu->addAction(aboutCurrentZimFileAction);
        helpMenu->addAction(aboutAction);
        helpMenu->addAction(aboutQtAction);


        menuIndexPage = new QMenu(this);
        menuIndexPage->addAction(openArticleAction);
        menuIndexPage->addAction(openZimFileDialogAction);
        menuIndexPage->addMenu(optionsMenuIndexPage);
        menuIndexPage->addMenu(helpMenu);
        menuIndexPage->addAction(exitAction);

        menuArticlePage = new QMenu(this);
        menuArticlePage->addAction(switchToIndexPageAction);
        menuArticlePage->addAction(openZimFileDialogAction);
        menuArticlePage->addMenu(optionsMenuArticlePage);
        menuArticlePage->addMenu(helpMenu);
        menuArticlePage->addAction(exitAction);

        menuArticlePageNoFileOpen = new QMenu(this);
        menuArticlePageNoFileOpen->addAction(openZimFileDialogAction);
        menuArticlePageNoFileOpen->addMenu(optionsMenuArticlePage);
        menuArticlePageNoFileOpen->addMenu(helpMenu);
        menuArticlePageNoFileOpen->addAction(exitAction);

        //Used to allow translation of softkey with menu.
        // (Actually should be translated automatically, but
        // appearantly not working, therefore just assign our
        // own (translatable) text)
        positiveSoftKeyActionMenuIndexPage = new QAction(tr("TRANLATOR Indexpage Menu Name"),this);
        positiveSoftKeyActionMenuIndexPage->setMenu(menuIndexPage);
        this->addAction(positiveSoftKeyActionMenuIndexPage);

        positiveSoftKeyActionMenuArticlePage = new QAction(tr("TRANLATOR Articlepage Menu Name"),this);
        positiveSoftKeyActionMenuArticlePage->setMenu(menuArticlePage);
        this->addAction(positiveSoftKeyActionMenuArticlePage);

        positiveSoftKeyActionMenuArticlePageNoFileOpen = new QAction(tr("TRANLATOR Articlepage (no zimfile open) Menu Name"),this);
        positiveSoftKeyActionMenuArticlePageNoFileOpen->setMenu(menuArticlePageNoFileOpen);
        this->addAction(positiveSoftKeyActionMenuArticlePageNoFileOpen);


	// Set context menu policy for widgets to suppress the useless 'Actions' submenu
#ifdef Q_OS_SYMBIAN
	QWidgetList widgets = QApplication::allWidgets();
	QWidget* w = 0;
	foreach( w, widgets )
			{
			w->setContextMenuPolicy(Qt::NoContextMenu);
			}
#endif
        enableSplitScreen();
        if (lastZimFile.isEmpty() ||  !openZimFile(lastZimFile)) {
                //If no zim file loaded yet (e.g. first start)
                // or open failed, show info how to download or open
                //ebooks.
                switchToWelcomePage();
        } else {
            switchToIndexPage();
        }
        }

WikiOnBoard::~WikiOnBoard()
	{
            delete zimFile;
	}

bool WikiOnBoard::openZimFile(QString zimFileName)
	{
        std::string zimfilename;
        try
		{
                //If zim file is split extension of first file is zimaa
                // zim::File however expects zim as extension, if it does
                // not find it, it tries zima
                // => Change extension in filename from zima to zim
		QRegExp rx(QLatin1String("(.*\\.zim)\\D\\D"));
	    rx.setCaseSensitivity(Qt::CaseInsensitive);
	    zimFileName.replace(rx,QLatin1String("\\1"));
            //converts fileName to the local 8-bit encoding determined by the user's locale
            // On symbian it is probably encoded to UTF-8. As zimlib does not use qt open
            // file mechanism, opening files with non latin1 characters may not work on all platforms.
            qDebug() << "encoding filename using QFile::encodeName";
            zimfilename =  std::string(QFile::encodeName(zimFileName));
            zim::File* oldZimFile = zimFile;
                zimFile = new zim::File(zimfilename);
                //If opensuccesful, delete pointer to previously openend zim file.
                // If open fails keep previously opened zim file open.
                delete oldZimFile;
                return true;
                }
	catch (const std::exception& e)
		{
			QMessageBox::StandardButton reply;
                     reply = QMessageBox::critical(this, tr("Error on opening zim file"),
                                                            QString(tr("Error on opening zim file %1.\nError message:%2\n").arg(fromUTF8EncodedStdString(zimfilename),QString::fromStdString(e.what()))),
		                                     QMessageBox::Ok);		 
                     return false;
                }
	}

//Article URL must be percent encoded.
// nameSpace should either be 'A' for Articles or 'I' for images.
zim::File::const_iterator WikiOnBoard::getArticleByUrl(QString articleUrl,QChar nameSpace, bool closestMatchIfNotFound) {
	QString strippedArticleUrl;
	//Supported article urls are:
	// A/Url  (Expected by zimlib find(Url) )
	// /A/Url  (Appearanlty used by
	// Url  (Without namespace /A/.), assume it is article. (Either relative or other namespace)
        if (articleUrl.startsWith(QLatin1String("/")+nameSpace+QLatin1String("/"))) {
		strippedArticleUrl=articleUrl.remove(0, 3); //Remove /A/
                qDebug() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" starts with /"<< nameSpace << "/./"<< nameSpace << "/ refers to article name space.";
		
        } else if (articleUrl.startsWith(nameSpace+QLatin1String("/"))) {
		//TODO remove this when correct behavior clarified.
		strippedArticleUrl=articleUrl.remove(0, 2); //Remove /A
                qWarning() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" starts with "<< nameSpace << "/. Assume "<<nameSpace<<"/ refers to article name space. ";
	} else {
		strippedArticleUrl=articleUrl; 
                qDebug() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" does not start with "<< nameSpace.toLatin1() <<"/ or /"<< nameSpace << "/. Assume it is a relative URL to "<< nameSpace << "/";
	}			
		
	std::string articleUrlStdStr = std::string(strippedArticleUrl.toUtf8());
	std::string articleUrlDecodedStdStr = zim::urldecode(articleUrlStdStr);
	qDebug() << "Open article by URL.\n QString: " << articleUrl
			<< "QString article namespace stripped:" << strippedArticleUrl
			<< "\n std:string: " << fromUTF8EncodedStdString(articleUrlStdStr)
			<< "\n decoded: " << fromUTF8EncodedStdString(
			articleUrlDecodedStdStr);
	

        std::pair<bool, zim::File::const_iterator> r = zimFile->findx(nameSpace.toLatin1(), articleUrlDecodedStdStr);
        if (!r.first) {
            qWarning() << " article not found. URL encoded: "<< strippedArticleUrl << " decoded: "  << fromUTF8EncodedStdString(
                              articleUrlDecodedStdStr) << "\n";
            strippedArticleUrl.replace(QLatin1String("+"),QLatin1String("%2B"));
            articleUrlStdStr = std::string(strippedArticleUrl.toUtf8());
            articleUrlDecodedStdStr = zim::urldecode(articleUrlStdStr);
            qWarning() << " Try whether article for url without replacing + with spaces exists";
            qDebug() << "Original URL: " << articleUrl
                            << "Stripped URL, + replaced by %2B " << strippedArticleUrl
                            << "\n std:string: " << fromUTF8EncodedStdString(articleUrlStdStr)
                            << "\n decoded: " << fromUTF8EncodedStdString(
                            articleUrlDecodedStdStr);

            r = zimFile->findx('A', articleUrlDecodedStdStr);
            if (!r.first) {
                if (!closestMatchIfNotFound) {
                    qWarning() << "Neither exists. closestMatchIfNotFound=false. Return zimFile->end()";
                    return zimFile->end();
                }
                qWarning() << "Neither exists. closestMatchIfNotFound=true => Return closest match. (With + not replaced by spaces)";
            }
        }
        return r.second;
 }

QString WikiOnBoard::getArticleTitleByUrl(QString articleUrl) {
	zim::File::const_iterator it = getArticleByUrl(articleUrl);
	if (it == zimFile->end()) return QString(tr("Error: article not found. (URL: %1 )").arg(articleUrl));			
	return fromUTF8EncodedStdString(it->getTitle());
}


//Note: expects encoded URL (as used in articles). Therefore don't use
// this for decoded URL (as in zim file index)
QString WikiOnBoard::getArticleTextByUrl(QString articleUrl)
	{
	QString articleText = QLatin1String("ERROR");
	zim::Blob blob;
	try
		{
		zim::File::const_iterator it = getArticleByUrl(articleUrl);
		//TODO: Actually not really clean, because if URL not found just closest match displayed.
		if (it == zimFile->end())			
			throw std::runtime_error("article not found");
		if (it->isRedirect())
			{
			//Redirect stores decoded URLs. (as in index) 
			std::string articleUrlDecodedStdStr = it->getRedirectArticle().getUrl();
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


QSize WikiOnBoard::getMaximumDisplaySizeInCurrentArticleForImage(QString imageUrl) {
    //int maxLengthArticleViewer= (articleViewer->size().height()>articleViewer->size().width()?articleViewer->size().height():articleViewer->size().width());
    //FIXME: Probably slow. Probably url not correct
    QSize size;
    for (QTextBlock it = articleViewer->document()->begin(); it != articleViewer->document()->end(); it = it.next()) {
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

//Note: expects encoded URL (as used in articles). Therefore don't use
// this for decoded URL (as in zim file index)
QPixmap WikiOnBoard::getImageByUrl(QString imageUrl)
{
    QTime timer;
    QTime subTimer;

    QPixmap image;
    zim::Blob blob;
    timer.start();
    try
    {
        //For images don't load closest match if url not found.
        subTimer.start();
        zim::File::const_iterator it = getArticleByUrl(imageUrl,QLatin1Char('I'),false);
        if (it == zimFile->end())
            throw std::runtime_error("image not found");
        if (it->isRedirect())
        {
            //Redirect stores decoded URLs. (as in index)
            // TODO: really necessary for images?
            std::string imageUrlDecodedStdStr = it->getRedirectArticle().getUrl();
            qDebug() << "Is redirect to url " << fromUTF8EncodedStdString(imageUrlDecodedStdStr);
            zim::File::const_iterator it1 = zimFile->find('I',
                                                          imageUrlDecodedStdStr);
            blob = it1->getData();
        }
        else
        {
            blob = it->getData();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "Error in load image. Return 1x1 pixel image instead";
        image = QPixmap(1,1);
        image.fill();
        return image;
    }
    qDebug() << " Image (URL: "<< imageUrl << ", Size: "<<blob.size()<<") loaded from zim file";
    qDebug() << "Loading image data" << imageUrl << " from zim file took" << subTimer.restart() << " milliseconds";
    QSize newSize = getMaximumDisplaySizeInCurrentArticleForImage(imageUrl);
    qDebug() << " Searching image size took " << subTimer.restart() << " milliseconds";
    QBuffer *imageBuffer = new QBuffer();
    imageBuffer->setData(blob.data(),blob.size());
    QImageReader *imageReader = new QImageReader(imageBuffer);
    qDebug() << "Original size of image: "<<imageReader->size();
    qDebug() << "Image format: "<<imageReader->format();
    qDebug() << " supports scaling: " << imageReader->supportsOption(QImageIOHandler::ScaledSize);
    if (newSize.isValid()) {
        //Resize to save memory.
        if ((newSize.height()==0)||(newSize.width()==0)) {
            image = QPixmap(1,1);
            image.fill();
            qDebug() << "Size defined in HTML was 0. ("<<newSize << ". Return 1x1 pixel size instead to avoid repeated reload attempts";
        } else {
            if (imageReader->supportsOption(QImageIOHandler::ScaledSize)) {
                //For formats which support scaled reading (e.g. jpg), use it (as significantly faster than separate reading and scaling)
                qDebug() << " ScaledSize supported. Read with scaling";
                imageReader->setScaledSize(newSize);
                image = QPixmap::fromImageReader(imageReader);
            }
            else {
                // If format does not support scaling (e.g. png) read and scale separately.
                // benefit is that faster (lower quality) scaling can be used, while setScaledSize
                // would use high quality (=slower) scaling.
                // Note: If in future loading is implemented in separate thread,  would probably
                // make sense to use imagereader (high quality) only.
               qDebug() << " ScaledSize not supported. Read, and scale afterwards.";
               QTime subSubTimer;
               subSubTimer.start();
               image = QPixmap::fromImageReader(imageReader);
               qDebug() << "\tQPixmap::fromImageReader (without scaling) took: " << subSubTimer.restart();
               image=image.scaled(newSize,Qt::IgnoreAspectRatio,Qt::FastTransformation);
               qDebug() << "\tscaling took: " << subSubTimer.restart();
            }
            qDebug() << "Image resized to size defined in HTML\nsize of scaled image: "<<image.size();
        }
    } else {
        image = QPixmap::fromImageReader(imageReader);
        qWarning() << "image size not found. Don't resize image";
    }
    if ((image.isNull()))   {

        qWarning() << "loadFromData failed for image. Return 1x1 pixel image instead. QImageReader Error Message: "<< imageBuffer->errorString();
        image = QPixmap(1,1);
        image.fill();
    }
    delete imageReader;
    delete imageBuffer;
    qDebug() << " Creating Pixmap (including resize) from image data took" << subTimer.restart() << " milliseconds";
    qDebug() << "Loading image " << imageUrl <<" took" << timer.elapsed() << " milliseconds";
    return image;
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

//For debug output
QString WikiOnBoard::articleListItemToString(QListWidgetItem* item) {
    QUrl url = item->data(ArticleUrlRole).toUrl();
    QString urlEncoded = QString::fromUtf8(url.toEncoded().data(),url.toEncoded().length());

    QString s = QString(QLatin1String("arcticleListItem: \n\tindex: %1\n"
                              "\ttitle: %2\n"
                        "\turl (encoded): %3\n")).arg(item->data(ArticleIndexRole).toString(),
                                                      item->data(ArticleTitleRole).toString(),
                                                      urlEncoded);
    return s;
}


std::pair <bool, QListWidgetItem*> WikiOnBoard::getArticleListItem(zim::File::const_iterator it) {
    QListWidgetItem* articleItem = new QListWidgetItem();
    if (it==zimFile->end()) {
        qDebug() << "getArticleListItem iterator points is end of article list. ";
        return std::pair<bool, QListWidgetItem*> (false, articleItem);
    }
    QString articleTitle = fromUTF8EncodedStdString(it->getTitle());
    QUrl articleUrl = QUrl::fromEncoded(QUrl::toPercentEncoding(fromUTF8EncodedStdString(it->getUrl())));
    QString articleIdx = QString::number(it->getIndex());
    if (it->getNamespace() != 'A')
    {
        qDebug()
                << " Index entry to be added is not in article namespace. Stop adding titles. \n\tArticle Title: ["
                << articleIdx << "] " << articleTitle
                << "\n\tArticle Namespace: " << it->getNamespace();
        return std::pair<bool, QListWidgetItem*> (false, articleItem);
    }
    articleItem->setText(articleTitle);
    articleItem->setData(ArticleTitleRole, articleTitle);
    articleItem->setData(ArticleIndexRole, articleIdx);
    articleItem->setData(ArticleUrlRole, articleUrl);
    return std::pair<bool, QListWidgetItem*> (true, articleItem);
}

void WikiOnBoard::populateArticleList() {	
	populateArticleList(ui.articleName->text(), 0, false);
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
			zim::File::const_iterator it = zimFile->findByTitle('A',
					articleNameStdStr);
                        if (((it==zimFile->end()) || (it->getNamespace() != 'A') )) {
                            qDebug() << " No valid article >= \"" << articleName << "\" found. Try using previous entry in zim file instead";
                            if (it==zimFile->begin()) {
                                    qDebug() << " zim file contains no entries. Add nothing to list";
                                    return;
                            }
                            --it;
                            if ((it==zimFile->end()) || (it->getNamespace() != 'A') ) {
                                qWarning() << " Previous entry is neither a valid article. Bug?. Add nothing to list";
                                return;
                            }
                        }
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
                                 std::pair <bool, QListWidgetItem*> articleItemPair =  getArticleListItem(it);
                                 if (!articleItemPair.first) {
                                    break;
                                }
				if (direction_up)
					{
					if (i >= ignoreFirstN)
						{
                                                ui.articleListWidget->insertItem(0, articleItemPair.second);
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
                                                ui.articleListWidget->addItem(articleItemPair.second);
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
                        if (hasTouchScreen) {

                            //Add some items before found item (For smoother scrolling up)
                            int addedItemsCount = addItemsToArticleList(true,20);

                            qDebug() << " Added " << addedItemsCount << " items before searched item. select searched item and scroll it to top of screen";

                            //Select found item. (i.p. useful for phones which have a keypad as well, and if not enough items left below found item to fill list)
                            ui.articleListWidget->setCurrentRow(addedItemsCount);
                            //Scroll to found item. (addItemsToArticleList scrolled to first added item, which is one too early)
                            ui.articleListWidget->scrollToItem (ui.articleListWidget->currentItem() , QAbstractItemView::PositionAtTop );



                            qDebug() << "First item of list (after add)" << articleListItemToString(ui.articleListWidget->item(0));
                            qDebug() << "Last item of list (after add)" << articleListItemToString(ui.articleListWidget->item(ui.articleListWidget->count()-1));
                          } else {
                            //Non-touchscreen
                            ui.articleListWidget->setCurrentRow(0); //Select first found item
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
                            if (hasTouchScreen) {
                                //To basically same thing if scrolled.
                                qDebug() << "up key while first entry selected. Add items.";
                                  addItemsToArticleList(true);
                            } else {
                                //Old behavior. (TODO: May make sense to change to similar scheme as
                                // used with touchscreen devices)
                               QListWidgetItem *item = ui.articleListWidget->currentItem();
                                if (item->data(ArticleIndexRole).toInt() > 0)
                                    {
                                    populateArticleList(item->data(ArticleTitleRole).toString(), 1,
						true);
                                    ui.articleListWidget->setCurrentRow(
						ui.articleListWidget->count() - 1);
                                    }
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
                         if (hasTouchScreen) {
                            //To basically same thing if scrolled.
                            qDebug() << "down key while last entry selected. Add items.";
                            addItemsToArticleList(false);
                         } else {
                            //TODO check outof bounds
                            QListWidgetItem *item = ui.articleListWidget->currentItem();
                            populateArticleList(item->data(ArticleTitleRole).toString(), 1,
					false);
                            ui.articleListWidget->setCurrentRow(0);
                         }
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
		
		QUrl url = item->data(ArticleUrlRole).toUrl();
		QString urlDecoded =  url.toString();
		QString urlEncoded = QString::fromUtf8(url.toEncoded().data(),url.toEncoded().length());
		qDebug() << "articleListOpenArticle: url (decoded): " <<urlDecoded<<"\nurl (encoded):"<<urlEncoded ;
			
                //Clear article and swich to article page before loading new article
                // => Feedback to user click was accepted.
                articleViewer->clear();
                switchToArticlePage();
                articleViewer->setSource(url);
                }
	}

void ArticleVi::openArticleByUrl(QUrl url)
{
    QString path = url.path();
    QString encodedPath = QString::fromUtf8(url.encodedPath().data(),url.encodedPath().length());

    qDebug() << "openArticleByUrl: " <<url.toString()<<"\nurl.path():"<<path << "\nurl.encodedPath():"<< encodedPath;

    if (url==welcomeUrl) {
        qDebug()  << "Url is welcome URL. Set article text to welcome text";
        QString zimDownloadUrl = QString(tr("https://github.com/cip/WikiOnBoard/wiki/Get-eBooks","Change link to page with localized zim files. (e.g https://github.com/cip/WikiOnBoard/wiki/Get-eBooks-DE"));
        QString getEBookLinkCaption = QString(tr("Download zimfile", "link"));
        QString zimDownloadUrlHtml = QString(tr("<a href=\"%1\">%2</a>", "DON'T translate this").arg(zimDownloadUrl,getEBookLinkCaption));

        QString informativeText = QString(tr("[TRANSLATOR] No zimfile selected. getEBook link  %1 opens url %3 with info where to get eBooks. Menu option %2 in option menu %4 opens zimfile on mobile", "Text is interpreted as HTML. Html for body and link (%1) automatically added. Other Html tags can be used if desired")).arg(zimDownloadUrlHtml,openZimFileDialogAction->text(),zimDownloadUrl, positiveSoftKeyActionMenuArticlePage->text());
        articleViewer->setHtml(informativeText);

    } else if (zimFile!=NULL) {

        //Only read article, if not same as currently
        //viewed article (thus don´t reload for article internal links)
        //TODO: this does not work as appearantly before calling changedSource
        // content is deleted. Therefore for now just reload in any case url.
        // Optimize (by handling in anchorClicked, but check what happens
        //	to history then)
        //if (!path.isEmpty() && (currentlyViewedUrl.path()!=url.path())) {

        QString articleTitle = getArticleTitleByUrl(encodedPath);
        qDebug() << "Set index search field to title of article: "<< articleTitle;
        ui.articleName->setText(articleTitle);

        QTime timer;
        timer.start();
        QString articleText = getArticleTextByUrl(encodedPath);
        qDebug() << "Reading article " <<path <<" from zim file took" << timer.restart() << " milliseconds";
        articleViewer->setHtml(articleText);
        qDebug() << "Loading article into textview (setHtml()) took" << timer.restart() << " milliseconds";
        if (url.hasFragment())
        {
            //Either a link within current file (if path was empty), or to   newly opened file
            QString fragment = url.fragment();
            articleViewer->scrollToAnchor(fragment);
            //Now text visible, but cursor not moved.
            //=> Move cursor to current visisble location.
            // TODO: no better way to achieve this. Furthermore, actually
            //	cursor reason for problem or something else?)
            moveTextBrowserTextCursorToVisibleArea();
        }
        else
        {
            QTextCursor cursor = articleViewer->textCursor();

            //Move cursor to start of file
            cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
            articleViewer->setTextCursor(cursor);
            if (hasTouchScreen == false)
            {
                //FIXME: This is a really ugly hack for the nextprevious link problem
                // described in keyEventHandler. Note that for links with anchor this
                // does not work
                // On touchscreen devices workaround is not performed.
                QKeyEvent *remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
                                                            Qt::Key_Up, Qt::NoModifier, QString(), false, 1);
                QApplication::sendEvent(articleViewer, remappedKeyEvent);
                remappedKeyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down,
                                                 Qt::NoModifier, QString(), false, 1);
                QApplication::sendEvent(articleViewer, remappedKeyEvent);

            }
        }
        /// ui.stackedWidget->setCurrentWidget(ui.articlePage);
        qDebug() << "Loading article into textview (gotoAnchor/moveposition) took" << timer.restart() << " milliseconds";
    } else {
        qWarning() << "openArticleByUrl called with non welcome page url while no zim file open. Should not happen";
    }
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
                        "[TRANSLATOR] Explain that link %1 clicked in article is not contained in ebook and needs to be opened in webrowser. Ask if ok.\n%2")).arg(
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

void WikiOnBoard::on_articleViewer_anchorClicked(QUrl url)
	{
	qDebug() << "on_textBrowser_anchorClicked: Url: " << url.toString();
        qDebug() << " Check  url.scheme(). " <<url.scheme();
        if ((QString::compare(url.scheme(), QLatin1String("http"), Qt::CaseInsensitive)==0)||
                (QString::compare(url.scheme(), QLatin1String("https"), Qt::CaseInsensitive)==0))
		{
                    qDebug() << "url scheme is http or https => open in browser";
                    openExternalLink(url);
		}
	else
                {
                        qDebug() << "Url is not an external website => search in zim file";
                        articleViewer->setSource(url);
		}
	}

void WikiOnBoard::on_articleViewer_sourceChanged(QUrl url)
	{
	showWaitCursor();
	openArticleByUrl(url);
	hideWaitCursor();
	currentlyViewedUrl = url;
	}

void WikiOnBoard::backArticleHistoryOrIndexPage()
	{
        if (articleViewer->isBackwardAvailable())
		{
                articleViewer->backward();
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
#if defined(Q_OS_SYMBIAN)
        //Hard code path to memory card/mass memory.  Tried QDir::homePath(); and QDesktopServices::DataLocation,
        // both return app private dir on c:
        path = QLatin1String("e:\\");
#endif
#if !defined(Q_OS_SYMBIAN)
        path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
#endif
        qDebug() << "No zim file open. Use  path for file dialog : "<<path;
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
                                                tr("eBooks (*.zim *.zimaa)"));
#if defined(Q_OS_SYMBIAN)
    QApplication::setNavigationMode(Qt::NavigationModeNone);
#endif
    if (!file.isNull() && openZimFile(file)) {
        qDebug() << "Opened zim file: " << file;
        QSettings settings;
        // Store this file to settings, and automatically open next time app is started
        settings.beginGroup(QLatin1String("ZimFile"));
        settings.setValue(QLatin1String("lastZimFile"), file);
        settings.endGroup();
        articleViewer->clearHistory();
        switchToIndexPage(); //In case currently viewing an article.
        populateArticleList();
    } else {
        //Either no file selected or open failed.
        if (zimFile==NULL) {
            qDebug() << "Open failed or cancelled and not file was open before => Show welcome page";
            switchToWelcomePage();
        }
    }// else: no  zim file selected, or open failed => keep previously openend zim file open
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
        QString zimfilename = QFile::decodeName(zimFile->getFilename().c_str());
        informativeText = QString(tr(""
			 "Current Zim File: %1\n"
                         "Articles : %2, Images: %3, Categories: %4\n",
                                     "Add new line after text")).arg(
                                         zimfilename,
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
                                    "Relation: %7\n", "Add newline after Text")).arg(
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
	
	QString selfSignedText = QLatin1String("");
	#if defined(Q_OS_SYMBIAN)
		#if __IS_SELFSIGNED__==1
		selfSignedText.append(tr("application is self-signed", "only displayed if application is self-signed"));
		#endif
	#endif
	
	QMessageBox msgBox;
	msgBox.setText(tr("About"));	
	QString text = QString (tr(""
			"WikiOnBoard %1\n"
			"Author: %2\n"
			"Uses zimlib (openzim.org) and liblzma.\n"
			"Build date: %3\n"
                        "%4\n",
                        "Add new line after text")).arg(
					QString::fromLocal8Bit(__APPVERSIONSTRING__),
					tr("Christian Puehringer"), 
					QString::fromLocal8Bit(__DATE__),
					selfSignedText
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

void WikiOnBoard::switchToArticlePage()
        {

        if (zimFile!=NULL) {
            #ifdef Q_OS_SYMBIAN
                positiveSoftKeyActionMenuArticlePage->setSoftKeyRole(QAction::PositiveSoftKey);
                positiveSoftKeyActionMenuArticlePageNoFileOpen->setSoftKeyRole(QAction::NoSoftKey);
                backArticleHistoryAction->setSoftKeyRole(QAction::NegativeSoftKey);
                emptyAction->setSoftKeyRole(QAction::NoSoftKey);
            #else
                menuBar()->clear();
                menuBar()->addMenu(menuArticlePage);
            #endif

        } else {            
            qDebug() << " switchToArticlePage with no zim file opened. (Basically happens for Welcome page) Display menu without goto index.  And don't display back. ";
            #ifdef Q_OS_SYMBIAN
                positiveSoftKeyActionMenuArticlePageNoFileOpen->setSoftKeyRole(QAction::PositiveSoftKey);
                emptyAction->setSoftKeyRole(QAction::NegativeSoftKey);
                positiveSoftKeyActionMenuArticlePage->setSoftKeyRole(QAction::NoSoftKey);
                backArticleHistoryAction->setSoftKeyRole(QAction::NoSoftKey);
            #else
                menuBar()->clear();
                menuBar()->addMenu(menuArticlePageNoFileOpen);
                qDebug() << "add openZimFileDialogAction to toolbar";
                ui.toolBar->addAction(openZimFileDialogAction);

#endif

        }
        positiveSoftKeyActionMenuIndexPage->setSoftKeyRole(QAction::NoSoftKey);

        clearSearchAction->setSoftKeyRole(QAction::NoSoftKey);
	
	ui.stackedWidget->setCurrentWidget(ui.articlePage);

        articleViewer->setFocus();
	}


void WikiOnBoard::switchToWelcomePage()
{
    articleViewer->clear();
    switchToArticlePage();
    articleViewer->setSource(welcomeUrl);
}


void WikiOnBoard::switchToIndexPage()
	{
#ifdef Q_OS_SYMBIAN
    positiveSoftKeyActionMenuIndexPage->setSoftKeyRole(QAction::PositiveSoftKey);
    positiveSoftKeyActionMenuArticlePage->setSoftKeyRole(QAction::NoSoftKey);
    positiveSoftKeyActionMenuArticlePageNoFileOpen->setSoftKeyRole(QAction::NoSoftKey);

    backArticleHistoryAction->setSoftKeyRole(QAction::NoSoftKey);
    emptyAction->setSoftKeyRole(QAction::NoSoftKey);
    clearSearchAction->setSoftKeyRole(QAction::NegativeSoftKey);
#else
        menuBar()->clear();
        menuBar()->addMenu(menuIndexPage);
#endif
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
        int position = articleViewer->cursorForPosition(QPoint(0, 0)).position();
        QTextCursor cursor = articleViewer->textCursor();
	cursor.setPosition(position, QTextCursor::MoveAnchor);
        articleViewer->setTextCursor(cursor);
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
                                articleViewer->verticalScrollBar()->triggerAction(
						QAbstractSlider::SliderPageStepSub);
                                if (articleViewer->verticalScrollBar()->value()!=articleViewer->verticalScrollBar()->minimum()) {
					//Only scroll down again a little, if not at beginning of article
                                        articleViewer->verticalScrollBar()->triggerAction(
										QAbstractSlider::SliderSingleStepAdd);								
				}
				break;
			case Qt::Key_VolumeUp:							
			case Qt::Key_8: //Scroll one page down (-one line as else one line may never be visible entirely)
                                articleViewer->verticalScrollBar()->triggerAction(
						QAbstractSlider::SliderPageStepAdd);
                                if (articleViewer->verticalScrollBar()->value()!=articleViewer->verticalScrollBar()->maximum()) {
					//Only scroll back again a little, if not at end of article
                                        articleViewer->verticalScrollBar()->triggerAction(
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
    qDebug() << "resizeEvent:\n "
                "New size: width: " << event->size().width() <<" height: " << event->size().height() << ""
                "Old size: width: " << event->oldSize().width() <<" height: " << event->oldSize().height();
    qDebug() << "  Wikionboard: width: " << this->width() << " height:" << this->height();
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
                                //ui.articleListWidget->setCurrentRow(0);
				}
			}

		}
	}



void WikiOnBoard::workAreaResized(int screen) {
    qDebug() << "In work area resized";
    QRect avail = QApplication::desktop()->availableGeometry();
    qDebug()<< "availableGeometry: Width: " << avail.width() << " Height: "<<avail.height();
#if defined(Q_OS_SYMBIAN)
    #if __ENABLE_SPLITSCREENKEYBOARD__==1
    if (hasTouchScreen) {
        //Workaround for problem that with
        //  split screen virtual keyboard orientation switch shrinks the wikionboard window to size
        // above keyboard. (issue 51)
        // Not understood why workaround  works, therefore significant risk of sideeffects.
        //   One countermeasure is to restrict execution of workaround to minimal set: only symbian
        //      touchscreen devices.
        //  TODO: restrict to s^3 only, as no splitscreen keyboard on S^1 devices available
        //Note: ui->articleName->clearFocus was used in resizeEvent() to hide virtual keyboard before
        //          Hiding does not work anymore with workaround.(but is anyway note required)
        qDebug() << "Workaround for splitscreen keyboard orientation change issue. (hide and show main widget)";
        setVisible(false);
        QApplication::processEvents();
        setVisible(true);
    }
    #endif
#endif
}
void WikiOnBoard::toggleFullScreen()
	{
        if (fullScreen)
		{
                 fullScreen = false;
                 showMaximized();

		}
	else
		{
                fullScreen = true;
		showFullScreen();
		}
	QSettings settings;
	settings.beginGroup(QLatin1String("UISettings"));
	if ((!settings.contains(QLatin1String("fullScreen"))) || (settings.value(QLatin1String("fullScreen"),
                        false).toBool() != fullScreen))
		{
                settings.setValue(QLatin1String("fullScreen"), fullScreen);
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
                articleViewer->zoomOut(abs(zoomDelta));
		}
	else
		{
                articleViewer->zoomIn(zoomDelta);
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
        //On Symbian^3 waitcursor not working for some reason.
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
        // processEvent leads clears article page while loading.
        // (At least some user feedback on S^3 that something is going on...)
        qApp->processEvents();
}

void WikiOnBoard::hideWaitCursor()
	{	
	QApplication::restoreOverrideCursor();
        #if defined(Q_OS_SYMBIAN)
            QApplication::setNavigationMode(Qt::NavigationModeNone);
        #endif
}

//TODO Consider merging with regular populate Article List 
//addCount: Number of article entries to be added (at most)
//maxCount: maximumCount of article entries allowed. Deletes items furthest away
//from added item for each item added
int WikiOnBoard::addItemsToArticleList(bool up, int addCount, int maxCount)
	{
    qDebug() << "WikiOnBoard::addItemsToArticleList (up:"<<up<<" addCount: "<<addCount <<" maxCount: "<<maxCount;
    if (zimFile != NULL)
		{
		try
			{
                        if (ui.articleListWidget->count()==0) {
                            qWarning() << "articleList empty. addItemsToArticleList requires a least on article in the article list." ;
                            return 0;
                        }

                        if (up)
				{
                                qDebug()
                                                << "AddItems to beginning of list";

                                QListWidgetItem * firstArticleInCurrentList = ui.articleListWidget->item(0);
                                QString
						titleFirstArticleInCurrentList =
                                                                firstArticleInCurrentList->data(
										ArticleTitleRole).toString();

                                //Zim index is UTF8 encoded. Therefore use utf8 functions to access
                                // it.
                                std::string articleTitleStdStr = std::string(
						titleFirstArticleInCurrentList.toUtf8());
                                qDebug() << " First article in current list is: "<< articleListItemToString(firstArticleInCurrentList);

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
                                        return 0;
					}

				int insertedItemsCount = 0;
                                while (insertedItemsCount < addCount)
					{
					--it;
                                        std::pair <bool, QListWidgetItem*> articleItemPair =  getArticleListItem(it);
                                        if (!articleItemPair.first) {
                                            break;
                                        }
                                        ui.articleListWidget->insertItem(0, articleItemPair.second);
					insertedItemsCount++;
					//Remove last item to avoid eating up to much memory. 
					//(But ensure that that there enough "old" items left
					// to fill the article list. (first added new item 
					// should be on top of list at the end)
                                        if (ui.articleListWidget->count()>maxCount) {
						QListWidgetItem *lastItem = ui.articleListWidget->takeItem(
							ui.articleListWidget->count() - 1);
						delete lastItem;
					}
					//order is different					
					if (it == zimFile->beginByTitle())
						{
                                                qDebug()
                                                                << "Beginning of title index reached. Stop adding titles. Last added title :"<< articleListItemToString(articleItemPair.second);
						break;
                                                }
					} //End while
				if (insertedItemsCount>0) {
					QListWidgetItem *firstNewItem = ui.articleListWidget->item(insertedItemsCount-1);
                                        qDebug() << insertedItemsCount
                                                << " items inserted at beginning of list. Scroll so that firstly newly added article is at bottom of list. Firstly new added article: "<<articleListItemToString(firstNewItem);

                                        ui.articleListWidget->scrollToItem(firstNewItem,QAbstractItemView::PositionAtTop);
                                        return insertedItemsCount;
				}	else {
					qDebug() << "No items inserted";
                                        return 0;
				}
				}
			else
				{// end  (if up()). => up=false
				qDebug()
                                                << "AddItems to end of list";
                                QListWidgetItem* lastArticleInCurrentList = ui.articleListWidget->item(
                                            ui.articleListWidget->count() - 1);
                                QString titleLastArticleInCurrentList =
                                                lastArticleInCurrentList->data(
								ArticleTitleRole).toString();

				//Zim index is UTF8 encoded. Therefore use utf8 functions to access
				// it.
				std::string articleTitleStdStr = std::string(
						titleLastArticleInCurrentList.toUtf8());
                                qDebug() << " Last article in current list is: " << articleListItemToString(ui.articleListWidget->item(
                                                                                                                ui.articleListWidget->count() - 1));

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
                                        return 0;
					}
				int insertedItemsCount = 0;
                                while (insertedItemsCount < addCount)
					{
					++it;
                                        std::pair <bool, QListWidgetItem*> articleItemPair =  getArticleListItem(it);
                                        if (!articleItemPair.first) {
                                            break;
                                        }
                                        ui.articleListWidget->addItem(articleItemPair.second);
					//Remove first item to avoid eating up to much memory. 
					//TODO: increase overlap. (User scrolls one direction than other else leads directly to reload)
                                        if (ui.articleListWidget->count()>maxCount) {
						QListWidgetItem *firstItem =
							ui.articleListWidget->takeItem(0);
						delete firstItem;
					}
					insertedItemsCount++;
					if (it == zimFile->end())
						{
						qDebug()
                                                                << "End of title index reached. Stop adding titles. Last added title: "<< articleListItemToString(articleItemPair.second);
						break;
						}
					} //End while
					int indexFirstNewItem = ui.articleListWidget->count()-insertedItemsCount;
					if ((indexFirstNewItem >= 0) && (indexFirstNewItem
						< ui.articleListWidget->count()))
					{
						QListWidgetItem *firstNewItem = ui.articleListWidget->item(
							indexFirstNewItem);
                                                qDebug() << insertedItemsCount
                                                        << " items appended to end of list. Scroll so that firstly newly added article is at bottom of list. Firstly new added article: "<<articleListItemToString(firstNewItem);
                                                ui.articleListWidget->scrollToItem(firstNewItem,
							QAbstractItemView::PositionAtBottom);
                                                return insertedItemsCount;
					}	else {
						qDebug() << "No items inserted";
                                                return 0;
					}
				} // End else (up=false)
			} //End try
		catch (const std::exception& e)
			{
			ui.articleListWidget->addItem(QLatin1String("Error occured"));
			}
		} //End if (zimFile!=NULL)
                return 0;
	}	

//http://www.developer.nokia.com/Community/Wiki/Implementing_a_split_screen_for_software_keyboard
// Qt slot that sets the approriate flag to the text editor.
void WikiOnBoard::enableSplitScreen()
{
    // The text editor must be open before setting the partial screen flag.

#if defined(Q_OS_SYMBIAN)
    #if __ENABLE_SPLITSCREENKEYBOARD__==1

    MCoeFepAwareTextEditor *fte = CCoeEnv::Static()->AppUi()->InputCapabilities().FepAwareTextEditor();

    // FepAwareTextEditor() returns 0 if no text editor is present
    if (fte)
    {
        CAknEdwinState *state = STATIC_CAST(CAknEdwinState*, fte->Extension1()->State(KNullUid));
        state->SetFlags(state->Flags() | EAknEditorFlagEnablePartialScreen);
    }
    #endif
#endif
}
ArticleViewer::ArticleViewer(QWidget* parent, WikiOnBoard* wikiOnBoard) : QTextBrowser(parent),wikiOnBoard(wikiOnBoard)
 {
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

 }
 QVariant ArticleViewer::loadResource ( int type, const QUrl & name ) {
       if (type==QTextDocument::ImageResource) {
           if (showImages) {               
               QString encodedPath = QString::fromUtf8(name.encodedPath().data(),name.encodedPath().length());

               qDebug() << "loadResource.: type is ImageResource and showImages =1 => load image from zim file. " << name.toString()<<"\nurl.path():"<<name.path() << "\nurl.encodedPath():"<< encodedPath;               
               return wikiOnBoard->getImageByUrl(encodedPath);
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
