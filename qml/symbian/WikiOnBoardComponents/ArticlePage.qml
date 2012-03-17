import QtQuick 1.1
import QtWebKit 1.0

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

    property alias backwardAvailable: articleViewer.backwardAvailable
    property alias forwardAvailable: articleViewer.forwardAvailable

    onShowImagesChanged: articleViewer.setShowImages(showImages);


    //zoomLevel property. Same as for showImages, C++ ArticleViewerQML bound to this
    property int zoomLevel
    zoomLevel: 0

    MediakeyCapture{
        onVolumeDownPressed: {
            console.log('VOLUME DOWN PRESSED ')
            articleViewer.pageDown()
        }
        onVolumeUpPressed: {
            console.log('VOLUME UP PRESSED ')
            articleViewer.pageUp()
        }
    }
    Keys.onUpPressed: {
        console.log("Key up pressed")
        articleViewer.pageUp()
        event.accepted = true
    }

    Keys.onDownPressed: {
        console.log("Key down pressed")
        articleViewer.pageDown()
        event.accepted = true
    }

    Keys.onVolumeUpPressed:  {
        console.log("Volume up pressed")
        articleViewer.pageUp()
        event.accepted = true


    }
    Keys.onVolumeDownPressed:  {
        console.log("Volume down pressed")
        articleViewer.pageDown()
        event.accepted = true


    }

    signal backwardAvailable(bool available)
    signal forwardAvailable(bool available)
    signal openExternalLink(url url)
    function init() {
        //articleViewerQML.setZimFileWrapper(backend.getZimFileWrapper())
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
                enabled: false
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
                id: showImageMenuItem
                text: qsTr("Show Images")
            }
            ExitMenuItem {}
        }
    }

    BusyIndicator {
        id: busyIndicator
        running: false
        visible: false
        anchors.left: parent.left
        anchors.top: parent.top
        z: 10
    }

    Flickable {
        id: flickable
        anchors.fill: parent
        clip: true
        contentWidth: Math.max(parent.width,articleViewer.width)
        contentHeight: Math.max(parent.height,articleViewer.height)
        WebView {
            id: articleViewer
            property bool forwardAvailable
            property bool backwardAvailable
            //TODO: required? behavior at least slightly different
            // (e.g. if in scroll down bounds check removed, without
            //   anchors fill works fine, with anchors fill scrolls over.

            //anchors.fill: parent
            url: ""
            preferredWidth: flickable.width
            preferredHeight: flickable.height
            contentsScale: 1
            smooth: false
            settings.defaultFontSize:  18 + (zoomLevel)
            settings.minimumFontSize:  19
            function setShowImages(showImages) {
                console.log("setShowImages. showImages: "+showImages)
                settings.autoLoadImages = showImages;
                reload.trigger();
            }
            javaScriptWindowObjects: [
                QtObject {
                                        //Provide console log to javascript functionality
                                        // (Appearantly else at least on symbian console log
                                        //  produces not output)
                                        // Note that in cordova-qt there is a console plugin
                                        // with the same objective, but implementing
                                        // it here does not require adding js/plugin
                                        WebView.windowObjectName: "console"

                                        function log(msg) {
                                            console.log("[JSLOG] "+msg);
                                        }
                                    },
                QtObject {
                                        WebView.windowObjectName: "webView"
                                        function scrollTo(y) {
                                            console.log("[JSLOG] scrollTo y:"+y);
                                            flickable.contentY = y

                                        }
                                    }

            ]
            onLoadStarted: {
                console.log("articleViewer onLoadStarted. url: "+url);

                busyIndicator.running = true;
                busyIndicator.visible = true;
                updateBackwardForwardAvailable();

            }

            onLoadFinished: {
                console.log("articleViewer onLoadFinished. url: " +url);
                busyIndicator.running = false;
                busyIndicator.visible = false;
                updateBackwardForwardAvailable();
                patchAnchors();
            }



            onLoadFailed: {
                console.log("articleViewer onLoadFailed");
                busyIndicator.running = false;
                busyIndicator.visible = false;
                banner.showMessage(qsTr("Loading failed"));
                updateBackwardForwardAvailable();
            }

            function updateBackwardForwardAvailable() {
                backwardAvailable = articleViewer.back.enabled;
                forwardAvailable = articleViewer.forward.enabled;
            }

            function patchAnchors() {
                 var c= "\
                        function getOffset( el ) {\
                            var _x = 0;\
                            var _y = 0;\
                            while( el && !isNaN( el.offsetLeft ) && !isNaN( el.offsetTop ) ) {\
                                _x += el.offsetLeft - el.scrollLeft;\
                                _y += el.offsetTop - el.scrollTop;\
                                el = el.offsetParent;\
                            }                   \
                        return { top: _y, left: _x };\
                        }\
                        \
                        function scrollToLink() {\
                                console.log(this+'.onclick');\
                                var target = document.querySelector(this.hash); \
                                console.log('target:'+target);\
                                var targetOffset = getOffset(target);\
                                console.log('targetOffset.top:'+ targetOffset.top);\
                                event.preventDefault();\
                                webView.scrollTo(targetOffset.top);\
                        }\
                        var allLinks = document.querySelectorAll('a[href*=\"#\"]'); \
                        for (var i=0; i<allLinks.length; i++){\
                              allLinks[i].onclick = scrollToLink;\
                  }"
                var r= evaluateJavaScript(c);
                console.log("result: "+r+ " for javascript\n"+c);

            }

            function pageUp() {
                if (flickable.contentY == 0) {
                    return
                }
                var yv = flickable.contentY - flickable.height +
                        articleViewer.settings.defaultFontSize;
                if (yv < 0) {
                    yv = 0
                }
                flickable.contentY = yv
            }

            function pageDown() {
                if (flickable.contentY + flickable.height >= contentsSize.height) {
                    return
                }
                var yv = flickable.contentY + flickable.height -
                        articleViewer.settings.defaultFontSize;
                if (yv + flickable.height > contentsSize.height) {
                    yv = contentsSize.height - flickable.height
                }
                flickable.contentY = yv
            }
            //FIXME: implement external link functionality
            //onOpenExternalLink: {
            //    console.log("onOpenExternalLink:"+url);
            //    article.openExternalLink(url);
            //}

            // Without this stored images disabled not working.
            Component.onCompleted: {
                settings.autoLoadImages = article.showImages;
                updateBackwardForwardAvailable();
            }
        }
    }
    ScrollBar {
        flickableItem: flickable
        anchors { right: flickable.right; top: flickable.top }
    }

    function openArticle(articleUrl) {
        console.log("in ArticlePage openArticle. Url:"+articleUrl)
        //TODO: appending file:/// to article url seems to work, but
        // not really understood why, and may not work  in all cases.
        // (Just using articleUrl does not work, because it adds absolute path)
        var fileUrl = "file:///"+articleUrl;
        articleViewer.url=fileUrl;
    }

    function backward() {
        console.log("in ArticlePage backward. back.enabled: "+articleViewer.back.enabled);
        articleViewer.back.trigger();
    }

    function forward() {
        console.log("in ArticlePage forward. forward.enabled: "+articleViewer.forward.enabled);
        articleViewer.forward.trigger();
    }


    onForwardAvailableChanged: {
        console.log("onForwardAvailableChanged. Set forwardButton enabled to : "+ forwardAvailable);
        forwardButton.enabled = forwardAvailable;
    }

}
