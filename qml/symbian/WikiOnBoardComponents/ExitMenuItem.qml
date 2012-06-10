
import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.android 1.1

MenuItem {
    id: root
    onClicked: Qt.quit()
    text: qsTr("Exit")   
}
