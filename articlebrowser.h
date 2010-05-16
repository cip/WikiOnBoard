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

#include <QtCore>
#include <QtGui>

#include "flickable.h"

class ArticleBrowser : public QTextBrowser, public Flickable
{
    Q_OBJECT

public:
    ArticleBrowser(QWidget *parent = 0)
            : QTextBrowser(parent) {
        m_offset = 0;
        m_height = QFontMetrics(font()).height() + 5;
        m_highlight = -1;
        m_selected = -1;
     
        //setAttribute(Qt::WA_OpaquePaintEvent, true);
       // setAttribute(Qt::WA_NoSystemBackground, true);

        setMouseTracking(true);
        Flickable::setAcceptMouseClick(this);
    }

protected:
    // reimplement from Flickable
    virtual QPoint scrollOffset() const {
        //return QPoint(0, this->scrollOm_offset);
		  return QPoint(0,this->verticalScrollBar()->value());
    }

    // reimplement from Flickable
    virtual void setScrollOffset(const QPoint &offset) {
        /*int yy = offset.y();
        if (yy != m_offset) {
            m_offset = qBound(0, yy, m_height * m_colorNames.count() - height());
            update();
        }*/
        this->verticalScrollBar()->setValue(offset.y());
    }

protected:
  /*  void paintEvent(QPaintEvent *event) {
        /*QPainter p(this);
        p.fillRect(event->rect(), Qt::white);
        int start = m_offset / m_height;
        int y = start * m_height - m_offset;
        if (m_offset <= 0) {
            start = 0;
            y = -m_offset;
        }
        int end = start + height() / m_height + 1;
        if (end > m_colorNames.count() - 1)
            end = m_colorNames.count() - 1;
        for (int i = start; i <= end; ++i, y += m_height) {

            p.setBrush(Qt::NoBrush);
            p.setPen(Qt::black);
            if (i == m_highlight) {
                p.fillRect(0, y, width(), m_height, QColor(0, 64, 128));
                p.setPen(Qt::white);
            }
            if (i == m_selected) {
                p.fillRect(0, y, width(), m_height, QColor(0, 128, 240));
                p.setPen(Qt::white);
            }

            p.drawText(m_height + 2, y, width(), m_height, Qt::AlignVCenter, m_colorNames[i]);

            p.setPen(Qt::NoPen);
            p.setBrush(m_firstColor[i]);
            p.drawRect(1, y + 1, m_height - 2, m_height - 2);
            p.setBrush(m_secondColor[i]);
            p.drawRect(5, y + 5, m_height - 11, m_height - 11);
        }
        p.end();
    }
*/
    
    void mousePressEvent(QMouseEvent *event) {
        Flickable::handleMousePress(event);
/*        if (event->isAccepted())
            return;

        if (event->button() == Qt::LeftButton) {
            int y = event->pos().y() + m_offset;
            int i = y / m_height;
            if (i != m_highlight) {
                m_highlight = i;
                m_selected = -1;
                update();
            }
            event->accept();
        }*/
    }

    void mouseMoveEvent(QMouseEvent *event) {
        Flickable::handleMouseMove(event);
    }
    void mouseReleaseEvent(QMouseEvent *event) {
        Flickable::handleMouseRelease(event);
/*        if (event->isAccepted())
            return;

        if (event->button() == Qt::LeftButton) {
            m_selected = m_highlight;
            event->accept();
            update();
        }*/
    }

private:
    int m_offset;
    int m_height;
    int m_highlight;
    int m_selected;
    QStringList m_colorNames;
    QList<QColor> m_firstColor;
    QList<QColor> m_secondColor;
};
/*
#include "main.moc"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ArticleBrowser list;
    list.setWindowTitle("Kinetic Scrolling");
#ifdef Q_OS_SYMBIAN
    list.showMaximized();
#else
    list.resize(360, 640);
    list.show();
#endif

    return app.exec();
}
*/
