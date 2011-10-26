import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
     id: index     
     signal openArticle(string articleUrl)
     Rectangle {
         anchors { fill: parent; bottomMargin: 6*parent.height/7 }
         TextField {
            id: articleName
            //TODO: for some reason halfoutside of screen if
            // verticalCenter: parent.verticalCenter.
            anchors.top: parent.verticalCenter
            width: parent.width
            onTextChanged: {
                console.log("TODO:Update search: "+text)
                indexListQML.searchArticle(text)
            }
         }
     }

     Rectangle {
        anchors { fill: parent; topMargin: parent.height/7 }
        IndexListQML {
            id: indexListQML
            anchors { fill: parent}
            onOpenArticle: {
                 index.openArticle(articleUrl);
             }
        }
     }
 }
