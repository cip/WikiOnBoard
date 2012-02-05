//SYMBIAN_SPECIFIC Equivalent file is called main.qml for harmattan

import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.symbian 1.1

import com.nokia.extras 1.1
import "settings.js" as Settings

Item {


    id: window

    function init() {
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

    //HARMATTAN_ONLY, for symbian loaded in main.qml
    //Visual {
    //    id: visual
    //}

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
        //SYMBIAN_SPECIFIC On harmattan use ToolIcon instead
            //(with iconId instead of iconSource)
        ToolButton {
            iconSource: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }

    ToolBarLayout {
        id: defaultTools
        //SYMBIAN_SPECIFIC On harmattan use ToolIcon instead
            //(with iconId instead of iconSource)
        ToolButton {
            id: backButton
            iconSource: "toolbar-back"
            enabled: false
            // Hide button completely if not on articlePage
            // (I.p. as On Symbian disabled buttons not grayed out)
            visible: tabGroup.currentTab == articlePage
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
                //SYMBIAN_SPECIFIC On harmattan no such pre-defined icon available.
                iconSource: "toolbar-home"
            }
            TabButton {
                id: indexTabButton
                tab: indexPage
                //SYMBIAN_SPECIFIC On harmattan no such pre-defined icon available.
                iconSource: "toolbar-search"
            }
            TabButton {
                id: articleTabButton
                tab: articlePage
                iconSource: visual.documentToolbarIconSource
            }
        }
        //SYMBIAN_SPECIFIC On harmattan use ToolIcon instead
        //(with iconId instead of iconSource)
        ToolButton {
            iconSource: "toolbar-view-menu";
            onClicked: {
                tabGroup.currentTab.openMenu()
            }
        }
    }

    ToolBar {
        id: sharedToolBar
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
   //FIXME: No toolbar displayed without tools. (try removal if lib page added, else keep in symbian as well)
        tools: defaultTools
    }

    Loader {
        id: openExternalLinkQueryDialog
        //Note: Don't use anchors.fill: parent here, as
        //  else dialog is (nearly) full screen instead
        //  having it's height depending on content.
        source: ""
        property url url;
        property bool isLinkInArticle;
        function askAndOpenUrlExternally(urlA, isLinkInArticleA) {
            url = urlA;
            isLinkInArticle = isLinkInArticleA;
            source = "OpenExternalLinkQueryDialog.qml"
        }

        onLoaded: {
            //Actually works (at least in simulator)
            //as well if open() called directly after
            //setting source, but doing it here appears
            //to be cleaner.
            item.url = url
            item.isLinkInArticle = isLinkInArticle
            item.open();
        }

        function closed() {
            // Closed is called by OpenExternalLinkQueryDialog
            // when dialog is closed
            console.log("OpenExternalLinkQueryDialog close(). Unload it")
            source = "";
        }
    }

    TabGroup {
        id: tabGroup
        anchors {
            //fill: parent; topMargin: statusBar.height; bottomMargin: sharedToolBar.height
            //Should be equivalent, but as trial...
            left: parent.left;
            right: parent.right;
            top: statusBar.bottom;
            bottom: sharedToolBar.top
        }

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
                openExternalLinkQueryDialog.askAndOpenUrlExternally(url, true);
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
    Component.onCompleted: init()
}


