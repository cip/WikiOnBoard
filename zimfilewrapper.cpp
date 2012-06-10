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


#include "zimfilewrapper.h"
#include <zimreply.h>
#include <QDebug>
#include <QStringBuilder>
#include <QBuffer>
#include <QImageReader>
#include <QTime>
#include <stdexcept>
#include <QDirIterator>

ZimFileWrapper::ZimFileWrapper(QObject *parent) :
    QObject(parent)
{
    zimFile = 0;
    valid = false;
    m_errorStr = QString();    
    m_isTooLargeError  = false;
    //TODO: consider replacing with cleaner solution
    ZimReply::setZimFileWrapper(this);
}
ZimFileWrapper::~ZimFileWrapper(){
    delete zimFile;
    delete dirIterator;
}

bool ZimFileWrapper::openZimFile(QString zimFileName)
{
    QMutexLocker locker(&mutex);
    std::string zimfilename;
    m_errorStr = QString();
    emit errorStringChanged(m_errorStr);
    m_isTooLargeError = false;
    emit isTooLargeErrorChanged(m_isTooLargeError);
    try
    {
        //If zim file is split extension of first file is zimaa
        // zim::File however expects zim as extension, if it does
        // not find it, it tries zima
        // => Change extension in filename from zima to zim
        QRegExp rx(QLatin1String("(.*\\.zim)\\D\\D"));
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        zimFileName.replace(rx,QLatin1String("\\1"));
        //converts fileName to the local 8-bit encoding determined by the user's locale
        // On symbian it is probably encoded to UTF-8. As zimlib does not use qt open
        // file mechanism, opening files with non latin1 characters may not work on all platforms.
        qDebug() << "encoding filename using QFile::encodeName";
        zimfilename =  std::string(QFile::encodeName(zimFileName));
        zim::File* oldZimFile = zimFile;
        zimFile = new zim::File(zimfilename);
        //If opensuccesful, delete pointer to previously openend zim file.
        // If open fails keep previously opened zim file open.
        delete oldZimFile;
        valid = true;        
        return true;

    }
    catch (const std::exception& e)
    {
        m_errorStr = QString::fromStdString(e.what());
        emit errorStringChanged(m_errorStr);
        qDebug() << "Opening file "<<zimFileName<<" failed. error message: "<<m_errorStr<<" "<<e.what();
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
        QFile f(zimFileName);
        qDebug() << "Size of file: "<<f.size();
        if (f.size()<0) {
            m_isTooLargeError = true;
            emit isTooLargeErrorChanged(m_isTooLargeError);
        }

#endif

        return false;
    }
}


QString ZimFileWrapper::getFilename() {
    if (isValid()) {
        return QFile::decodeName(zimFile->getFilename().c_str());
    } else return QString();
}

int ZimFileWrapper::getNamespaceCount(QChar nameSpace) {
    return zimFile->getNamespaceCount(nameSpace.toLatin1());
}

int ZimFileWrapper::getNamespaceCount(QString nameSpace) {
    return zimFile->getNamespaceCount(nameSpace.at(0).toLatin1());
}


zim::File::const_iterator ZimFileWrapper::findByTitle(QChar nameSpace, QString articleTitle) {
    return findxByTitle(nameSpace,articleTitle).second;
}

std::pair<bool, zim::File::const_iterator> ZimFileWrapper::findxByTitle(QChar nameSpace, QString articleTitle) {
    //Zim index is UTF8 encoded. Therefore use utf8 functions to access
    // it.
    std::string articleTitleStdStr = std::string(
                    articleTitle.toUtf8());


    return zimFile->findxByTitle(nameSpace.toLatin1(), articleTitleStdStr);
}


zim::File::const_iterator ZimFileWrapper::begin() {
    if (isValid()) {
        return zimFile->begin();
    } else return zim::File::const_iterator();
}

