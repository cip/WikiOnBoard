import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
     id: article

     signal backwardAvailable(bool available)
     signal forwardAvailable(bool available)

     function init() {
        articleViewerQML.setZimFileWrapper(backend.getZimFileWrapper())
     }

     function openMenu() {
         articlePageMenu.open()
     }

     Menu {
             id: articlePageMenu
             // define the items in the menu and corresponding actions
             content: MenuLayout {
                 MenuItemCheckable {
                     text: qsTr("Show Images")
                     onClicked: console.log("show images")
                 }
             }
         }
     ArticleViewerQML {
        id: articleViewerQML
        anchors.fill: parent

        onBackwardAvailable: {
               console.log("onBackwardAvailable:"+available);
               article.backwardAvailable(available);
        }

        onForwardAvailable: {
               console.log("onForwardAvailable:"+available);
               article.forwardAvailable(available);
        }


     }

     function openArticle(articleUrl) {
         console.log("in ArticlePage openArticle. Url:"+articleUrl)
         articleViewerQML.openArticle(articleUrl)
     }

     function backward() {
         console.log("in ArticlePage backward")
         articleViewerQML.backward()
     }

     function forward() {
         console.log("in ArticlePage forward")
         articleViewerQML.forward()
     }

     function isBackwardAvailable() {
         return articleViewerQML.isBackwardAvailable();
     }

     function isForwardAvailable() {
         return articleViewerQML.isForwardAvailable();
     }


 }
