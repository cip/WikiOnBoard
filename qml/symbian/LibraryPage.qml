import QtQuick 1.1
////import com.nokia.meego 1.0

import com.nokia.symbian 1.1

import "UIConstants.js" as UI

WikionboardPage {
    id: library
    signal openZimFile(string fileName);
    signal findEbookClicked();
    signal downloadEbookClicked();
    signal showAboutClicked();

    function addZimFile(fileName) {
        return libraryModel.addZimFile(fileName);
    }

    LibraryModel {
        id: libraryModel
    }

    function openMenu() {
        menu.open()
    }

    Menu {
        id: menu
        // define the items in the menu and corresponding actions
        content: MenuLayout {
            AboutMenuItem {}
            ExitMenuItem {}
        }
    }

    Component {
        id: listHeading
        ListHeading {
            width: parent.width
            ListItemText {
                anchors.fill: parent.paddingItem
                role: "Heading"
                text: "eBook Library"
            }
        }
    }
    Component {
        id: listDelegate



        //
        ListItem {
            id: listItem
            //Not sure whether really correct size, but looks fine
            height: zimFileItem.implicitHeight+platformStyle.borderSizeMedium
            Column {
                id: zimFileItem
                anchors.fill: listItem.paddingItem
                ListItemText {
                    function hasTitle() {
                        //FIXME, do this in a better way.
                        return !(title == "Not available" || title == "")
                    }

                    function getTitle() {
                        if (!hasTitle()) {
                            //Show filename (without path) if not title defined
                            return fileName.split('\\').pop().split('/').pop();
                        } else {
                            return title
                        }
                    }
                    elide: hasTitle()?Text.ElideRight:Text.ElideMiddle
                    width: parent.width
                    mode: listItem.mode
                    role: "Title"
                    text: getTitle()
                }
                ListItemText {
                    mode: listItem.mode
                    elide: Text.ElideRight
                    width: parent.width
                    role: "SubTitle"
                    text: {return language+", "+date}
                }
                ListItemText {
                    mode: listItem.mode
                    elide: Text.ElideRight
                    width: parent.width
                    role: "SubTitle"
                    text: creator
                }
                ListItemText {
                    mode: listItem.mode
                    elide: Text.ElideRight
                    width: parent.width
                    role: "SubTitle"
                    text: description
                }
                ListItemText {
                    mode: listItem.mode
                    elide: Text.ElideMiddle
                    width: parent.width
                    role: "SubTitle"
                    text: fileName
                }
            }
            onClicked: {
                library.openZimFile(fileName)
            }
            onPressAndHold: {
                console.log("in pressAndHold: index "+index)
                libraryItemMenu.openMenu(index)
            }
        }
    }

    ContextMenu {
           id: libraryItemMenu
           property int itemIndex
           itemIndex: -1
           MenuLayout {
               MenuItem {
                   text: "Remove"
                   onClicked: {
                       console.log(" Remove item "+libraryItemMenu.itemIndex)
                       libraryModel.remove(libraryItemMenu.itemIndex)
                    }
                }
               MenuItem {
                   text: "Remove All"
                   onClicked: {
                       console.log(" Remove all items");
                       libraryModel.clear();
                    }
               }
           }

           function openMenu(index) {
                   itemIndex = index
                   console.log("in openMenu: index "+itemIndex);
                   open();
            }
    }


    ListView {
        id: libraryListView
        //anchors.horizontalCenter: parent.horizontalCenter
        anchors { fill: parent; bottomMargin: parent.height*1/5}
        clip: true
        delegate: listDelegate
        model: libraryModel
        header: listHeading
    }

    Column {
        anchors.top: libraryListView.bottom
        anchors.topMargin: UI.MARGIN_XLARGE
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right:  parent.right

        Button {
            id: addEBookButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Find eBook on Phone")
            onClicked: findEbookClicked()
        }
        Button {
            id: downloadEBookButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Download eBook")
            onClicked: downloadEbookClicked()
        }        
    }
}
