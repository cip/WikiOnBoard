import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
     id: index     
     signal openArticle(string articleUrl)
     Rectangle {
           anchors { fill: parent; bottomMargin: 4*parent.height/5 }
        TextInput {
            id: articleName
            anchors {fill:parent;}
         }
     }
     Rectangle {
        anchors { fill: parent; topMargin: parent.height/5 }
        IndexListQML {
            anchors { fill: parent}
            onOpenArticle: {
                 index.openArticle(articleUrl);
             }
        }
     }
 }
