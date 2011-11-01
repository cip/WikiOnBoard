
import QtQuick 1.0
////import com.nokia.meego 1.0
import com.nokia.symbian 1.0
import com.nokia.extras 1.0

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

    LibraryPage {
        id: libraryPage

        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }

        tools: ToolBarLayout {
            ToolButton {
                text: "Exit" //Strange, not icon available
                onClicked: Qt.quit();
            }
            ToolButton {
                iconSource: "toolbar-refresh"
                onClicked: {
                    //TODO, questionable whether useful at all
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

    Component.onCompleted: pageStack.push(libraryPage);
}


