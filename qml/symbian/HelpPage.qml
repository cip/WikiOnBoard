import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI

WikionboardPage {
    id: helpPage
    signal findEbookClicked();

    Flickable {
        anchors.fill: parent
        contentHeight : helpText.paintedHeight+buttons.height
        contentWidth : width
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Text {
            id: helpText
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            font.pixelSize: platformStyle.fontSizeSmall
            color: visual.defaultFontColor
            x: 10
            y: 10
            height: helpPage.height*4/5
            width: helpPage.width-2*x

            text: qsTr("[TRANSLATOR] No zimfile selected. getEBook link  %1 opens url %3 with info where to get eBooks. Menu option %2 in option menu %4 opens zimfile on mobile")
        }

        Column {
            id: buttons
            y : helpText.paintedHeight+helpText.y+10
            width: helpPage.width


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
}
