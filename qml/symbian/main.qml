
import QtQuick 1.0
////import com.nokia.meego 1.0
import com.nokia.symbian 1.0

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
                text: "back"
                onClicked: Qt.quit();
            }
            ToolButton {
                //iconSource: "images/tb_reload.svg"
                text: "reload"
                onClicked: {
                   // photoFeedModel.reload();
                 //   searchBar.close();
                }
            }
            ToolButton {
                //iconSource: "images/tb_search.svg"
                text: "search"
                //onClicked: searchBar.toggle();
                onClicked: pageStack.push(indexPage);
            }
        }
        onOpenZimFile: {
            console.log("Open zimfile");
            pageStack.push(indexPage);
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
                //iconSource: "images/tb_back.svg"
                text: "back"
                onClicked: pageStack.pop();
            }
            ToolButton {
                //iconSource: "images/tb_info.svg"
                text: "info"
                checked: false
                onClicked: pageStack.replace(articlePage);
            }
        }
    }

    ArticlePage {
        id: articlePage

        anchors { fill: parent; topMargin: statusBar.height; bottomMargin: toolBar.height }

        tools: ToolBarLayout {
            ToolButton {
                //iconSource: "images/tb_back.svg"
                text: "back"
                onClicked: pageStack.pop();
            }
            ToolButton {
                //iconSource: "images/tb_info.svg"
                text: "info"
                checked: true
                onClicked: pageStack.replace(largeImagePage);
            }
        }
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