zim::File::const_iterator ZimFileWrapper::beginByTitle() {
    if (isValid()) {
        return zimFile->beginByTitle();
    } else return zim::File::const_iterator();
}

zim::File::const_iterator ZimFileWrapper::end() {
    if (isValid()) {
        return zimFile->end();
    } else return zim::File::const_iterator();
}

QByteArray ZimFileWrapper::getUUID() {

    QByteArray uuidBA;
    if (isValid()) {
        uuidBA=QByteArray(zimFile->getFileheader().getUuid().data, zimFile->getFileheader().getUuid().size());
    }
    return uuidBA;
}

QString ZimFileWrapper::byteArray2HexQString(const QByteArray & byteArray)
{
    QString hexString;
    QTextStream textStream(&hexString);
    textStream << QLatin1String("0x")
             << hex << qSetFieldWidth(2) << qSetPadChar(QLatin1Char('0')) << left;
    for (int i = 0; i < byteArray.size(); i++)
    {
        textStream << static_cast<unsigned char>(byteArray.at(i));
    }
    return hexString;
}

QString ZimFileWrapper::getUUIDString() {
    return byteArray2HexQString(getUUID());
}

QString ZimFileWrapper::getArticleTitleByUrl(QString articleUrl) {
    zim::File::const_iterator it = getArticleByUrl(articleUrl);
    if (it == zimFile->end()) return QString(tr("Error: article not found. (URL: %1 )").arg(articleUrl));
    return fromUTF8EncodedStdString(it->getTitle());
}


//Note: expects encoded URL (as used in articles). Therefore don't use
// this for decoded URL (as in zim file index)
QPair<QByteArray, QString> ZimFileWrapper::getDataByUrl(QString articleUrl)
{
    QMutexLocker locker(&mutex);
    QByteArray data;
    QString mimeType;
    zim::Blob blob;

    try
    {
        //FIXME: for now don't return closest match
        zim::File::const_iterator it = getArticleByUrl(articleUrl,QLatin1Char('A'),false);
        //TODO: Actually not really clean, because if URL not found just closest match displayed.
        if (it == zimFile->end())
            throw std::runtime_error("article not found");
        if (it->isRedirect())
        {
            //Redirect stores decoded URLs. (as in index)
            std::string articleUrlDecodedStdStr = it->getRedirectArticle().getUrl();
            qDebug() << "Is redirect to url " << fromUTF8EncodedStdString(articleUrlDecodedStdStr);
            zim::File::const_iterator it1 = zimFile->find('A',
                                                          articleUrlDecodedStdStr);
            blob = it1->getData();
            mimeType = QString::fromStdString(it1->getMimeType());

        }
        else
        {
            blob = it->getData();
            mimeType = QString::fromStdString(it->getMimeType());
        }
        qDebug() << Q_FUNC_INFO << " Article (URL: "<< articleUrl << ", Size: "<<blob.size()<<", MimeType: "<<mimeType<<") loaded from zim file";
        //TODO: this copies data, which should be avoided
        data = QByteArray(blob.data(), blob.size());
    }
    catch (const std::exception& e)
    {
        return qMakePair(QString::fromStdString(e.what()).toUtf8(), QString(QLatin1String("text/plain")));
    }

    return qMakePair(data,mimeType);
}


QString ZimFileWrapper::getArticleTextByTitle(QString articleTitle)
{
    QMutexLocker locker(&mutex);
    QString articleText = QLatin1String("ERROR");
    zim::Blob blob;
    try
    {
        std::string articleNameStdStr = std::string(articleTitle.toUtf8());

        zim::File::const_iterator it = zimFile->findByTitle('A',
                                                            articleNameStdStr);
        if (it == zimFile->end())
            throw std::runtime_error("article not found");
        if (it->isRedirect())
        {
            articleNameStdStr = it->getRedirectArticle().getTitle();
            zim::File::const_iterator it1 = zimFile->find('A',
                                                          articleNameStdStr);
            blob = it1->getData();
        }
        else
        {
            blob = it->getData();
        }
        articleText = QString::fromUtf8(blob.data(), blob.size());
    }
    catch (const std::exception& e)
    {
        return QString::fromStdString(e.what());
    }

    return articleText;
}

