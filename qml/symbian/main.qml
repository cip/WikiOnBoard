
import QtQuick 1.1
////import com.nokia.meego 1.0
import com.nokia.symbian 1.1
import com.nokia.extras 1.1
import "settings.js" as Settings


Window {    
    id: window
    function openZimFile(fileName) {

        console.log("Open zimfile:"+fileName);
        if (backend.openZimFile(fileName)) {
            Settings.setSetting("lastZimFile",fileName);
            tabGroup.currentTab = indexPage;
        } else {
            var s = "Error opening zim file: "+fileName+" Error: "+backend.errorString()
            banner.showMessage(s)
            console.log(s)
        }
    }

    Visual {
        id: visual
    }

    StatusBar {
        id: statusBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
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
            iconSource: "toolbar-back"
            onClicked: pageStack.pop();
        }
        ButtonRow {
            id: buttonRow
            TabButton {
                id: libraryTabButton
                tab: mainPage
                iconSource: "toolbar-back" //TODO
            }
            TabButton {
                id: indexTabButton
                tab: indexPage
                iconSource: "toolbar-search"
            }
            TabButton {
                id: articleTabButton
                tab: articlePage
                iconSource: "toolbar-back"
            }
        }
        ToolButton {
            iconSource: "toolbar-menu"
            //onClicked:
        }
    }

    ToolBarLayout {
        id: articlePageTools
        ToolButton {
            iconSource: "toolbar-back"
            onClicked: articlePage.backward();
        }
    /*    ToolButton {
            id: backwardButton
            iconSource: "toolbar-previous"
            onClicked: {
                articlePage.backward();
            }
        }*/
        ButtonRow {
            id: buttonRow1
            TabButton {
                id: apLibraryTabButton
                tab: mainPage
                iconSource: "toolbar-back" //TODO
            }
            TabButton {
                id: apIndexTabButton
                tab: indexPage
                iconSource: "toolbar-search"
            }
            TabButton {
                id: apArticleTabButton
                tab: articlePage
                iconSource: "toolbar-back"
            }
        }

        ToolButton {
            iconSource: "toolbar-menu"
            onClicked: articlePage.openMenu()
            //onClicked:
        }


    }

    ToolBar {
        id: toolBar

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
        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }


        Page {
            id: mainPage
            anchors { fill: parent}

            PageStack {
                id: pageStack

                anchors.fill: parent
                toolBar: toolBar

                LibraryPage {
                    id: libraryPage
                    anchors { fill: parent}
                    function findZimFiles(path, recurseSubdirs) {
                        console.log("sendMessage")
                        //searchZimFileWorker.sendMessage({'action': 'appendCurrentTime', 'model': libraryModel});
                        //FIXME
                        backend.zimFileIterator(path, recurseSubdirs);
                        var zimFile;

                        //This does not work yet (not time to redraw..)
                        libraryPageBusyIndicator.visible = true
                        libraryPageBusyIndicator.running = true
                        while ((zimFile = backend.nextZimFile()) !== "" ) {
                            console.log("zimfile found:"+zimFile)
                            libraryPage.addZimFile(zimFile);
                            libraryPageBusyIndicator.running = true
                        }
                        console.log("finished adding zimfiles");
                        libraryPageBusyIndicator.running = false
                        libraryPageBusyIndicator.visible = false
                    }

                    WorkerScript {
                        id: searchZimFileWorker
                        source: "searchzimfiles.js"
                        //onMessage: {console.log("message received"+message)}
                    }

                    BusyIndicator {
                        anchors.centerIn: parent
                        id: libraryPageBusyIndicator
                        running: false
                        visible: false
                    }


                    tools: defaultTools

                    // Create an info banner with no icon
                    InfoBanner {
                        id: banner
                        text: ""
                        function showMessage(msg) {
                            text = msg
                            open()
                        }
                    }
                    onFindEbookClicked: {
                        pageStack.push(zimFileSelectPage)
                    }

                    onOpenZimFile: {
                        window.openZimFile(fileName)
                    }
                    onShowAboutClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
                    onDownloadEbookClicked: pageStack.push(Qt.resolvedUrl("HelpPage.qml"))
                    //onDepthChanged:
                }
                ZimFileSelectPage {
                    //TODO: Probably makes sense to use loader. (Else list populated on app start)
                    id: zimFileSelectPage
                    anchors { fill: parent}
                    onZimFileSelected: {
                        console.log("zimFileSelected:"+file)
                        if (libraryPage.addZimFile(file)) {
                            pageStack.pop();
                        } else {
                            var s = "Error adding zim file: "+file+" Error: "+backend.errorString()
                            banner.showMessage(s)

                        }
                    }
                    tools: ToolBarLayout {
                        ToolButton {
                            id: backButton
                            iconSource: "toolbar-back"
                            onClicked: zimFileSelectPage.isDriveSelection?pageStack.pop():zimFileSelectPage.folderUp()
                        }
                        ToolButton {
                            iconSource: visual.closeToolbarIconSource
                            onClicked: pageStack.pop();
                        }

                    }
                    onStatusChanged: {
                        if (status == PageStatus.Activating) {
                            //TODO?
                        } else if (status == PageStatus.Deactivating) {
                            toolBar.tools = defaultTools;
                        }
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
            tools: defaultTools

            onOpenArticle: {
                console.log("Item clicked in index list"+articleUrl+ "Open in articlePage")
                articlePage.openArticle(articleUrl)
                tabGroup.currentTab = articlePage;
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
                backwardButton.enabled = available;
            }

            onForwardAvailable: {
                console.log("onForwardAvailable. Set forwardButton enabled to : "+available);
                forwardButton.enabled = available;
            }
            onShowImagesChanged: Settings.setSetting("showImages",showImages);
            tools: articlePageTools
            onStatusChanged: {
                if (status == PageStatus.Activating) {
                    toolBar.tools = articlePageTools
                } else if (status == PageStatus.Deactivating) {
                    toolBar.tools = defaultTools;
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

    }
}


