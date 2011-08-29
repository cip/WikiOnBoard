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

#ifndef ARTICLEVIEWER_H
#define ARTICLEVIEWER_H
#include <QtGui/QTextBrowser>
#include "zimfilewrapper.h"

class ArticleViewer : public QTextBrowser
{
    Q_OBJECT
public:
    ArticleViewer(QWidget* parent = 0, ZimFileWrapper* zimFileWrapper = 0, bool hasTouchScreen = true);
    QVariant loadResource ( int type, const QUrl & name );
    QSize getMaximumDisplaySizeInCurrentArticleForImage(QString imageUrl);
    void moveTextBrowserTextCursorToVisibleArea();
    void openArticleByUrl(QUrl url);
    const QUrl welcomeUrl;
    void setWelcomePage(QString welcomePage);
protected:
    void hideWaitCursor();
    void showWaitCursor();
private:
    ZimFileWrapper* zimFileWrapper;
    QString articleTitle;
    bool hasTouchScreen;
    bool showImages;
    int zoomLevel;
    QString welcomePage;
signals:
    void openExternalLink( QUrl url);
    void articleOpened(QString articleTitle);
public slots:
    void onAnchorClicked (QUrl url);
    void onSourceChanged (QUrl url );

    void toggleImageDisplay(bool checked);
    void zoom(int zoomDelta);
    void zoomOutOneStep();
    void zoomInOneStep();

};



#endif // ARTICLEVIEWER_H
