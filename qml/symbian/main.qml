
import QtQuick 1.0
////import com.nokia.meego 1.0
import com.nokia.symbian 1.0
import com.nokia.extras 1.0
import "settings.js" as Settings

Window {
    id: window

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
        id: zimFileSelectPage
        onZimFileSelected: {
            console.log("zimFileSelected:"+file)
            libraryPage.addZimFile(file);
            pageStack.pop();
        }
        tools: ToolBarLayout {
            ToolButton {
                iconSource: "toolbar-back"
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
                text: "Exit" //Strange, not icon available
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
            console.log("Open zimfile:"+fileName);
            if (backend.openZimFile(fileName)) {
                pageStack.push(indexPage);
            } else {
                var s = "Error opening zim file: "+fileName+" Error: "+backend.errorString()
                banner.showMessage(s)
                console.log(s)
            }
        }
    }

    IndexPage {
        id: indexPage

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

    ArticlePage {
        id: articlePage

        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }

        onBackwardAvailable: {
            console.log("onBackwardAvailable. Set backwardButton enabled to : "+available);
            backwardButton.enabled = available;
        }

        onForwardAvailable: {
            console.log("onForwardAvailable. Set forwardButton enabled to : "+available);
            forwardButton.enabled = available;
        }

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
                //onClicked:
            }
        }
    }

    Backend {
        id: backend
    }

    Component.onCompleted: {
        Settings.initialize();
        Settings.setSetting("test","value");
        console.log("Storage:" + Settings.getSetting("test"));

        pageStack.push(libraryPage);
    }
}


