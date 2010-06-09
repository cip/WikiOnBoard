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
//TODO: not necessary for symbian, why necessary on linux? (and anyway exception should be replaced
//  by something else
#include <stdexcept>

#if defined(Q_OS_SYMBIAN)
#include <eikbtgpc.h>       // symbian: LIBS += -lavkon -leikcoctl
#endif

WikiOnBoard::WikiOnBoard(void* bgc, QWidget *parent) :
	QMainWindow(parent), m_bgc(bgc)
	{
	zimFile = NULL; //zimFile unitialized until,
	//file loaded (either stored filename from last run,
	// or user loads file). To allow test for == NULL, explicitlz
	// set to null. (at least in symbian emulator else 0xCCCCCCCC

	QSettings settings;
	settings.beginGroup("ZimFile");

	QString lastZimFile = settings.value("lastZimFile").toString();
	settings.endGroup();
	if (!lastZimFile.isEmpty())
		{
		openZimFile(lastZimFile);
		}

	ui.setupUi(this);
	setStatusBar(0); //Remove status bar to increase useable screen sizz.
	//TODO still not perfect, quite some distance between
	//  widgeht and menu.

	settings.beginGroup("UISettings");
	int zoomInit = settings.value("zoomLevel", -1).toInt();
	bool fullScreen = settings.value("fullScreen", false).toBool();
	settings.endGroup();
	QTextDocument* defaultStyleSheetDocument = new QTextDocument(this);
	//Override link color. At least on symbian per default textbrowser uses phone color scheme which is
	// typically not very ergonomical. (e.g. white text on green background with N97 standard scheme). 
	// Text and background color is changed in stylesheet property of textBrowser. Link color (white
	// on N97...) is changed here. 
	// Only do this one, and not on every article load as this
	// appearantly affects zoom level. 
	defaultStyleSheetDocument->setDefaultStyleSheet("a:link{color: blue}");	
	ui.textBrowser->setDocument(defaultStyleSheetDocument);		
	zoomLevel = 0;
	zoom(zoomInit);

	if (fullScreen)
		{
		toggleFullScreen();
		}
	else
		{
		showMaximized();
		}
	currentlyViewedUrl = QUrl("");

	qDebug() << "Debug output test \n";

	openZimFileDialogAction = new QAction(tr("Open Zimfile"), this);
	connect(openZimFileDialogAction, SIGNAL(triggered()), this,
			SLOT(openZimFileDialog()));

	downloadZimFileAction = new QAction(tr("Download Zimfile"), this);
	connect(downloadZimFileAction, SIGNAL(triggered()), this,
			SLOT(downloadZimFile()));

	//Define search action (populates article list view with articles found searching for article
	//name line edit.
	// 
	searchArticleAction = new QAction("Search Article", this);

	connect(searchArticleAction, SIGNAL(triggered()), this,
			SLOT(searchArticle()));
	//connect(ui.articleName, SIGNAL(textChanged(QString)), searchArticleAction,
	//		SIGNAL(triggered())); //Automatically search after text changed
	//TODO: this hangs, instead textEdited used. find out why not working 
	connect(ui.articleName, SIGNAL(textEdited(QString)), searchArticleAction,
			SIGNAL(triggered())); //Automatically search after text changed


	//ui.findButton->addAction(searchArticleAction);
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
	clearSearchAction = new QAction("Clear", this);
	connect(clearSearchAction, SIGNAL(triggered()), ui.articleName,
				SLOT(clear()));
	connect(clearSearchAction, SIGNAL(triggered()), searchArticleAction,
					SIGNAL(triggered())); //Automatically search after cleared.

	this->addAction(clearSearchAction);
		
	openArticleAction = new QAction("Open Article", this);

	connect(openArticleAction, SIGNAL(triggered()), this,
			SLOT(articleListOpenArticle()));
	connect(ui.articleListWidget, SIGNAL(itemClicked(QListWidgetItem*)), this,
			SLOT(articleListOpenArticle(QListWidgetItem *))); //For touchscreen devices
	//slot just calls openArticlAction.trigger

	//connect(ui.articleListWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(articleListOpenArticle(QListWidgetItem *)));
	ui.articleListWidget->addAction(openArticleAction);

	//Open article when clicked or return key clicked. Note that for keypad phones return (amongst others) is
	// forwarded to articleListWidget, so that this works even if it has not focus.

	switchToIndexPageAction = new QAction("Switch to index page", this);
	connect(switchToIndexPageAction, SIGNAL(triggered()), this,
			SLOT(switchToIndexPage()));
	this->addAction(switchToIndexPageAction);

	//	ui.actionSearch->setSoftKeyRole(QAction::NegativeSoftKey); //Right softkey: return to search page
	///	ui.articlePage->addAction(ui.actionSearch);

	backArticleHistoryAction = new QAction("Back", this);
	connect(backArticleHistoryAction, SIGNAL(triggered()), this,
			SLOT(backArticleHistoryOrIndexPage()));
	this->addAction(backArticleHistoryAction); 
	
	toggleFullScreenAction = new QAction("Toggle Fullscreen", this); //TODO shortcut
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
		QRegExp rx("(.*\\.zim)\\D\\D");
	    rx.setCaseSensitivity(Qt::CaseInsensitive);
	    zimFileName.replace(rx,"\\1");
		std::string zimfilename = zimFileName.toStdString(); //
		zimFile = new zim::File(zimfilename);
		}
	catch (const std::exception& e)
		{
			QMessageBox::StandardButton reply;
		     reply = QMessageBox::critical(this, tr("Error on opening zim file"),
		                                     e.what(),
		                                     QMessageBox::Ok);		 
		}
	}