//Article URL must be percent encoded.
// nameSpace should either be 'A' for Articles or 'I' for images.
zim::File::const_iterator ZimFileWrapper::getArticleByUrl(QString articleUrl,QChar nameSpace, bool closestMatchIfNotFound) {
    QString strippedArticleUrl;

    if (articleUrl == QLatin1String("") || articleUrl == QLatin1String("/")) {
        int mainPageIdx = zimFile->getFileheader().getMainPage();
        qDebug() << " url is "<< articleUrl << " -> Open main page. MainPageIdx "<< mainPageIdx;
        if (mainPageIdx != 0xffffffffff) {
            articleUrl = QString::fromStdString(zimFile->getArticle(mainPageIdx).getLongUrl());
            qDebug() << " main page url is: "<<articleUrl;
        } else {
            qDebug() << " No main page defined in zim file ";
        }

    }

    qDebug() << "Loading article from url: " << articleUrl ;

    //Supported article urls are:
    // A/Url  (Expected by zimlib find(Url) )
    // /A/Url  (Appearanlty used by
    // Url  (Without namespace /A/.), assume it is article. (Either relative or other namespace)
    if (articleUrl.startsWith(QLatin1String("/")+nameSpace+QLatin1String("/"))) {
        strippedArticleUrl=articleUrl.remove(0, 3); //Remove /A/
        qDebug() << Q_FUNC_INFO << ": articleUrl \""<<articleUrl<<"\" starts with /"<< nameSpace << "/./"<< nameSpace << "/ refers to article name space.";

    } else if (articleUrl.startsWith(QLatin1String("/")) && (articleUrl.at(2)==QLatin1Char('/'))) {
        //TODO: clean up (e.g. always use namespace in url)
        nameSpace =  articleUrl.at(1);
        strippedArticleUrl= strippedArticleUrl=articleUrl.remove(0, 3); //Remove /I/
        qDebug() << Q_FUNC_INFO << ": articleUrl \""<<articleUrl<<". Namespace in url: "+nameSpace.toLatin1();
    } else if (articleUrl.startsWith(nameSpace+QLatin1String("/"))) {
        //TODO remove this when correct behavior clarified.
        strippedArticleUrl=articleUrl.remove(0, 2); //Remove /A
        qWarning() <<Q_FUNC_INFO << ": articleUrl \""<<articleUrl<<"\" starts with "<< nameSpace << "/. Assume "<<nameSpace<<"/ refers to article name space. ";
    } else if (articleUrl.startsWith(QLatin1String("/"))) {
        // Workaround for welcomepage bug: after opening
        // welcomepage, links clicked in an article always contain welcome://.. in url.
        // as host/scheme is stripped before getArticleByUrl is called, this
        // is not visible here, except for an addional slash at the beginnig of the Url.
        // TODO: remove actually removes from articleUrl as well. As only
        // debug output is affected (actually shows stripped) but for all cases here,
        // this has not been fixed.
        strippedArticleUrl=articleUrl.remove(0, 1); //Remove /
        qDebug() << Q_FUNC_INFO <<": articleUrl \""<<articleUrl<<"\" starts with /, but does not contain expected namespace  "<< nameSpace.toLatin1() <<". Strip / from URL and assume remaining part is relative to nameSpace "<< nameSpace;
     } else {
        strippedArticleUrl=articleUrl;
        qDebug() << Q_FUNC_INFO <<": articleUrl \""<<articleUrl<<"\" does not start with "<< nameSpace.toLatin1() <<"/ or /"<< nameSpace << "/. Assume it is a relative URL to "<< nameSpace << "/";
    }

    std::string articleUrlStdStr = std::string(strippedArticleUrl.toUtf8());
    std::string articleUrlDecodedStdStr = zim::urldecode(articleUrlStdStr);
    qDebug() << "Open data by URL.\n QString: " << articleUrl
             << "QString article namespace stripped:" << strippedArticleUrl
             << "\n std:string: " << fromUTF8EncodedStdString(articleUrlStdStr)
             << "\n decoded: " << fromUTF8EncodedStdString(
                    articleUrlDecodedStdStr)
             << "\n nameSpace: " <<nameSpace;


    std::pair<bool, zim::File::const_iterator> r = zimFile->findx(nameSpace.toLatin1(), articleUrlDecodedStdStr);
    if (!r.first) {
        qWarning() << " article not found. URL encoded: "<< strippedArticleUrl << " decoded: "  << fromUTF8EncodedStdString(
                          articleUrlDecodedStdStr) << "\n";
        strippedArticleUrl.replace(QLatin1String("+"),QLatin1String("%2B"));
        articleUrlStdStr = std::string(strippedArticleUrl.toUtf8());
        articleUrlDecodedStdStr = zim::urldecode(articleUrlStdStr);
        qWarning() << " Try whether article for url without replacing + with spaces exists";
        qDebug() << "Original URL: " << articleUrl
                 << "Stripped URL, + replaced by %2B " << strippedArticleUrl
                 << "\n std:string: " << fromUTF8EncodedStdString(articleUrlStdStr)
                 << "\n decoded: " << fromUTF8EncodedStdString(
                        articleUrlDecodedStdStr);

        r = zimFile->findx('A', articleUrlDecodedStdStr);
        if (!r.first) {
            if (!closestMatchIfNotFound) {
                qWarning() << "Neither exists. closestMatchIfNotFound=false. Return zimFile->end()";
                return zimFile->end();
            }
            qWarning() << "Neither exists. closestMatchIfNotFound=true => Return closest match. (With + not replaced by spaces)";
        }
    }

    return r.second;
}

