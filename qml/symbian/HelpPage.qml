import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI

WikionboardPage {
    id: helpPage
    signal findEbookClicked();

    Text {
        id: helpText
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        font.pixelSize: platformStyle.fontSizeSmall
        color: visual.defaultFontColor        
        anchors { fill: parent; bottomMargin: parent.height*1/5}
        anchors.topMargin: 10
        anchors.rightMargin: 10
        anchors.leftMargin: 10

        text: qsTr("[TRANSLATOR] No zimfile selected. getEBook link  %1 opens url %3 with info where to get eBooks. Menu option %2 in option menu %4 opens zimfile on mobile")        
    }

    Column {
        anchors.top: helpText.bottom
        anchors.topMargin: UI.MARGIN_XLARGE
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right:  parent.right

        Button {
            id: addEBookButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Find eBook on Phone")
            onClicked: findEbookClicked()
        }
        Button {
            id: downloadEBookButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Download eBook")
        }
    }
}
