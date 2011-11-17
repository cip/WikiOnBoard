// MenuItemWithCheck.qml

import QtQuick 1.1

import com.nokia.meego 1.0
//import com.nokia.symbian 1.1

MenuItem {
    id: root

    property bool checked: false

    //FIXME: not on meego platformLeftMargin: 2 * platformStyle.paddingMedium + platformStyle.graphicSizeSmall
    onClicked: checked = !checked

    Image {
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
    }
}
