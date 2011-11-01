import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
     id: index     
     signal openArticle(string articleUrl)

     function init() {
        indexListQML.setZimFileWrapper(backend.getZimFileWrapper())
     }

     Rectangle {
         anchors { fill: parent; bottomMargin: parent.height-articleName.height }
         TextField {
            id: articleName
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            onTextChanged: {
                console.log("TODO:Update search: "+text)
                indexListQML.searchArticle(text)
            }            
         }
     }
     onStatusChanged: {
         console.log("IndexPage onStatusChanged: status"+status)
         if (PageStatus.Activating == status) {
             console.log("IndexPage onStatusChanged: PageStatus.Activating")
             indexListQML.searchArticle(articleName.text)
         }
     }

     Rectangle {
        anchors { fill: parent; topMargin: articleName.height }
        IndexListQML {
            id: indexListQML
            anchors { fill: parent}
            onOpenArticle: {
                 index.openArticle(articleUrl);
             }
        }

     }
 }
