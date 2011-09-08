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
#include <QDebug>
#include <QStringBuilder>
#include <QBuffer>
#include <QImageReader>
#include <QTime>
#include <stdexcept>

ZimFileWrapper::ZimFileWrapper(QObject *parent) :
    QObject(parent)
{
    zimFile = 0;
    valid = false;
    errorStr = QString();
}
ZimFileWrapper::~ZimFileWrapper(){
    delete zimFile;
}

bool ZimFileWrapper::openZimFile(QString zimFileName)
{
    std::string zimfilename;
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
        errorStr = QString::fromStdString(e.what());
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

QString ZimFileWrapper::getArticleTitleByUrl(QString articleUrl) {
    zim::File::const_iterator it = getArticleByUrl(articleUrl);
    if (it == zimFile->end()) return QString(tr("Error: article not found. (URL: %1 )").arg(articleUrl));
    return fromUTF8EncodedStdString(it->getTitle());
}


//Note: expects encoded URL (as used in articles). Therefore don't use
// this for decoded URL (as in zim file index)
QString ZimFileWrapper::getArticleTextByUrl(QString articleUrl)
{
    QString articleText = QLatin1String("ERROR");
    zim::Blob blob;
    try
    {
        zim::File::const_iterator it = getArticleByUrl(articleUrl);
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
        }
        else
        {
            blob = it->getData();
        }
        qDebug() << " Article (URL: "<< articleUrl << ", Size: "<<blob.size()<<") loaded from zim file";
        articleText = QString::fromUtf8(blob.data(), blob.size());
    }
    catch (const std::exception& e)
    {
        return QString::fromStdString(e.what());
    }

    return articleText;
}

//Note: expects encoded URL (as used in articles). Therefore don't use
// this for decoded URL (as in zim file index)
QPixmap ZimFileWrapper::getImageByUrl(QString imageUrl, QSize newSize)
{
    QTime timer;
    QTime subTimer;

    QPixmap image;
    zim::Blob blob;
    timer.start();
    try
    {
        //For images don't load closest match if url not found.
        subTimer.start();
        zim::File::const_iterator it = getArticleByUrl(imageUrl,QLatin1Char('I'),false);
        if (it == zimFile->end())
            throw std::runtime_error("image not found");
        if (it->isRedirect())
        {
            //Redirect stores decoded URLs. (as in index)
            // TODO: really necessary for images?
            std::string imageUrlDecodedStdStr = it->getRedirectArticle().getUrl();
            qDebug() << "Is redirect to url " << fromUTF8EncodedStdString(imageUrlDecodedStdStr);
            zim::File::const_iterator it1 = zimFile->find('I',
                                                          imageUrlDecodedStdStr);
            blob = it1->getData();
        }
        else
        {
            blob = it->getData();
        }
    }
    catch (const std::exception& e)
    {
        qDebug() << "Error in load image. Return null pixmap. ";
        return image;
    }
    qDebug() << " Image (URL: "<< imageUrl << ", Size: "<<blob.size()<<") loaded from zim file";
    qDebug() << "Loading image data" << imageUrl << " from zim file took" << subTimer.restart() << " milliseconds";
    QBuffer *imageBuffer = new QBuffer();
    imageBuffer->setData(blob.data(),blob.size());
    QImageReader *imageReader = new QImageReader(imageBuffer);
    qDebug() << "Original size of image: "<<imageReader->size();
    qDebug() << "Image format: "<<imageReader->format();
    qDebug() << " supports scaling: " << imageReader->supportsOption(QImageIOHandler::ScaledSize);
    if (newSize.isValid()) {
        //Resize to save memory.
        if ((newSize.height()==0)||(newSize.width()==0)) {
            image = QPixmap(1,1);
            image.fill();
            //Note that this branch currently is never triggered,
            // because ArticleViewer::getMaximumDisplaySizeInCurrentArticleViewer
            //  returns invalid size if height or width is 0. (This is because it cannot
            // distinguish between 0 size definition and missing size tag)
            qDebug() << "Size defined in HTML was 0. ("<<newSize << ". Return 1x1 pixel size instead to avoid repeated reload attempts";
        } else {
            if (imageReader->supportsOption(QImageIOHandler::ScaledSize)) {
                //For formats which support scaled reading (e.g. jpg), use it (as significantly faster than separate reading and scaling)
                qDebug() << " ScaledSize supported. Read with scaling";
                imageReader->setScaledSize(newSize);
                image = QPixmap::fromImageReader(imageReader);
            }
            else {
                // If format does not support scaling (e.g. png) read and scale separately.
                // benefit is that faster (lower quality) scaling can be used, while setScaledSize
                // would use high quality (=slower) scaling.
                // Note: If in future loading is implemented in separate thread,  would probably
                // make sense to use imagereader (high quality) only.
                qDebug() << " ScaledSize not supported. Read, and scale afterwards.";
                QTime subSubTimer;
                subSubTimer.start();
                image = QPixmap::fromImageReader(imageReader);
                qDebug() << "\tQPixmap::fromImageReader (without scaling) took: " << subSubTimer.restart();
                image=image.scaled(newSize,Qt::IgnoreAspectRatio,Qt::FastTransformation);
                qDebug() << "\tscaling took: " << subSubTimer.restart();
            }
            qDebug() << "Image resized to size defined in HTML\nsize of scaled image: "<<image.size();
        }
    } else {
        image = QPixmap::fromImageReader(imageReader);
        qWarning() << "image size not found. Don't resize image";
    }
    if ((image.isNull()))   {

        qWarning() << "loadFromData failed for image. Return 1x1 pixel image instead. QImageReader Error Message: "<< imageBuffer->errorString();
        image = QPixmap(1,1);
        image.fill();
    }
    delete imageReader;
    delete imageBuffer;
    qDebug() << " Creating Pixmap (including resize) from image data took" << subTimer.restart() << " milliseconds";
    qDebug() << "Loading image " << imageUrl <<" took" << timer.elapsed() << " milliseconds";
    return image;
}



