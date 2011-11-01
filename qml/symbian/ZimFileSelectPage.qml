import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0
import com.nokia.symbian 1.0

WikionboardPage {
    signal zimFileSelected(string file)
    ListView {
        anchors { fill: parent}
        FolderListModel {
            id: folderModel
            showDotAndDotDot: true
            showOnlyReadable: true
            folder: "file://"
            nameFilters: ["*.zim"]
            sortField: FolderListModel.Name
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