std::pair<bool, QString> ZimFileWrapper::getMetaData(QString key) {
    std::string keyStdStr = std::string(key.toUtf8());
    if (zimFile!=NULL) {
        std::pair<bool, zim::File::const_iterator> r = zimFile->findxByTitle('M', keyStdStr);
        if (r.first) {
            zim::Blob blob;
            zim::Article a =*r.second;
            blob = a.getData();
            return std::pair<bool, QString>(true,  QString::fromUtf8(blob.data(), blob.size()));
        } else {
            return std::pair<bool, QString>(false, QLatin1String(""));
        }
    } else {
        return std::pair<bool, QString>(false, QLatin1String(""));
    }
}

QString ZimFileWrapper::getMetaDataString(QString key) {
    std::pair<bool, QString> metaData = getMetaData(key);
    return metaData.first ? metaData.second : QString(tr("Not available"));
}

//TODO probably better to have separate class (like ZimFileIterator)
void ZimFileWrapper::zimFileIterator(QString path, bool recurseSubdirs) {
    delete dirIterator;
    if (recurseSubdirs) {
        dirIterator = new QDirIterator(path, QDirIterator::Subdirectories);
    } else {
        dirIterator = new QDirIterator(path);
    }
}

QString ZimFileWrapper::nextZimFile() {
    while (dirIterator->hasNext()) {
        dirIterator->next();
        if ((dirIterator->fileInfo().suffix().compare(QLatin1String("zim"),Qt::CaseInsensitive)==0)
                || (dirIterator->fileInfo().suffix().compare(QLatin1String("zimaa"),Qt::CaseInsensitive)==0)) {
            qDebug() << "zim file found: " << dirIterator->fileInfo().absoluteFilePath();
               return dirIterator->fileInfo().absoluteFilePath();
        }
    }
    return QString();
}



