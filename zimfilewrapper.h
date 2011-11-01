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

#ifndef ZIMFILEWRAPPER_H
#define ZIMFILEWRAPPER_H


#include <QObject>

#include <zim/zim.h>
#include <zim/fileiterator.h>
#include <QPixmap>
#include <QFile>

//TODO define implictly shared class?
class ZimFileWrapper : public QObject
{
    Q_OBJECT
private:
    zim::File* zimFile;
    bool valid;
    QString errorStr;

public:
    explicit ZimFileWrapper(QObject *parent = 0);
    ~ZimFileWrapper();
    bool isValid() {return valid;}
Q_INVOKABLE QString errorString() {return errorStr;}
Q_INVOKABLE bool openZimFile(QString zimFileName);
    zim::File::const_iterator getArticleByUrl(QString articleUrl,QChar nameSpace='A', bool closestMatchIfNotFound=true);

    QString getArticleTextByTitle(QString articleTitle);
    QPixmap getImageByUrl(QString imageUrl, QSize newSize=QSize());
    QString getMetaDataString(QString key);
    std::pair<bool, QString> getMetaData(QString key);

    QString getArticleTitleByUrl(QString articleUrl);
    QString getArticleTextByUrl(QString articleUrl);
    QByteArray getUUID();
    QString getFilename();
    //TODO: should no expose the zim file iterators, but encapsulate them instead
    zim::File::const_iterator end();
    int getNamespaceCount(QChar nameSpace);
    zim::File::const_iterator findByTitle(QChar nameSpace, QString articleTitle);
    std::pair<bool, zim::File::const_iterator> findxByTitle(QChar nameSpace, QString articleTitle);
    zim::File::const_iterator begin();
    zim::File::const_iterator beginByTitle();
    QString fromUTF8EncodedStdString(std::string s) {
        return QString::fromUtf8(s.data(), int(s.size()));
    }

protected:

signals:

public slots:

};

#endif // ZIMFILEWRAPPER_H
