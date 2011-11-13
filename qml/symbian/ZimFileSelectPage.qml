import QtQuick 1.1
import Qt.labs.folderlistmodel 1.0
import com.nokia.symbian 1.1

WikionboardPage {    
    id: zimFileSelectPage

    signal zimFileSelected(string file)

    function folderUp() {
        folderModel.folder = folderModel.parentFolder
    }
    property bool isDriveSelection : true

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
            iconSource: "toolbar-back"
            onClicked: zimFileSelectPage.isDriveSelection?pageStack.pop():zimFileSelectPage.folderUp()
        }
        ToolButton {
            iconSource: visual.closeToolbarIconSource
            onClicked: pageStack.pop();
        }

    }
    onStatusChanged: {
        //TODO: Download->Open zim file->Back/Open No toolbar on library page fix this
        if (status == PageStatus.Activating) {
            //TODO?
        } else if (status == PageStatus.Deactivating) {
            toolBar.tools = defaultTools;
        }
    }
    onIsDriveSelectionChanged: console.log("isDriveSelection:"+isDriveSelection)
    ListView {
        id: folderListView
        anchors { fill: parent}
        FolderListModel {
            id: folderModel
            // Not very useful, because not displayed at  drive (like c:) level
            //showDotAndDotDot: true
            showOnlyReadable: true
            folder: "file://"
            nameFilters: ["*.zim","*.zimaa"]
            sortField: FolderListModel.Name
            onFolderChanged: {isDriveSelection = (parentFolder=="")}
        }

        Component {
            id: fileDelegate
            ListItem {
                function isSystemDrive() {
                    //Sysmbian specific, TODO change for meego.(D, Z are internal drives)
                    return isDriveSelection && (fileName == "D:/" || fileName == "Z:/")

                }
                visible:!isSystemDrive()
                height: {
                    return isSystemDrive()?0:Math.max(file.implicitHeight, folderIcon.implicitHeight);
                }

                Image {
                    id: folderIcon
                    //height: parent.height*2/3
                    //width: parent.height*2/3
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                    source: visual.folderListIconSource
                    visible: folderModel.isFolder(index)
                }

                Text {
                    id: file
                    anchors {
                        left: folderIcon.right
                        right: parent.right
                        leftMargin: visual.margins
                        verticalCenter: parent.verticalCenter
                    }
                    elide: Text.ElideRight
                    font.pixelSize: visual.defaultFontSize
                    font.letterSpacing: -1
                    color: visual.defaultFontColor
                    text: fileName
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: {
                        console.log("listonclicked. file: "+fileName+ "index: "+index )
                        if (folderModel.isFolder(index)) {
                            if (fileName == "..")
                                folderModel.folder = folderModel.parentFolder
                            else
                                folderModel.folder += "/" + fileName

                            console.log(folderModel.folder)
                        } else {
                            var file = folderModel.folder + "/" + fileName
                            console.log("ZImFileSelect file selected:"+file)

                            file = file.split("file:///")[1]; //FIXME: hardly a reliabe solution
                            console.log("ZImFileSelect file selected: (After file:// removal)"+file)
                            zimFileSelected(file)
                        }
                    }
                }
            }

        }


        model: folderModel
        delegate: fileDelegate
    }
}
