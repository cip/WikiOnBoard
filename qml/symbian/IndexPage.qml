import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI

WikionboardPage {
     id: index

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
/*
             DetailText {
                 text: "<big><b>" + details.photoTitle + "</b></big>"
             }

             DetailText {
                 text: "<big><b>" + qsTr("Description") + "&nbsp;</b></big>" + details.photoDescription
             }

             DetailText {
                 text: "<big><b>" + qsTr("Author") + "&nbsp;</b></big>" + details.photoAuthor
             }

             DetailText {
                 text: "<big><b>" + qsTr("Published") + "&nbsp;</b></big>" + details.photoDate
             }

             DetailText {
                 text: "<big><b>" + qsTr("Size") + "&nbsp;</b></big>" + details.photoWidth + qsTr("x")
                       + details.photoHeight
             }

             DetailText {
                 text: "<big><b>" + qsTr("Tags") + "&nbsp;</b></big>" + details.photoTags
             }*/
         }
     }
 }
