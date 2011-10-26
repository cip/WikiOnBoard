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

#include <QtGui/QApplication>
#include <qdeclarative.h>
#include "qmlapplicationviewer.h"
#include "wikionboard.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    //TODO use QDeclarativeExtensionPlugin instead? (probably not very useful, because QMLViewer anyway
    //does not display Symbian QML components
    qmlRegisterType<ZimFileWrapper>("WikiOnBoardModule", 1, 0, "ZimFileWrapper");
    qmlRegisterType<ArticleViewerQML>("WikiOnBoardModule", 1, 0, "ArticleViewerQML");
    qmlRegisterType<IndexListQML>("WikiOnBoardModule", 1, 0, "IndexListQML");

    QmlApplicationViewer viewer;
    viewer.setMainQmlFile(QLatin1String("qml/symbian/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
