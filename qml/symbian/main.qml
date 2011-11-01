
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
        //onDepthChanged: searchBar.close();
    }

    StatusBar {
        id: statusBar

        anchors { top: parent.top; left: parent.left; right: parent.right }
        //opacity: largeImagePage.chromeOpacity
    }

/*    SearchBar {
        id: searchBar

        anchors.top: statusBar.bottom
        width: statusBar.width
        onSearchTagChanged: photoFeedModel.tags = searchTag
    }*/

    ToolBar {
        id: toolBar

        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
        //opacity: largeImagePage.chromeOpacity
    }

    LibraryPage {
        id: libraryPage

        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }
        /*inPortrait: window.inPortrait
        model: PhotoFeedModel {
            id: photoFeedModel
        }*/
        tools: ToolBarLayout {
            ToolButton {
                //iconSource: "images/tb_back.svg"
                text: "Exit" //Strange, not icon available
                onClicked: Qt.quit();
            }
            ToolButton {
                iconSource: "toolbar-refresh"
                onClicked: {
                   // photoFeedModel.reload();
                 //   searchBar.close();
                }
            }
            ToolButton {
                iconSource: "toolbar-search"
                onClicked: pageStack.push(indexPage);
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

        /*

        onPhotoClicked: {
              largeImagePage.setPhotoData(url, photoWidth, photoHeight);
            detailsPage.setPhotoData(author, date, description, tags, title,
                                     photoWidth, photoHeight);
            pageStack.push(indexPage);
        }*/
    }

    IndexPage {
        id: indexPage

        tools: ToolBarLayout {
            ToolButton {
                iconSource: "toolbar-back"
                onClicked: pageStack.pop();
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

        tools: ToolBarLayout {
            ToolButton {                
                iconSource: "toolbar-back"
                onClicked: pageStack.pop();
            }
            ToolButton {
                iconSource: "toolbar-previous"
                onClicked: articlePage.backward();
            }
            ToolButton {
                iconSource: "toolbar-next"
                onClicked: articlePage.forward();
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

/*
    Splash {
        id: splash

        image : "images/splash.png"
        timeout: 1000
        fadeout: 700
        Component.onCompleted: splash.activate();
        onFinished: splash.destroy();
    }*/

    Component.onCompleted: pageStack.push(libraryPage);
}


