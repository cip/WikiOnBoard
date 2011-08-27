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
    ArticleViewer(QWidget* parent = 0, ZimFileWrapper* zimFileWrapper = 0);
  //  ~ArticleViewer() {//TODO};
    QVariant loadResource ( int type, const QUrl & name );
private:
    ZimFileWrapper* zimFileWrapper;
    bool showImages;
public slots:
    void toggleImageDisplay(bool checked);

};

#endif // ARTICLEVIEWER_H
