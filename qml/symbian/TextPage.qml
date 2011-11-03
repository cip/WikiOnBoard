import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI

WikionboardPage {
    id: textPage
    signal linkActivated(string link)
    property alias text: txt.text
    Flickable {
        anchors.fill: parent
        contentHeight : txt.paintedHeight
        contentWidth : width
        flickableDirection: Flickable.VerticalFlick
        clip: true
        Rectangle {
            height: textPage.height
            width: textPage.width
            color: "white"
            Text {
                id: txt
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pixelSize: platformStyle.fontSizeSmall
                color: "black"
                x: 10
                y: 10
                height: parent.height-2*y
                width: textPage.width-2*x
                onLinkActivated: textPage.linkActivated(link)
            }
        }
    }
}

