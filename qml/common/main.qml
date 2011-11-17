
import QtQuick 1.1
////import com.nokia.meego 1.0
import com.nokia.symbian 1.1
import com.nokia.extras 1.1
import "settings.js" as Settings


Window {
    id: window
    function openZimFile(fileName) {
        if (fileName!="") {
            console.log("Open zimfile:"+fileName);
            if (backend.openZimFile(fileName)) {
                Settings.setSetting("lastZimFile",fileName);
                indexTabButton.enabled = true
                tabGroup.currentTab = indexPage;
                buttonRow.checkedButton = indexTabButton
            } else {
                var s = "Error opening zim file: "+fileName+" Error: "+backend.errorString()
                banner.showMessage(s)
                console.log(s)
            }
        }
    }

    function closeZimFile() {
        backend.closeZimFile();
        Settings.setSetting("lastZimFile","");
        indexTabButton.enabled = false;
        articleTabButton.enabled =  false;
    }

    Visual {
        id: visual
    }

    StatusBar {
        id: statusBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
    }

    // Create an info banner with no icon
    InfoBanner {
        id: banner
        text: ""
        function showMessage(msg) {
            text = msg
            open()
        }
    }

    ToolBarLayout {
        id: backOnlyTools
        ToolButton {
            iconSource: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }

    ToolBarLayout {
        id: defaultTools
        ToolButton {
            id: backButton
            iconSource: "toolbar-back"
            enabled: false
            onClicked: {
                    if (tabGroup.currentTab == articlePage) {
                        articlePage.backward();
                    }
            }
        }

        ButtonRow {
            id: buttonRow
            TabButton {
                id: libraryTabButton
                tab: mainPage
                iconSource: "toolbar-home"
            }
            TabButton {
                id: indexTabButton
                tab: indexPage
                iconSource: "toolbar-search"
            }
            TabButton {
                id: articleTabButton
                tab: articlePage
                iconSource: visual.documentToolbarIconSource
            }
        }
        ToolButton {
            iconSource: "toolbar-menu"
            onClicked: {
                tabGroup.currentTab.openMenu()
            }
        }
    }

    ToolBar {
        id: sharedToolBar
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
    }


    QueryDialogWrapMode {
        //TODO should probably be handled by loader. (Or directly dynamically)
        id: openExternalLinkQueryDialog
        property url url
        icon: visual.internetToolbarIconSource
        titleText: qsTr("Open link in browser")
        //To ensure url is wrappd. QueryDialogWrapMode is just a copy of the symbian component,
        // but with message wrapMode exposed.
        messageWrapMode: Text.WrapAtWordBoundaryOrAnywhere
        //TODO: Would be nice if the self-signed string can be displayed as well
        message: qsTr("[TRANSLATOR] Explain that link \"%1\" clicked in article is not contained in ebook and needs to be opened in webrowser. Ask if ok.\n%2").replace(
                     "%1", url).replace(
                     "%2", appInfo.isSelfSigned?qsTr("[TRANLATOR]Explain that may not work if browser running.","only displayed if self_signed"):"")
        acceptButtonText: qsTr("Open")
        rejectButtonText: qsTr("Cancel")
        onClickedOutside: reject()
        onAccepted:  {
            if (!Qt.openUrlExternally(url)) {
                banner.showMessage(qsTr("Opening link \"%1\" in system web browser failed.").replace("%1",url));
            }
        }
        function askAndOpenUrlExternally(url) {
            openExternalLinkQueryDialog.url = url
            open();
        }
    }

    TabGroup {
        id: tabGroup
        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: sharedToolBar.height }


        Page {
            id: mainPage
            anchors { fill: parent}

            function openMenu() {
                libraryPage.openMenu()
            }

            PageStack {
                id: pageStack

                anchors.fill: parent

                //Appearantly not working as expected, see workaround
                //in onDepthChanged
                toolBar: sharedToolBar

                LibraryPage {
                    id: libraryPage
                    anchors { fill: parent}

                    tools: defaultTools

                    onFindEbookClicked: {
                        pageStack.push(Qt.resolvedUrl("ZimFileSelectPage.qml"))
                    }

                    onOpenZimFile: {
                        window.openZimFile(fileName)
                    }
                    onAboutZimFile: {
                        console.log("onAboutZimFile: "+fileName)
                        pageStack.push(Qt.resolvedUrl("AboutZimFilePage.qml"),{ fileName: fileName })
                    }

                    onDownloadEbookClicked: {
                        pageStack.push(Qt.resolvedUrl("HelpPage.qml"))
                    }

                }
                onDepthChanged: {
                    if (depth==1) {
                        //Necessary, as else when returning to library
                        // page from subpage (like help page) toolbar
                        // is empty. (Unclear why, because setting "tools"
                        // property in libraryPage should handle this)
                       sharedToolBar.setTools(defaultTools)
                    }

                }

                Component.onCompleted: {
                    pageStack.push(libraryPage)
                }
            }


        }


        IndexPage {
            id: indexPage
            anchors { fill: parent}
            //Note: Setting tools here propbably does  not really have an effect.
            // (Seems to be because its not a pagestack but a TabGroup)
            tools: defaultTools

            onOpenArticle: {
                //TODO: make sense to have one general function
                console.log("Item clicked in index list"+articleUrl+ "Open in articlePage")
                articlePage.openArticle(articleUrl)
                articleTabButton.enabled = true
                tabGroup.currentTab = articlePage;
                buttonRow.checkedButton = articleTabButton
            }

        }

        ArticlePage {
            id: articlePage
            anchors { fill: parent}
            onOpenExternalLink: {
                //TODO ask banner.showMessage("Open url "+url+" in system web browser.");
                openExternalLinkQueryDialog.askAndOpenUrlExternally(url);
            }

            onBackwardAvailable: {
                console.log("onBackwardAvailable. Set backwardButton enabled to : "+available);
                backButton.enabled = available;
            }

            onShowImagesChanged: Settings.setSetting("showImages",showImages);
            onZoomLevelChanged: Settings.setSetting("zoomLevel", zoomLevel);

            //Note: Setting tools here propbably does  not really have an effect.
            // (Seems to be because its not a pagestack but a TabGroup)
            tools: defaultTools
            onStatusChanged: {
                if (status == PageStatus.Activating) {
                    backButton.enabled = isBackwardAvailable()
                } else if (status == PageStatus.Deactivating) {
                    //Other pages currently don't have back
                    backButton.enabled= false;
                }
            }
        }
    }

    Backend {
        id: backend
    }

    Component.onCompleted: {
        //Sets zimFileWrapper in this components
        articlePage.init();
        indexPage.init();
        //Enabled as soon as as zim file respectively article openend
        indexTabButton.enabled = false
        articleTabButton.enabled = false
        Settings.initialize();        
        var lastZimFile =  Settings.getSetting("lastZimFile");
        if (lastZimFile != "Unknown") {
            console.log("Setting lastZimFile:"+lastZimFile+" open it.")
            window.openZimFile(lastZimFile)
        }
        var showImages = Settings.getSetting("showImages");
        if (showImages != "Unknown") {
            console.log("Setting showImages is "+showImages+". set articlePage showImages accordingly")
            articlePage.showImages = showImages;
        } else {
            articlePage.showImages = true;
        }
        var zoomLevel = Settings.getSetting("zoomLevel");
        if (zoomLevel != "Unknown") {
            console.log("Setting zoomLevel is "+zoomLevel+". set articlePage zoomLevel accordingly")
            articlePage.zoomLevel = zoomLevel;
        } else {
            articlePage.zoomLevel = 0;
        }


    }
}


