import QtQuick 1.1
////import com.nokia.meego 1.0

import com.nokia.symbian 1.1

import "UIConstants.js" as UI

WikionboardPage {
    id: textPage
    signal linkActivated(string link)
    property alias text: txt.text
    function getHtmlLink(caption, url) {
        return "<a href=\"%1\" style=\"color:blue\">%2</a>".replace("%1",url).replace("%2",caption);
    }
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
                font.pixelSize: platformStyle.fontSizeLarge
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

