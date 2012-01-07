
import QtQuick 1.1
//HARMATTAN_SPECIFIC. For symbian use: import com.nokia.symbian 1.1
import com.nokia.meego 1.0

MenuItem {
    id: root
    onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
    text: qsTr("About")    
}
