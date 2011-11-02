
import QtQuick 1.0
////import com.nokia.meego 1.0
import com.nokia.symbian 1.0
import com.nokia.extras 1.0
import "settings.js" as Settings

Window {
    id: window
    function openZimFile(fileName) {

        console.log("Open zimfile:"+fileName);
        if (backend.openZimFile(fileName)) {
            Settings.setSetting("lastZimFile",fileName);
            pageStack.push(indexPage);
        } else {
            var s = "Error opening zim file: "+fileName+" Error: "+backend.errorString()
            banner.showMessage(s)
            console.log(s)
        }
    }

    Visual {
        id: visual
    }


    PageStack {
        id: pageStack

        anchors.fill: parent
        toolBar: toolBar
        //onDepthChanged:
    }

    StatusBar {
        id: statusBar
        anchors { top: parent.top; left: parent.left; right: parent.right }
    }

    ToolBar {
        id: toolBar

        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
    }

    ZimFileSelectPage {
        //TODO: Probably makes sense to use loader. (Else list populated on app start)
        id: zimFileSelectPage
        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }
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
    }

    LibraryPage {
        id: libraryPage

        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }

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

        tools: ToolBarLayout {
            ToolButton {
                id: exitButton
                iconSource: visual.closeToolbarIconSource
                onClicked: Qt.quit();
            }
            ToolButton {
                iconSource: "toolbar-refresh"
                onClicked: {
                    //FIXME (first windows, 2nd are symbian...)
                    libraryPage.findZimFiles("C:\\Users\\Christian\\Downloads\\",true);
                    libraryPage.findZimFiles("E:\\", false);
                    libraryPage.findZimFiles("F:\\", false);

                }
            }
            ToolButton {
                iconSource: "toolbar-search"
                onClicked: pageStack.push(indexPage);
            }
            ToolButton {
                iconSource: "toolbar-menu"
                //onClicked:
            }
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
        onFindEbookClicked: {
            pageStack.push(zimFileSelectPage)
        }

        onOpenZimFile: {
            window.openZimFile(fileName)
        }
    }

    IndexPage {
        id: indexPage
        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }
        tools: ToolBarLayout {
            ToolButton {
                iconSource: "toolbar-back"
                onClicked: pageStack.pop();
            }
            ToolButton {
                iconSource: "toolbar-menu"
                //onClicked:
            }
        }

        onOpenArticle: {
            console.log("Item clicked in index list"+articleUrl+ "Open in articlePage")
            articlePage.openArticle(articleUrl)
            pageStack.push(articlePage);
        }

    }

    QueryDialog {
        //TODO should probably be handled by loader. (Or directly dynamically)
        id: openExternalLinkQueryDialog
        property url url
        icon: visual.internetToolbarIconSource
        titleText: qsTr("Open link in browser")
        acceptButtonText: qsTr("Open")
        rejectButtonText: qsTr("Cancel")
        onClickedOutside: reject()
        onAccepted:  {
            if (!Qt.openUrlExternally(url)) {
                banner.showMessage(qsTr("Opening link \""+url+"\" in system web browser failed."));
            }
        }
        function askAndOpenUrlExternally(url) {
            //TODO: %1 translation scheme not working in qml?
            //TODO: Would be nice if the self-signed string can be displayed as well
            message = qsTr("[TRANSLATOR] Explain that link\""+url+"\" clicked in article is not contained in ebook and needs to be opened in webrowser. Ask if ok.")
            openExternalLinkQueryDialog.url = url
            open();
        }
    }

    ArticlePage {
        id: articlePage
        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }

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
        tools: ToolBarLayout {
            ToolButton {
                iconSource: "toolbar-back"
                onClicked: pageStack.pop();
            }
            ToolButton {
                id: backwardButton
                iconSource: "toolbar-previous"
                onClicked: {
                    articlePage.backward();
                }
            }
            ToolButton {
                id: forwardButton
                iconSource: "toolbar-next"
                onClicked: {
                    articlePage.forward();
                }
            }
            ToolButton {
                iconSource: "toolbar-menu"
                onClicked: articlePage.openMenu()
                //onClicked:
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
        pageStack.push(libraryPage);
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


