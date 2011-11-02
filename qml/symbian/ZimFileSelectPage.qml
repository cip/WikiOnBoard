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
            ListItem {
                Image {
                    id: folderIcon
                    //height: parent.height*2/3
                    //width: parent.height*2/3
                    anchors { left: parent.left; verticalCenter: parent.verticalCenter }
                    source: visual.folderListIconSource
                    visible: folderModel.isFolder(index)
                }

                Text {
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
