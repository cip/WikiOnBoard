#ifndef INDEXLIST_H
#define INDEXLIST_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QResizeEvent>
#include <zimfilewrapper.h>


enum ArticleListItemDataRole {
        ArticleUrlRole=Qt::UserRole,
        ArticleIndexRole,
        ArticleTitleRole
};

class IndexList : public QListWidget
{
    Q_OBJECT
public:
    explicit IndexList(QWidget *parent = 0, ZimFileWrapper* zimFileWrapper = 0, bool hasTouchScreen = true);
    std::pair<bool, QListWidgetItem *> getArticleListItem(zim::File::const_iterator it);
    void populateArticleList(QString articleName);
    void populateArticleList(QString articleName, int ignoreFirstN, bool direction_up, bool noDelete=false);
    void articleListSelectPreviousEntry();
    void articleListSelectNextEntry();
    QString articleListItemToString(QListWidgetItem *item);
    QUrl currentItemUrl();
    void resizeEvent ( QResizeEvent * event );
public slots:
    int addItemsToArticleList(bool up, int addCount=100, int maxCount=120);
private:
    ZimFileWrapper* zimFileWrapper;
    bool hasTouchScreen;
};

class ArticleListFilter : public QObject {
        Q_OBJECT
public:
    ArticleListFilter(QObject *parent=0) : QObject(parent) {}

protected:

    bool eventFilter(QObject *o, QEvent *e);
signals:
        bool approachingEndOfList(bool up);
};


#endif // INDEXLIST_H
