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
            platformLeftMargin: search.width + platformStyle.paddingSmall
            platformRightMargin: clearText.width + platformStyle.paddingMedium * 2
            onInputMethodHintsChanged: console.log("inputMethodHints:" +inputMethodHints)
            onFocusChanged: console.log("focus: "+focus)
            onTextChanged: {
                console.log("TODO:Update search: "+text)
                indexListQML.searchArticle(text)
            }
            Image {
                    id: search
                    anchors { top: parent.top; left: parent.left; margins: platformStyle.paddingMedium }
                    smooth: true
                    fillMode: Image.PreserveAspectFit
                    source: visual.searchToolbarIconSource
                    height: parent.height - platformStyle.paddingMedium * 2
                    width: parent.height - platformStyle.paddingMedium * 2
                }
            Image {
                     anchors { top: parent.top; right: parent.right; margins: platformStyle.paddingMedium }
                     id: clearText
                     fillMode: Image.PreserveAspectFit
                     smooth: true; visible: articleName.text
                     source: visual.removeToolbarIconSource
                     height: parent.height - platformStyle.paddingMedium * 2
                     width: parent.height - platformStyle.paddingMedium * 2

                     MouseArea {
                         id: clear
                         anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
                         height: articleName.height; width: articleName.height
                         onClicked: {
                             articleName.text = ""
                             articleName.forceActiveFocus()
                         }
                     }
                 }
         }
     }
     onStatusChanged: {
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
