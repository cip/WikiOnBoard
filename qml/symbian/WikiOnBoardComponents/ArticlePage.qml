import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.symbian 1.1

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0
import Mediakey 1.0

WikionboardPage {
     id: article
     //Alias showImages to menu. The C++ ArticleViewerQML binds to this as well.
     // (Change of setting in C++ would not visible here.)
     // Note that it is not possible to bind the other way around (or bidirectionally)
     // because showImages is set from javascript to load settings, which clears any binding)
     property alias showImages: showImageMenuItem.checked     
     //zoomLevel property. Same as for showImages, C++ ArticleViewerQML bound to this
     property alias darkTheme: darkThemeMenuItem.checked
     property int zoomLevel
     zoomLevel: 0

     MediakeyCapture{
             onVolumeDownPressed: {
                 console.log('VOLUME DOWN PRESSED ')
                 articleViewerQML.pageDown()
             }
             onVolumeUpPressed: {
                 console.log('VOLUME UP PRESSED ')
                 articleViewerQML.pageUp()
             }
         }
     Keys.onUpPressed: {
             console.log("Key up pressed")
             articleViewerQML.pageUp()
             event.accepted = true
         }

     Keys.onDownPressed: {
             console.log("Key down pressed")
             articleViewerQML.pageDown()
             event.accepted = true
         }

     Keys.onVolumeUpPressed:  {
         console.log("Volume up pressed")
         articleViewerQML.pageUp()
         event.accepted = true


     }
     Keys.onVolumeDownPressed:  {
         console.log("Volume down pressed")
         articleViewerQML.pageDown()
         event.accepted = true


     }

     signal backwardAvailable(bool available)
     signal forwardAvailable(bool available)
     signal openExternalLink(url url)
     function init() {
        articleViewerQML.setZimFileWrapper(backend.getZimFileWrapper())
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

     onStatusChanged: {
         if (PageStatus.Active == status) {
             console.log("ArticlePage onStatusChanged: PageStatus.Active")
             forceActiveFocus()
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
                     //SYMBIAN_ONLY
                     platformSubItemIndicator: true
                     onClicked: textSizeMenu.open()
                 }
                 MenuItemCheckable {
                     id: darkThemeMenuItem
                     text: qsTr("Dark Theme")
                 }
                 MenuItemCheckable {
                     id: showImageMenuItem
                     text: qsTr("Show Images")
                 }
                 ExitMenuItem {}
             }
         }

     ArticleViewerQML {
        id: articleViewerQML
        anchors.fill: parent
        showImages: article.showImages
        zoomLevel: article.zoomLevel
        darkTheme: article.darkTheme
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

     onForwardAvailable: {
         console.log("onForwardAvailable. Set forwardButton enabled to : "+available);
         forwardButton.enabled = available;
     }

 }
