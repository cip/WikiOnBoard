#include "indexlist.h"
#include <QEvent>
//"Official" kinetic scrolling. (Backport from Qt 4.8)
//	See http://qt.gitorious.org/qt-labs/kineticscroller/commits/solution and
//		http://bugreports.qt.nokia.com/browse/QTBUG-9054?focusedCommentId=130700&page=com.atlassian.jira.plugin.system.issuetabpanels%3Acomment-tabpanel#action_130700
#include <QtScroller>
#include <qtscrollevent>
#include <QDebug>
#include <QScrollBar>
#include <QUrl>

//To update article list during scrolling.
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



IndexList::IndexList(QWidget *parent, ZimFileWrapper* zimFileWrapper, bool hasTouchScreen) :
    QListWidget(parent), zimFileWrapper(zimFileWrapper), hasTouchScreen(hasTouchScreen)
{    
    setStyleSheet(QLatin1String("QListWidget {background-color:white;color: black; selection-background-color: blue}"));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // ScrollPerPixel required for kinetic scrolling
    setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);

    QtScroller::grabGesture(viewport(), QtScroller::LeftMouseButtonGesture);

    ArticleListFilter *articleListFilter = new ArticleListFilter(this);
    viewport()->installEventFilter(articleListFilter);
    connect(articleListFilter,SIGNAL(approachingEndOfList(bool)),this, SLOT(addItemsToArticleList(bool)));

}


//For debug output
QString IndexList::articleListItemToString(QListWidgetItem* item) {
    QUrl url = item->data(ArticleUrlRole).toUrl();
    QString urlEncoded = QString::fromUtf8(url.toEncoded().data(),url.toEncoded().length());

    QString s = QString(QLatin1String("arcticleListItem: \n\tindex: %1\n"
                              "\ttitle: %2\n"
                        "\turl (encoded): %3\n")).arg(item->data(ArticleIndexRole).toString(),
                                                      item->data(ArticleTitleRole).toString(),
                                                      urlEncoded);
    return s;
}

