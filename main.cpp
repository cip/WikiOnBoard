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
