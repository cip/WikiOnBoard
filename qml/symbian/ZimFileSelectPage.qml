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
                id: listItem
                Column {
                    anchors.fill: listItem.paddingItem
                    ListItemText {
                        mode: listItem.mode
                        role: "Title"
                        text: fileName
                    }
                }
                onClicked:{
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


        model: folderModel
        delegate: fileDelegate
    }
}
