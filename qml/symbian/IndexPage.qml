import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.1

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
            onInputMethodHintsChanged: console.log("inputMethodHints:" +inputMethodHints)
            onFocusChanged: console.log("focus: "+focus)
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

     function openMenu() {
         menu.open()
     }
     Menu {
         id: menu
         // define the items in the menu and corresponding actions
         content: MenuLayout {
             AboutMenuItem {}
             ExitMenuItem {}
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
