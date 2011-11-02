import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0
import com.nokia.symbian 1.0

WikionboardPage {    
    signal zimFileSelected(string file)

    function folderUp() {
        folderModel.folder = folderModel.parentFolder
    }
    property bool isDriveSelection : true

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
            Item {
                height: 75
                width: folderListView.width

                Rectangle {
                    width: 4
                    height: parent.height
                    color: "#2d2875"
                    visible: index % 2
                }

                Image {
                    id: folderIcon

                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                    source: "./gfx/list-icons/folder.svg"
                    visible: folderModel.isFolder(index)
                }

                Text {
                    anchors {
                        left: folderIcon.right
                        right: parent.right
                        leftMargin: 5
                        verticalCenter: parent.verticalCenter
                    }
                    elide: Text.ElideRight
                    font.pixelSize: 22
                    font.letterSpacing: -1
                    color: "white"
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