QString WikiOnBoard::getArticleTextByIdx(QString articleIdx)
	{
	QString articleText = QString("ERROR");
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
		return e.what();
		}

	return articleText;
	}

QString WikiOnBoard::getArticleTextByUrl(QString articleUrl)
	{
	QString articleText = QString("ERROR");
	zim::Blob blob;
	try
		{
		std::string articleNameStdStr = std::string(articleUrl.toUtf8());
		zim::File::const_iterator it = zimFile->find('A', articleNameStdStr);
		if (it == zimFile->end())
			throw std::runtime_error("article not found");
		if (it->isRedirect())
			{
			articleNameStdStr = it->getRedirectArticle().getUrl();
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
		return e.what();
		}

	return articleText;
	}

QString WikiOnBoard::getArticleTextByTitle(QString articleTitle)
	{
	QString articleText = QString("ERROR");
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
		return e.what();
		}

	return articleText;
	}

void WikiOnBoard::populateArticleList() {	
	populateArticleList(ui.articleName->text(), 0, false);
	ui.articleListWidget->setCurrentRow(0); //Select first found item
}

void WikiOnBoard::populateArticleList(QString articleName, int ignoreFirstN,
		bool direction_up)
	{
	if (zimFile != NULL)
		{
		try
			{ 
			//Zim index is (appearantly?) UTF8 encoded. Therefore use utf8 functions to access
			// it.
			std::string articleNameStdStr = std::string(articleName.toUtf8());
			zim::File::const_iterator it = zimFile->findByTitle('A',
					articleNameStdStr);
			if (!direction_up) {
				// If populating in reverse direction, don´t clear items now
				// but instead each time a new item is added. This avoids
				// that cannot be fully filled if the beginning of the zim file
				// is reached. 
				ui.articleListWidget->clear();
			}
			int i = 0;
			int insertedItemsCount = 0;
			while (true)
				{

				QString articleTitle = QString::fromUtf8(
						it->getTitle().c_str(), it->getTitle().size());
				QUrl articleUrl(QString::fromUtf8(it->getUrl().c_str(),
						it->getUrl().size()));
				QString articleIdx = QString::number(it->getIndex());
				QListWidgetItem* articleItem = new QListWidgetItem();
				articleItem->setText(articleTitle);

				articleItem->setData(ArticleTitleRole, articleTitle);
				articleItem->setData(ArticleIndexRole, articleIdx);
				articleItem->setData(ArticleUrlRole, articleUrl);
				// articleItem->setToolTip(articleIdx); ////TODO check to better use model list view,
				// easily save url as well.
				if (direction_up)
					{
					if (i >= ignoreFirstN)
						{
						ui.articleListWidget->insertItem(0, articleItem);
						insertedItemsCount++;
						QListWidgetItem *lastItem = ui.articleListWidget->takeItem(ui.articleListWidget->count() - 1);
						delete lastItem;							
						}
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
				if (insertedItemsCount > 0)
					{
					//Calculate height of all inserted items, and stop
					//insertion when visible area of list is full.
					int itemHeight = ui.articleListWidget->visualItemRect(
							ui.articleListWidget->item(0)).height();
					int
							articleListWidgetHeight =
									ui.articleListWidget->maximumViewportSize().height();
					QListWidgetItem *bottomItem = ui.articleListWidget->item(insertedItemsCount - 1);
					int bottomItemBottom = ui.articleListWidget->visualItemRect(
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
			ui.articleListWidget->addItem("Error occured");
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

	path = path.replace('+', ' '); //TODO hack for '+' in article names  with spaces (de wik),
	// find out what's the root cause. (
	//Note that with this hack articles containg '+'
	// cannot be opened as a link

	//This is on purpose, so if link does not work its easier to search it in index.
	//TODO remove if links fixed

	ui.articleName->setText(path);
	
	QString articleText = getArticleTextByUrl(path);
	ui.textBrowser->setHtml(articleText);
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
		//FIXME: This is a really ugly hack for the nextprevious link problem
		// described in keyEventHandler. Note that for links with anchor this 
		// does not work
		QKeyEvent *remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
				Qt::Key_Up, Qt::NoModifier, false, 1);
		QApplication::sendEvent(ui.textBrowser, remappedKeyEvent);
		remappedKeyEvent = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down,
				Qt::NoModifier, false, 1);
		QApplication::sendEvent(ui.textBrowser, remappedKeyEvent);

		}
	/// ui.stackedWidget->setCurrentWidget(ui.articlePage);

	}

void WikiOnBoard::on_textBrowser_anchorClicked(QUrl url)
	{
	if (!QString::compare(url.scheme(), "http", Qt::CaseInsensitive))
		{
		//External link, open browser
		QDesktopServices::openUrl(url);
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
		path = QString::fromStdString(zimFile->getFilename());
		}
        #if defined(Q_OS_SYMBIAN)
            QApplication::setNavigationMode(Qt::NavigationModeCursorAuto);
        #endif
	//Enable virtual mouse cursor on non-touch devices, as 
	// else file dialog not useable 
	QString file = QFileDialog::getOpenFileName(this,
			"Choose eBook in zim format to open", path,
			"eBooks (*.zim*);;All files (*.*)");
        #if defined(Q_OS_SYMBIAN)
            QApplication::setNavigationMode(Qt::NavigationModeNone);
        #endif
        if (!file.isNull())
		{
		openZimFile(file);
		QSettings settings;
		// Store this file to settings, and automatically open next time app is started
		settings.beginGroup("ZimFile");
		settings.setValue("lastZimFile", file);
		settings.endGroup();
		ui.textBrowser->clearHistory();
		populateArticleList();
		}

	}

void WikiOnBoard::downloadZimFile()
	{
	QString zimDownloadUrl(tr("http://openzim.org/ZIM_File_Archive"));
	QMessageBox msgBox;
	msgBox.setText(tr("Download ZIM file"));
	msgBox.setInformativeText(
			tr("Open a webbrowser to download zim files from ")
					% zimDownloadUrl
					% tr(
							"?\n"
								"	Note that zim files may be very large and thus it can be expensive to download one over the mobile network. "
								"  You should consider download from a desktop system"
								"  and transfer the file later to the memory card of your phone."));
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
	
	optionsMenu = new QMenu(tr("Options", "Option menu"));
	optionsMenu->addAction(zoomInAction);
	optionsMenu->addAction(zoomOutAction);
	optionsMenu->addAction(toggleFullScreenAction);

	menuBar()->addMenu(optionsMenu);
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
		
	menuBar()->addMenu(optionsMenu);
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
						Qt::Key_Left, Qt::NoModifier, false, 1);
				QApplication::sendEvent(ui.articleName, remappedKeyEvent);
				break;
			case Qt::Key_Right:
				remappedKeyEvent = new QKeyEvent(QEvent::KeyPress,
						Qt::Key_Right, Qt::NoModifier, false, 1);
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
	settings.beginGroup("UISettings");
	if ((!settings.contains("fullScreen")) || (settings.value("fullScreen",
			false).toBool() != isFullScreen()))
		{
		settings.setValue("fullScreen", isFullScreen());
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
	settings.beginGroup("UISettings");
	if ((!settings.contains("zoomLevel"))
			|| (settings.value("zoomLevel").toInt() != zoomLevel))
		{
		settings.setValue("zoomLevel", zoomLevel);
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
