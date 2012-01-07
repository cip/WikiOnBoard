
import QtQuick 1.1

//HARMATTAN_SPECIFIC. For symbian use: import com.nokia.symbian 1.1
import com.nokia.meego 1.0

import "UIConstants.js" as UI

WikionboardPage {
    id: library
    signal openZimFile(string fileName);
    signal aboutZimFile(string fileName);
    signal findEbookClicked();
    signal downloadEbookClicked();

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
        ListItem {
            id: listItem
            //Not sure whether really correct size, but looks fine
            // HARMATTAN_SPECIFIC On symbian use "+ platformStyle.borderSizeMedium" instead 
            height: zimFileItem.implicitHeight+visual.borderSizeMedium

            function hasInfo(info) {
                //FIXME, do this in a better way.
                return (info !== undefined &&  info != "Not available" && info !== "")

            }
            Rectangle {
                anchors.fill:  parent
                color: fileName==backend.fileName?"blue":"transparent"


                Column {
                    id: zimFileItem

                    ListItemText {
                        function getTitle() {
                            if (!hasInfo(title)) {
                                //Show filename (without path) if not title defined
                                return fileName.split('\\').pop().split('/').pop();
                            } else {
                                return title
                            }
                        }
                        elide: hasInfo(title)?Text.ElideRight:Text.ElideMiddle
                        width: listItem.paddingItem.width
                        x: listItem.paddingItem.x
                        mode: listItem.mode
                        role: "Title"
                        text: getTitle()
                    }
                    ListItemText {
                        height: hasInfo(language)||hasInfo(date)?implicitHeight:0
                        mode: listItem.mode
                        elide: Text.ElideRight
                        width: listItem.paddingItem.width
                        x: listItem.paddingItem.x
                        role: "SubTitle"
                        text: hasInfo(language)||hasInfo(date)?language+", "+date:""
                    }
                    ListItemText {
                        mode: listItem.mode
                        elide: Text.ElideRight
                        width: listItem.paddingItem.width
                        x: listItem.paddingItem.x
                        role: "SubTitle"
                        height : hasInfo(creator)?implicitHeight:0
                        text: hasInfo(creator)?creator:""
                    }
                    ListItemText {
                        mode: listItem.mode
                        elide: Text.ElideRight
                        width: listItem.paddingItem.width
                        x: listItem.paddingItem.x
                        role: "SubTitle"
                        height : hasInfo(description)?implicitHeight:0
                        text: hasInfo(description)?description:""
                    }
                    ListItemText {
                        mode: listItem.mode
                        elide: Text.ElideMiddle
                        width: listItem.paddingItem.width
                        x: listItem.paddingItem.x
                        role: "SubTitle"
                        text: fileName
                    }
                }
            }
            onClicked: {
                library.openZimFile(fileName)
            }
            onPressAndHold: {
                console.log("in pressAndHold: index, "+index)
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
                text: qsTr("About zimfile")
                onClicked: library.aboutZimFile(libraryModel.get(libraryItemMenu.itemIndex).fileName)
            }
            MenuItem {
                text: qsTr("Remove")
                onClicked: {
                    if (backend.fileName==libraryModel.get(libraryItemMenu.itemIndex).fileName) {
                        console.log("Removed currently open zim file. Close it.")
                        closeZimFile();
                    }

                    console.log(" Remove item "+libraryItemMenu.itemIndex)
                    libraryModel.remove(libraryItemMenu.itemIndex)

                }
            }
            MenuItem {
                text: qsTr("Remove All")
                onClicked: {
                    if (backend.fileName != "") {
                        closeZimFile();
                    }
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
        anchors {
            right: parent.right
            left: parent.left
            top: parent.top
            bottom: buttons.top
            bottomMargin: UI.LISTVIEW_MARGIN
            }
        clip: true
        //Focus: true enables keyboard navigation
        focus: true
        delegate: listDelegate
        model: libraryModel
        header: listHeading
    }

    Column {
        id: buttons
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right:  parent.right
        anchors.bottomMargin: UI.LISTVIEW_MARGIN
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