std::pair <bool, QListWidgetItem*> IndexList::getArticleListItem(zim::File::const_iterator it) {
    QListWidgetItem* articleItem = new QListWidgetItem();
    if (it==zimFileWrapper->end()) {
        qDebug() << "getArticleListItem iterator points is end of article list. ";
        return std::pair<bool, QListWidgetItem*> (false, articleItem);
    }
    QString articleTitle = zimFileWrapper->fromUTF8EncodedStdString(it->getTitle());
    QUrl articleUrl = QUrl::fromEncoded(QUrl::toPercentEncoding(zimFileWrapper->fromUTF8EncodedStdString(it->getUrl())));
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

void IndexList::populateArticleList(QString articleName) {
        populateArticleList(articleName, 0, false);
}

void IndexList::populateArticleList(QString articleName, int ignoreFirstN,
                bool direction_up, bool noDelete)
        {
        qDebug() << "in populateArticleList. articleName:  " << articleName
                        << ". ignoreFirstN: " << ignoreFirstN << ". direction_up:"
                        << direction_up << ".noDelete: " << noDelete;
        if (zimFileWrapper->isValid())
                {
                try
                        {
                        //Find article, if not an exact match,  Iterator may point to end, or to
                        // element of other namespace. (like image (I) or metadata (M))
                        zim::File::const_iterator it = zimFileWrapper->findByTitle(QLatin1Char('A'),
                                        articleName);
                        if (((it==zimFileWrapper->end()) || (it->getNamespace() != 'A') )) {
                            qDebug() << " No valid article >= \"" << articleName << "\" found. Try using previous entry in zim file instead";
                            if (it==zimFileWrapper->begin()) {
                                    qDebug() << " zim file contains no entries. Add nothing to list";
                                    return;
                            }
                            --it;
                            if ((it==zimFileWrapper->end()) || (it->getNamespace() != 'A') ) {
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
                                        clear();
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
                                                insertItem(0, articleItemPair.second);
                                                insertedItemsCount++;
                                                if (noDelete==false) {
                                                        QListWidgetItem *lastItem = takeItem(count() - 1);
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
                                        if (it == zimFileWrapper->begin())
                                                break;
                                        --it;
                                        }
                                else
                                        {
                                        if (i >= ignoreFirstN)
                                                {
                                                addItem(articleItemPair.second);
                                                insertedItemsCount++;
                                                }
                                        if (it == zimFileWrapper->end())
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
                                        int itemHeight = visualItemRect(
                                                        item(0)).height();
                                        int
                                                        articleListWidgetHeight =
                                                                        maximumViewportSize().height();
                                        QListWidgetItem *bottomItem = item(
                                                        insertedItemsCount - 1);
                                        int
                                                        bottomItemBottom =
                                                                        visualItemRect(
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
                            setCurrentRow(addedItemsCount);
                            //Scroll to found item. (addItemsToArticleList scrolled to first added item, which is one too early)
                            scrollToItem (currentItem() , QAbstractItemView::PositionAtTop );



                            qDebug() << "First item of list (after add)" << articleListItemToString(item(0));
                            qDebug() << "Last item of list (after add)" << articleListItemToString(item(count()-1));
                          } else {
                            //Non-touchscreen
                            setCurrentRow(0); //Select first found item
                            }

                    }

                catch (const std::exception& e)
                        {
                        addItem(QLatin1String("Error occured"));
                        }

                }

        }

void IndexList::articleListSelectPreviousEntry()
        {
        //Actually forwarding key basically worked,
        // as well, but crashing when trying to select item outside of list. (pretty strange)
        //Anyway, for in future TODO planned to reload list when moving out of list.
        if (count() > 0)
                {
                if (currentRow() == 0)
                        {
                            if (hasTouchScreen) {
                                //To basically same thing if scrolled.
                                qDebug() << "up key while first entry selected. Add items.";
                                  addItemsToArticleList(true);
                            } else {
                                //Old behavior. (TODO: May make sense to change to similar scheme as
                                // used with touchscreen devices)
                               QListWidgetItem *item = currentItem();
                                if (item->data(ArticleIndexRole).toInt() > 0)
                                    {
                                    populateArticleList(item->data(ArticleTitleRole).toString(), 1,
                                                true);
                                    setCurrentRow(
                                                count() - 1);
                                    }
                            }
                        }
                else
                        {
                        setCurrentRow(
                                        currentRow() - 1);
                        }
                }
        }

void IndexList::articleListSelectNextEntry()
        {
        //Actually forwarding key basically worked,
        // as well, but crashing when trying to select item outside of list. (pretty strange)
        //Anyway, for in future TODO planned to reload list when moving out of list.
        if (count() > 0)
                {
                if (currentRow() == count()
                                - 1)
                        {
                         if (hasTouchScreen) {
                            //To basically same thing if scrolled.
                            qDebug() << "down key while last entry selected. Add items.";
                            addItemsToArticleList(false);
                         } else {
                            //TODO check outof bounds
                            QListWidgetItem *item = currentItem();
                            populateArticleList(item->data(ArticleTitleRole).toString(), 1,
                                        false);
                            setCurrentRow(0);
                         }
                        }
                else
                        {
                        setCurrentRow(
                                        currentRow() + 1);
                        }
                }
        }


//TODO Consider merging with regular populate Article List
//addCount: Number of article entries to be added (at most)
//maxCount: maximumCount of article entries allowed. Deletes items furthest away
//from added item for each item added
int IndexList::addItemsToArticleList(bool up, int addCount, int maxCount)
        {
    qDebug() << "WikiOnBoard::addItemsToArticleList (up:"<<up<<" addCount: "<<addCount <<" maxCount: "<<maxCount;
    if (zimFileWrapper->isValid())
                {
                try
                        {
                        if (count()==0) {
                            qWarning() << "articleList empty. addItemsToArticleList requires a least on article in the article list." ;
                            return 0;
                        }

                        if (up)
                                {
                                qDebug()
                                                << "AddItems to beginning of list";

                                QListWidgetItem * firstArticleInCurrentList = item(0);
                                QString
                                                titleFirstArticleInCurrentList =
                                                                firstArticleInCurrentList->data(
                                                                                ArticleTitleRole).toString();

                                qDebug() << " First article in current list is: "<< articleListItemToString(firstArticleInCurrentList);

                                std::pair<bool, zim::File::const_iterator> r =
                                                zimFileWrapper->findxByTitle(QLatin1Char('A'), titleFirstArticleInCurrentList);
                                if (!r.first)
                                        {
                                        qWarning()
                                                        << " No exact match found. (Only possible if bug in wikionboard)";
                                        }
                                zim::File::const_iterator it = r.second;
                                if (it == zimFileWrapper->beginByTitle())
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
                                        insertItem(0, articleItemPair.second);
                                        insertedItemsCount++;
                                        //Remove last item to avoid eating up to much memory.
                                        //(But ensure that that there enough "old" items left
                                        // to fill the article list. (first added new item
                                        // should be on top of list at the end)
                                        if (count()>maxCount) {
                                                QListWidgetItem *lastItem = takeItem(
                                                        count() - 1);
                                                delete lastItem;
                                        }
                                        //order is different
                                        if (it == zimFileWrapper->beginByTitle())
                                                {
                                                qDebug()
                                                                << "Beginning of title index reached. Stop adding titles. Last added title :"<< articleListItemToString(articleItemPair.second);
                                                break;
                                                }
                                        } //End while
                                if (insertedItemsCount>0) {
                                        QListWidgetItem *firstNewItem = item(insertedItemsCount-1);
                                        qDebug() << insertedItemsCount
                                                << " items inserted at beginning of list. Scroll so that firstly newly added article is at bottom of list. Firstly new added article: "<<articleListItemToString(firstNewItem);

                                        scrollToItem(firstNewItem,QAbstractItemView::PositionAtTop);
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
                                QListWidgetItem* lastArticleInCurrentList = item(
                                            count() - 1);
                                QString titleLastArticleInCurrentList =
                                                lastArticleInCurrentList->data(
                                                                ArticleTitleRole).toString();

                                qDebug() << " Last article in current list is: " << articleListItemToString(item(
                                                                                                                count() - 1));

                                std::pair<bool, zim::File::const_iterator> r =
                                                zimFileWrapper->findxByTitle(QLatin1Char('A'), titleLastArticleInCurrentList);
                                if (!r.first)
                                        {
                                        qWarning()
                                                        << " No exact match found. (Only possible if bug in wikionboard)";
                                        }
                                zim::File::const_iterator it = r.second;
                                if (it == zimFileWrapper->end())
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
                                        addItem(articleItemPair.second);
                                        //Remove first item to avoid eating up to much memory.
                                        //TODO: increase overlap. (User scrolls one direction than other else leads directly to reload)
                                        if (count()>maxCount) {
                                                QListWidgetItem *firstItem =
                                                        takeItem(0);
                                                delete firstItem;
                                        }
                                        insertedItemsCount++;
                                        if (it == zimFileWrapper->end())
                                                {
                                                qDebug()
                                                                << "End of title index reached. Stop adding titles. Last added title: "<< articleListItemToString(articleItemPair.second);
                                                break;
                                                }
                                        } //End while
                                        int indexFirstNewItem = count()-insertedItemsCount;
                                        if ((indexFirstNewItem >= 0) && (indexFirstNewItem
                                                < count()))
                                        {
                                                QListWidgetItem *firstNewItem = item(
                                                        indexFirstNewItem);
                                                qDebug() << insertedItemsCount
                                                        << " items appended to end of list. Scroll so that firstly newly added article is at bottom of list. Firstly new added article: "<<articleListItemToString(firstNewItem);
                                                scrollToItem(firstNewItem,
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
                        addItem(QLatin1String("Error occured"));
                        }
                } //End if (zimFile!=NULL)
                return 0;
        }

QUrl IndexList::currentItemUrl() {
    QListWidgetItem *item = currentItem();
    if (item != NULL)
        {
            QUrl url = item->data(ArticleUrlRole).toUrl();
            return url;
        }
    else {
        return QUrl();
    }
 }


void IndexList::resizeEvent(QResizeEvent * event)
{
    qDebug() << "resizeEvent:\n "
                "New size: width: " << event->size().width() <<" height: " << event->size().height() << ""
                "Old size: width: " << event->oldSize().width() <<" height: " << event->oldSize().height();
    qDebug() << "  indexList: width: " << this->width() << " height:" << this->height();
    if (count() > 0)
    {
        //Current item (if none first) is new first item of list.
        // TODO: Keeping offset would be nicer, but more complex
        //      in case new size is smaller than offset.
        int itemIndex = currentRow();
        if (itemIndex < 0) {
            itemIndex = 0;
        }
        if (item(itemIndex)->data(ArticleIndexRole).toInt() > 0)
        {
            populateArticleList(item(itemIndex)->data(ArticleTitleRole).toString(), 0,
                                false);
            //indexList->setCurrentRow(0);
        }
    }

}
