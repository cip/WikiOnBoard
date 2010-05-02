/*
 * WikiBrowser.h
 *
 *  Created on: 28.03.2010
 *      Author: Christian
 */
#include <qtextbrowser>
#ifndef WIKIBROWSER_H_
#define WIKIBROWSER_H_

class WikiBrowser : public QTextBrowser
	{
	   Q_OBJECT

public:
	WikiBrowser(QWidget *parent);
	virtual ~WikiBrowser();
	};

#endif /* WIKIBROWSER_H_ */
