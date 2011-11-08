import QtQuick 1.1
import com.nokia.symbian 1.1

//import Qt.labs.components.native 1.0
import "UIConstants.js" as UI

Page {
    Rectangle {
        id: background

        anchors.fill: parent
        color: visual.defaultBackgroundColor

        Image {
            //source: "images/stripes.png"
            fillMode: Image.Tile
            anchors.fill: parent
            opacity: UI.FLICKRPAGE_BG_OPACITY
        }
    }
}
