
import QtQuick 1.1
import com.nokia.symbian 1.1

MenuItem {
    id: root

    property bool checked: false

    //platformLeftMargin: 2 * platformStyle.paddingMedium + platformStyle.graphicSizeSmall
    onClicked: pageStack.push(Qt.resolvedUrl("AboutPage.qml"))
    text: qsTr("About")
    /*Image {
        id: checkIcon

        anchors {
            left: parent.left
            leftMargin: platformStyle.paddingMedium
            verticalCenter: parent.verticalCenter
        }

        visible: root.checked
        source: visual.okToolbarIconSource
        sourceSize.width: platformStyle.graphicSizeSmall
        sourceSize.height: platformStyle.graphicSizeSmall
    }*/
}
