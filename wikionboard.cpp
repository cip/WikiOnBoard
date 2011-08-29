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
#include <QDesktopWidget>
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
#include <stdexcept>

#if defined(Q_OS_SYMBIAN)
#include <eikbtgpc.h>       // symbian: LIBS += -lavkon -leikcoctl
#endif

//Get VERSION from qmake .pro file as string 
#define __VER1M__(x) #x
#define __VERM__(x) __VER1M__(x)
#define __APPVERSIONSTRING__ __VERM__(__APPVERSION__)



WikiOnBoard::WikiOnBoard(void* bgc, QWidget *parent) :
    QMainWindow(parent), m_bgc(bgc), welcomeUrl(QUrl(QLatin1String("wikionboard://welcome")))

        {
        zimFileWrapper = new ZimFileWrapper(this);
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
	


	QSettings settings;
	settings.beginGroup(QLatin1String("ZimFile"));

	QString lastZimFile = settings.value(QLatin1String("lastZimFile")).toString();
	settings.endGroup();

	ui.setupUi(this);
        articleViewer = new ArticleViewer(ui.articlePage,zimFileWrapper,hasTouchScreen);
        ui.gridLayout_3->addWidget(articleViewer);        
        indexList = new IndexList(ui.indexPage,zimFileWrapper, hasTouchScreen);
        ui.gridLayout_2->addWidget(indexList,1,0);
        setStatusBar(0); //Remove status bar to increase useable screen size.
	//TODO still not perfect, quite some distance between
        //  widget and menu.

        settings.beginGroup(QLatin1String("UISettings"));
        fullScreen = settings.value(QLatin1String("fullScreen"), false).toBool();
        settings.endGroup();
        if (connect(articleViewer,SIGNAL(sourceChanged(QUrl)),this, SLOT(on_articleViewer_sourceChanged(QUrl)))) {
            qDebug() << "Connected sourceChanged";
        } else {
            qWarning()<<"Could not connect sourceChanged";
        }
        if (connect(articleViewer,SIGNAL(articleOpened(QString)),this, SLOT(onArticleOpened(QString)))) {
            qDebug() << "Connected articleOpened";
        } else {
            qWarning()<<"Could not connect articleOpened";
        }

        if (connect(articleViewer,SIGNAL(openExternalLink(QUrl)),this, SLOT(openExternalLink(QUrl)))) {
            qDebug() << "Connected openExternalLink";
        } else {
            qWarning()<<"Could not connect openExternalLink";
        }
        connect(QApplication::desktop(), SIGNAL(workAreaResized(int)), this, SLOT(workAreaResized(int)));

	
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

        connect(indexList, SIGNAL(itemClicked(QListWidgetItem*)), this,
                        SLOT(articleListOpenArticle(QListWidgetItem *))); //For touchscreen devices
        //slot just calls openArticlAction.trigger
        connect(openArticleAction, SIGNAL(triggered()), this,
                        SLOT(articleListOpenArticle()));


        //connect(indexList, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(articleListOpenArticle(QListWidgetItem *)));
        indexList->addAction(openArticleAction);

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
        connect(zoomOutAction, SIGNAL(triggered()), articleViewer, SLOT(zoomOutOneStep()));
        connect(zoomInAction, SIGNAL(triggered()), articleViewer, SLOT(zoomInOneStep()));

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

	}

bool WikiOnBoard::openZimFile(QString zimfilename) {
    QMessageBox::StandardButton reply;
    bool ok = zimFileWrapper->openZimFile(zimfilename);
    if (!ok) {
        reply = QMessageBox::critical(this, tr("Error on opening zim file"),
                                      QString(tr("Error on opening zim file %1.\nError message:%2\n").arg(zimfilename,zimFileWrapper->errorString())),
                                      QMessageBox::Ok);
    }
    return ok;
}


void WikiOnBoard::articleListOpenArticle(QListWidgetItem * item)
        {
        openArticleAction->trigger();
        }

void WikiOnBoard::articleListOpenArticle()
{
    QUrl url = indexList->currentItemUrl();
    if (!url.isEmpty())
    {

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
    if (!zimFileWrapper->isValid())
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
        path = zimFileWrapper->getFilename();
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
        indexList->populateArticleList(ui.articleName->text());
    } else {
        //Either no file selected or open failed.
        if (!zimFileWrapper->isValid()) {
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
        if (!zimFileWrapper->isValid()) {
		informativeText = QString(tr("No zim file is currently opened"));	
	} else {
	
        QByteArray uuidBA =zimFileWrapper->getUUID();
        QString zimfilename = zimFileWrapper->getFilename();
        informativeText = QString(tr(""
			 "Current Zim File: %1\n"
                         "Articles : %2, Images: %3, Categories: %4\n",
                                     "Add new line after text")).arg(
                                         zimfilename,
                                         QString::number(zimFileWrapper->getNamespaceCount(QLatin1Char('A'))), //Including redirects
                                         QString::number(zimFileWrapper->getNamespaceCount(QLatin1Char('I'))),
                                         QString::number(zimFileWrapper->getNamespaceCount(QLatin1Char('U')))
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
                                         zimFileWrapper->getMetaDataString(QLatin1String("Title")),
                                         zimFileWrapper->getMetaDataString(QLatin1String("Creator")),
                                         zimFileWrapper->getMetaDataString(QLatin1String("Date")),
                                         zimFileWrapper->getMetaDataString(QLatin1String("Source")),
                                         zimFileWrapper->getMetaDataString(QLatin1String("Description")),
                                         zimFileWrapper->getMetaDataString(QLatin1String("Language")),
                                         zimFileWrapper->getMetaDataString(QLatin1String("Relation"))
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

        if (zimFileWrapper->isValid()) {
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
        indexList->populateArticleList(ui.articleName->text());
	}

void WikiOnBoard::searchArticle()
	{
        indexList->populateArticleList(ui.articleName->text());
	}

//FIXME move to appropriate components.
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
                                 QApplication::sendEvent(indexList, remappedKeyEvent);
				 */
                                indexList->articleListSelectPreviousEntry();
				break;
			case Qt::Key_Down:
				/*remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
				 Qt::Key_Down, Qt::NoModifier, false, 1);
                                 QApplication::sendEvent(indexList, remappedKeyEvent);*/
                                indexList->articleListSelectNextEntry();
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
                        false).toBool() != fullScreen))
		{
                settings.setValue(QLatin1String("fullScreen"), fullScreen);
		}
	settings.endGroup();
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

void WikiOnBoard::onArticleOpened(QString articleTitle) {
    qDebug() << "Set index search field to title of article: "<< articleTitle;
    ui.articleName->setText(articleTitle);
}
