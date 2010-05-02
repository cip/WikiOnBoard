/*
 * WikiBrowser.cpp
 *
 *  Created on: 28.03.2010
 *      Author: Christian
 */

#include "WikiBrowser.h"

WikiBrowser::WikiBrowser(QWidget *parent)
	{
	/*	Q_D(QWikiBrowser);
	    d->init();
	*/}

WikiBrowser::~WikiBrowser()
	{
	// TODO Auto-generated destructor stub
	}

QVariant QTextBrowser::loadResource(int /*type*/, const QUrl &name)
{
//TODO
	return QTextBrowser::loadResource(0, name);
}
