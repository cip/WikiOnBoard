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

#ifndef INDEXLISTQML_H
#define INDEXLISTQML_H

#include <QtGui/QGraphicsProxyWidget>
#include <QUrl>
#include <qdebug>
#include "zimfilewrapper.h"
#include "indexlist.h"

class 	IndexListQML : public QGraphicsProxyWidget
{
    Q_OBJECT

public:
    IndexListQML(QGraphicsItem* parent = 0)
        : QGraphicsProxyWidget(parent)
    {

        widget = new IndexList(0, 0);
        widget->setAttribute(Qt::WA_NoSystemBackground);
        //Prevent focusing, because keyboard events handled in qml
        // If focus not  disabled here, after touching list,
        // no text entry via keypad possible, as focus changed.
        //Drawback:virtual keyboard not closed anymore when clicking
        // on list
        setFocusPolicy(Qt::NoFocus);
        #if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
        #else
            //On harmattan index font size way to small,
            // therefore increase font size
            widget->setFont(QFont(QLatin1String(""),20));
        #endif
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

    void openCurrentEntry() {
        itemClicked(widget->currentItem());
    }
    void selectPreviousEntry() {
        widget->articleListSelectPreviousEntry();
    }
    void selectNextEntry() {
        widget->articleListSelectNextEntry();
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
    IndexList *widget;

};

#endif // INDEXLISTQML_H
