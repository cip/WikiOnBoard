import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
     id: article
     //Alias showImages to menu. The C++ ArticleViewerQML binds to this as well.
     // (Change of setting in C++ would not visible here.)
     // Note that it is not possible to bind the other way around (or bidirectionally)
     // because showImages is set from javascript to load settings, which clears any binding)
     property alias showImages: showImageMenuItem.checked
     signal backwardAvailable(bool available)
     signal forwardAvailable(bool available)
     signal openExternalLink(url url)
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
                     id: showImageMenuItem
                     text: qsTr("Show Images")
                 }
             }
         }
     ArticleViewerQML {
        id: articleViewerQML
        anchors.fill: parent
        showImages: article.showImages
        onBackwardAvailable: {
               console.log("onBackwardAvailable:"+available);
               article.backwardAvailable(available);
        }

        onForwardAvailable: {
               console.log("onForwardAvailable:"+available);
               article.forwardAvailable(available);
        }
        onOpenExternalLink: {
            console.log("onOpenExternalLink:"+url);
            article.openExternalLink(url);
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
