// MenuItemWithCheck.qml

import QtQuick 1.1
import com.nokia.symbian 1.1

MenuItem {
    id: root

    property bool checked: false

    //1.1. only, therefore right aligned instead platformLeftMargin: 2 * platformStyle.paddingMedium + platformStyle.graphicSizeSmall
    onClicked: checked = !checked

    Image {
        id: checkIcon

        anchors {
            right: parent.right
            leftMargin: platformStyle.paddingMedium
            verticalCenter: parent.verticalCenter
        }

        visible: root.checked
        source: visual.okToolbarIconSource
        sourceSize.width: platformStyle.graphicSizeSmall
        sourceSize.height: platformStyle.graphicSizeSmall
    }
}
