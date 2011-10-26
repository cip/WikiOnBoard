import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI

WikionboardPage {
    id: library
    signal openZimFile();
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


    ListModel {
        id: libraryModel

        ListElement {
            titleText: "Wikipedia DE"
            subTitleText: "Deutsche Wikipedia, 10.08.2010, 13455 Artikel"
        }
        ListElement {
            titleText: "Wikipedia EN"
            subTitleText: "English Wikipedia, 11.12.2009, 43455 Artikel"
        }
    }

    Component {
        id: listHeading
        ListHeading {
            width: parent.width
            ListItemText {
                anchors.fill: parent.paddingItem
                role: "Heading"
                text: "eBook Library"
            }
        }
    }
    Component {
        id: listDelegate



        //
        ListItem {
            id: listItem
            Column {
                anchors.fill: listItem.paddingItem
                ListItemText {
                    mode: listItem.mode
                    role: "Title"
                    text: titleText
                }
                ListItemText {
                    mode: listItem.mode
                    role: "SubTitle"
                    text: subTitleText
                }
            }
            onClicked: library.openZimFile()

        }
    }

    Flickable {
        id: flickable

        width: parent.width; height: parent.height
        contentWidth: width
        contentHeight: parent.childrenRect.height + UI.MARGIN_XLARGE * 2
        flickableDirection: Flickable.VerticalFlick
/*        Column {
          anchors.fill: parent
          spacing: UI.MARGIN_DEFAULT*/

        ListView {
            id: list
            //anchors.horizontalCenter: parent.horizontalCenter
            anchors { fill: parent; bottomMargin: parent.height/5 }
            //width: parent.width - parent.spacing; height: 120
            clip: true            
            delegate: listDelegate
            model: libraryModel
            header: listHeading
        }

        Button {
            id: addEBookButton
            anchors.top: list.bottom
            anchors.topMargin: UI.MARGIN_DEFAULT
            anchors.horizontalCenter: parent.horizontalCenter

            text: qsTr("Find eBook on Phone")
        }
        Button {
            id: downloadEBookButton            
            anchors.top: addEBookButton.bottom
            anchors.topMargin: UI.MARGIN_DEFAULT
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Download eBook")
        }
    //}
    }


    /*
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
