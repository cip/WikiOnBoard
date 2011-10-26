import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
     id: article
/*
     property string photoAuthor
     property string photoDate
     property string photoDescription
     property string photoTags
     property string photoTitle
     property int photoHeight
     property int photoWidth

     function setPhotoData(author, date, description, tags, title, photoWidth, photoHeight) {
         details.photoAuthor = author;
         details.photoDate = date;
         details.photoDescription = description;
         details.photoTags = tags;
         details.photoTitle = title;
         details.photoHeight = photoHeight;
         details.photoWidth = photoWidth;
     }
*/
     ArticleViewerQML {
        id: articleViewerQML
        anchors.fill: parent
     }

     Component.onCompleted: {
         console.log("onCompleted ArticlePage. Set zimfilewrapper")
         articleViewerQML.setZimFileWrapper(backend.getZimFileWrapper())
     }

     function openArticle(articleUrl) {
         console.log("in ArticlePage openArticle. Url:"+articleUrl)
         articleViewerQML.openArticle(articleUrl)
     }
     /*
     Flickable {

         id: flickable

         width: parent.width; height: parent.height
         contentWidth: width
         contentHeight: content.childrenRect.height + UI.MARGIN_XLARGE * 2
         flickableDirection: Flickable.VerticalFlick

         Column {
             id: content

             anchors.top: parent.top
             anchors.left: parent.left
             anchors.right: parent.right
             anchors.margins: UI.MARGIN_XLARGE
             spacing: UI.DETAILS_SPACING

         }
     }*/
 }
