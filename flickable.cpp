/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Qt Software Information (qt-info@nokia.com)
**
** This file is part of the Graphics Dojo project on Qt Labs.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 or 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.fsf.org/licensing/licenses/info/GPLv2.html and
** http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at qt-sales@nokia.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "flickable.h"

#include <QtCore>
#include <QtGui>

class FlickableTicker: QObject
{
public:
    FlickableTicker(Flickable *scroller) {
        m_scroller = scroller;
    }

    void start(int interval) {
        if (!m_timer.isActive())
            m_timer.start(interval, this);
    }

    void stop() {
        m_timer.stop();
    }

protected:
    void timerEvent(QTimerEvent *event) {
        Q_UNUSED(event);
        m_scroller->tick();
    }

private:
    Flickable *m_scroller;
    QBasicTimer m_timer;
};

class FlickablePrivate
{
public:
    typedef enum {
        Steady,
        Pressed,
        ManualScroll,
        AutoScroll,
        Stop
    } State;

    State state;
    int threshold;
    QPoint pressPos;
    QPoint offset;
    QPoint delta;
    QPoint speed;
    FlickableTicker *ticker;
    QTime timeStamp;
    QWidget *target;
    QList<QEvent*> ignoreList;
};

Flickable::Flickable()
{
    d = new FlickablePrivate;
    d->state = FlickablePrivate::Steady;
    d->threshold = 10;
    d->ticker = new FlickableTicker(this);
    d->timeStamp = QTime::currentTime();
    d->target = 0;
}

Flickable::~Flickable()
{
    delete d;
}

void Flickable::setThreshold(int th)
{
    if (th >= 0)
        d->threshold = th;
}

int Flickable::threshold() const
{
    return d->threshold;
}

void Flickable::setAcceptMouseClick(QWidget *target)
{
    d->target = target;
}

static QPoint deaccelerate(const QPoint &speed, int a = 1, int max = 64)
{
    int x = qBound(-max, speed.x(), max);
    int y = qBound(-max, speed.y(), max);
    x = (x == 0) ? x : (x > 0) ? qMax(0, x - a) : qMin(0, x + a);
    y = (y == 0) ? y : (y > 0) ? qMax(0, y - a) : qMin(0, y + a);
    return QPoint(x, y);
}

void Flickable::handleMousePress(QMouseEvent *event)
{
    event->ignore();

    if (event->button() != Qt::LeftButton)
        return;

    if (d->ignoreList.removeAll(event))
        return;

    switch (d->state) {

    case FlickablePrivate::Steady:
        event->accept();
        d->state = FlickablePrivate::Pressed;
        d->pressPos = event->pos();
        break;

    case FlickablePrivate::AutoScroll:
        event->accept();
        d->state = FlickablePrivate::Stop;
        d->speed = QPoint(0, 0);
        d->pressPos = event->pos();
        d->offset = scrollOffset();
        d->ticker->stop();
        break;

    default:
        break;
    }
}

void Flickable::handleMouseRelease(QMouseEvent *event)
{
    event->ignore();

    if (event->button() != Qt::LeftButton)
        return;

    if (d->ignoreList.removeAll(event))
        return;

    QPoint delta;

    switch (d->state) {

    case FlickablePrivate::Pressed:
        event->accept();
        d->state = FlickablePrivate::Steady;
        if (d->target) {
            QMouseEvent *event1 = new QMouseEvent(QEvent::MouseButtonPress,
                                                  d->pressPos, Qt::LeftButton,
                                                  Qt::LeftButton, Qt::NoModifier);
            QMouseEvent *event2 = new QMouseEvent(*event);
            d->ignoreList << event1;
            d->ignoreList << event2;
            QApplication::postEvent(d->target, event1);
            QApplication::postEvent(d->target, event2);
        }
        break;

    case FlickablePrivate::ManualScroll:
        event->accept();
        delta = event->pos() - d->pressPos;
        if (d->timeStamp.elapsed() > 100) {
            d->timeStamp = QTime::currentTime();
            d->speed = delta - d->delta;
            d->delta = delta;
        }
        d->offset = scrollOffset();
        d->pressPos = event->pos();
        if (d->speed == QPoint(0, 0)) {
            d->state = FlickablePrivate::Steady;
        } else {
            d->speed /= 4;
            d->state = FlickablePrivate::AutoScroll;
            d->ticker->start(20);
        }
        break;

    case FlickablePrivate::Stop:
        event->accept();
        d->state = FlickablePrivate::Steady;
        d->offset = scrollOffset();
        break;

    default:
        break;
    }
}

void Flickable::handleMouseMove(QMouseEvent *event)
{
    event->ignore();

    if (!(event->buttons() & Qt::LeftButton))
        return;

    if (d->ignoreList.removeAll(event))
        return;

    QPoint delta;

    switch (d->state) {

    case FlickablePrivate::Pressed:
    case FlickablePrivate::Stop:
        delta = event->pos() - d->pressPos;
        if (delta.x() > d->threshold || delta.x() < -d->threshold ||
                delta.y() > d->threshold || delta.y() < -d->threshold) {
            d->timeStamp = QTime::currentTime();
            d->state = FlickablePrivate::ManualScroll;
            d->delta = QPoint(0, 0);
            d->pressPos = event->pos();
            event->accept();
        }
        break;

    case FlickablePrivate::ManualScroll:
        event->accept();
        delta = event->pos() - d->pressPos;
        setScrollOffset(d->offset - delta);
        if (d->timeStamp.elapsed() > 100) {
            d->timeStamp = QTime::currentTime();
            d->speed = delta - d->delta;
            d->delta = delta;
        }
        break;

    default:
        break;
    }
}

void Flickable::tick()
{
    if (d->state == FlickablePrivate:: AutoScroll) {
        d->speed = deaccelerate(d->speed);
        setScrollOffset(d->offset - d->speed);
        d->offset = scrollOffset();
        if (d->speed == QPoint(0, 0)) {
            d->state = FlickablePrivate::Steady;
            d->ticker->stop();
        }
    } else {
        d->ticker->stop();
    }
}
