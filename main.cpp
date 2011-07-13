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


#include "wikionboard.h"

#include <QtGui>
#include <QApplication>

#if defined(Q_OS_SYMBIAN)
#include <eikbtgpc.h>       // symbian: LIBS += -lavkon -leikcoctl
#include <QSymbianEvent>
#include <w32std.h>
#endif

class MyApplication : public QApplication
	{
public:
	MyApplication(int argc, char** argv) :
		QApplication(argc, argv)
		{
		}
	void setBgc(void* bgc)
		{
		m_bgc = bgc;
		}
#ifdef Q_OS_SYMBIAN
protected:
	bool symbianEventFilter(const QSymbianEvent* symbianEvent)
		{
		const TWsEvent *event = symbianEvent->windowServerEvent();

		if (!event)
			{
			return false;
			}

		switch (event->Type())
			{
			case EEventWindowVisibilityChanged:
				{
				//Repaint, else in fullscreen mode
				// menus disappear after app has been switched to background.
				// Note: Using focus event instead appearantly does not work.
				// TODO: Consider only repainting when changed to visible.
				CEikButtonGroupContainer* bgc =
										(CEikButtonGroupContainer*) m_bgc;
								if (bgc)
									{
									bgc->MakeVisible(ETrue);
									bgc->DrawNow();
									}
								
				break;
				}
			case EEventFocusGained:
				{
				break;
				}
			case EEventFocusLost:
				{
				break;
				}
			default:
				break;
			}

		// Always return false so we don't stop
		// the event from being processed
		return false;
		}

#endif // Q_OS_SYMBIAN
private:
	void* m_bgc;
	};

int main(int argc, char *argv[])
	{
	MyApplication a(argc, argv);

	//Load translation
	QTranslator translatorDefault;
	QTranslator translatorLocale;
		
	
	//First load english tranlsations. (replaces translation from source code, to separate text and code more)
	if (translatorDefault.load(QLatin1String("wikionboard_en"))) {
		qDebug() <<"Installing translator for english. (Replaces (some) texts from source code)";
		a.installTranslator(&translatorDefault);			
	} else {
		qWarning() << "Loading translation file for english failed. Will use text from source code instead, which may be less complete/polished";
	}	
	QString locale = QLocale::system().name(); 				
	//Load tranlsation for local language, if available.
	//Note: If translator.load does not find locale file, it automatically strips local name and tries again.
	//  E.g. If wikionboard_de_AT.qsf does not exist, it tries wikionboard_de.qsf next. 
	//Note: In case of english locale the same translator is installed twice. This should however
	// not have a negative impact.
	if (translatorLocale.load(QLatin1String("wikionboard_") + locale)) {
		qDebug() << "Installing transloator for locale: "<<locale; 
		a.installTranslator(&translatorLocale);					
	} else {
		qDebug() << "Loading translation file for locale " << locale << " failed. (english translator is used instead)";
	}		     
	//Workaround for now softkeys in fullscreen mode
	// See: http://discussion.forum.nokia.com/forum/showthread.php?t=192624
	// In QT 4.6.3 this has been fixed. (new  Qt::WindowSoftkeysVisibleHint flag.
	// See main and http://bugreports.qt.nokia.com/browse/QTBUG-5171
	// However, workaround kept as hopefully still working with 4.6.2
#if defined(Q_OS_SYMBIAN) 
	CEikButtonGroupContainer* bgc = CEikButtonGroupContainer::Current();
#else
	void* bgc = 0;
#endif

#ifdef Q_OS_SYMBIAN
	a.setBgc(bgc);
	//a.setNavigationMode(Qt::NavigationModeCursorAuto);
	a.setNavigationMode(Qt::NavigationModeNone);
	// TODO: set navigationmode none as soon as compl. useably by keypad.
	// In particular note that with CursorAuto select key does not work. (because it is
	// like a clicke. This is very stupid for textBrowser if links are selected. (perhaps
	// switch navimode off just for this widget as well)
	//a.setNavigationMode(Qt::NavigationModeNone);
#endif

	WikiOnBoard w(bgc);
	return a.exec();
	}
