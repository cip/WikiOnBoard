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
    QString errorString() {return errorStr;}
    bool openZimFile(QString zimFileName);
    zim::File::const_iterator getArticleByUrl(QString articleUrl,QChar nameSpace='A', bool closestMatchIfNotFound=true);

    QString getArticleTextByTitle(QString articleTitle);
    QPixmap getImageByUrl(QString imageUrl);
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
protected:
    QString fromUTF8EncodedStdString(std::string s) {
        return QString::fromUtf8(s.data(), int(s.size()));
    }

signals:

public slots:

};

#endif // ZIMFILEWRAPPER_H
