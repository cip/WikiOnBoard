import QtQuick 1.0
import Qt.labs.folderlistmodel 1.0

WikionboardPage {
    ListView {
        anchors { fill: parent}
        FolderListModel {
            id: folderModel
            nameFilters: ["*.zim"]
        }

        Component {
            id: fileDelegate
            Text { text: fileName }
        }

        model: folderModel
        delegate: fileDelegate
    }
}
