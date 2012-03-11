import QtQuick 1.1
import QtWebKit 1.0

//HARMATTAN_SPECIFIC. For symbian use: import com.nokia.symbian 1.1
import com.nokia.meego 1.0

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
     id: article
     //Alias showImages to menu. The C++ ArticleViewerQML binds to this as well.
     // (Change of setting in C++ would not visible here.)
     // Note that it is not possible to bind the other way around (or bidirectionally)
     // because showImages is set from javascript to load settings, which clears any binding)
     property alias showImages: showImageMenuItem.checked     
     //zoomLevel property. Same as for showImages, C++ ArticleViewerQML bound to this
     property int zoomLevel
     zoomLevel: 0

     signal backwardAvailable(bool available)
     signal forwardAvailable(bool available)
     signal openExternalLink(url url)
     function init() {
     //   articleViewerQML.setZimFileWrapper(backend.getZimFileWrapper())
     }

     function openMenu() {
         menu.open()
     }

     onZoomLevelChanged: {
         //Limit max/min zoom levels
         if (zoomLevel>5) {
             zoomLevel=5;
         } else if (zoomLevel<-5) {
             zoomLevel=-5;
         }
     }


     ContextMenu {
         id: textSizeMenu


         MenuLayout {
             MenuItemCheckable {
                 id: verySmall
                 text: qsTr("Very Small")
                 onClicked: zoomLevel = -2
             }
             MenuItemCheckable {
                 id: small
                 text: qsTr("Small")
                 onClicked: zoomLevel = -1
             }
             MenuItemCheckable {
                 id: normal
                 text: qsTr("Normal")
                 onClicked: zoomLevel = 0

             }
             MenuItemCheckable {
                 id: large
                 text: qsTr("Large")
                 onClicked: zoomLevel = 1
             }
             MenuItemCheckable {
                 id: veryLarge
                 text: qsTr("Very Large")
                 onClicked: zoomLevel = 2
             }
         }

         onStatusChanged: {
                // Note: also tried having "checked: (zoomLevel==<level>)"
                // in all MenuItemCheckables,but this didn't work (no unselection done)
                if (status==DialogStatus.Opening) {
                    normal.checked =  (zoomLevel ==0);
                    large.checked =  (zoomLevel == 1);
                    veryLarge.checked = (zoomLevel == 2)
                    small.checked =  (zoomLevel == -1);
                    verySmall.checked = (zoomLevel == -2);

                    //Other: (should not happen as always set here)
                    //     Nothing selected.
                }
         }
     }
     Menu {
             id: menu
             // define the items in the menu and corresponding actions
             content: MenuLayout {
                 ToolButton {
                     id: forwardButton
                     iconSource: "toolbar-next"
                     onClicked: {
                         articlePage.forward();
                     }
                 }
                 MenuItem {
                     text: qsTr("Text size")
                     //SYMBIAN_ONLY platformSubItemIndicator: true
                     onClicked: textSizeMenu.open()
                 }

                 MenuItemCheckable {
                     id: showImageMenuItem
                     text: qsTr("Show Images")
                 }
                 ExitMenuItem {}
             }
         }

     Flickable {
         id: flickable
         anchors.fill: parent
         contentWidth: Math.max(parent.width,articleViewer.width)
         contentHeight: Math.max(parent.height,articleViewer.height)
     WebView {
          id: articleViewer
          url: ""
          preferredWidth: flickable.width
          preferredHeight: flickable.height
          contentsScale: 1
          smooth: false
      }
     }

/*
     ArticleViewerQML {
        id: articleViewerQML
        anchors.fill: parent
        showImages: article.showImages
        zoomLevel: article.zoomLevel
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
*/

     function openArticle(articleUrl) {
         console.log("in ArticlePage openArticle. Url:"+articleUrl)
         //TODO: appending file:/// to article url seems to work, but
         // not really understood why, and may not work  in all cases.
         // (Just using articleUrl does not work, because it adds absolute path)
         var fileUrl = "file:///"+articleUrl;
         articleViewer.url=fileUrl;
     }

     function backward() {
         console.log("in ArticlePage backward")
         articleViewer.backward()
     }

     function forward() {
         console.log("in ArticlePage forward")
         articleViewer.forward()
     }

     function isBackwardAvailable() {
         return false;
     //    return articleViewer.isBackwardAvailable();
     }

     function isForwardAvailable() {
          return false;
       //  return articleViewer.isForwardAvailable();
     }

     onForwardAvailable: {
         console.log("onForwardAvailable. Set forwardButton enabled to : "+available);
         forwardButton.enabled = available;
     }

 }