QString ZimFileWrapper::getArticleTextByTitle(QString articleTitle)
{
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
    //Supported article urls are:
    // A/Url  (Expected by zimlib find(Url) )
    // /A/Url  (Appearanlty used by
    // Url  (Without namespace /A/.), assume it is article. (Either relative or other namespace)
    if (articleUrl.startsWith(QLatin1String("/")+nameSpace+QLatin1String("/"))) {
        strippedArticleUrl=articleUrl.remove(0, 3); //Remove /A/
        qDebug() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" starts with /"<< nameSpace << "/./"<< nameSpace << "/ refers to article name space.";

    } else if (articleUrl.startsWith(nameSpace+QLatin1String("/"))) {
        //TODO remove this when correct behavior clarified.
        strippedArticleUrl=articleUrl.remove(0, 2); //Remove /A
        qWarning() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" starts with "<< nameSpace << "/. Assume "<<nameSpace<<"/ refers to article name space. ";
    } else {
        strippedArticleUrl=articleUrl;
        qDebug() << "getArticleTextByUrl: articleUrl \""<<articleUrl<<"\" does not start with "<< nameSpace.toLatin1() <<"/ or /"<< nameSpace << "/. Assume it is a relative URL to "<< nameSpace << "/";
    }

    std::string articleUrlStdStr = std::string(strippedArticleUrl.toUtf8());
    std::string articleUrlDecodedStdStr = zim::urldecode(articleUrlStdStr);
    qDebug() << "Open article by URL.\n QString: " << articleUrl
             << "QString article namespace stripped:" << strippedArticleUrl
             << "\n std:string: " << fromUTF8EncodedStdString(articleUrlStdStr)
             << "\n decoded: " << fromUTF8EncodedStdString(
                    articleUrlDecodedStdStr);


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


