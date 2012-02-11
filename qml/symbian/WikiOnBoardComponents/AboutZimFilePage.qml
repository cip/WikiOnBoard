import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.symbian 1.1

import "UIConstants.js" as UI

TextPage {
    id: aboutZimFilePage
    property string fileName
    text: ""

    anchors { fill: parent}
    tools: ToolBarLayout {
        ToolButton {
            iconSource: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
    function setText() {
        aboutZimFilePage.text+= qsTr("%1<p>\
Articles : %2<br>\
Images: %3<br>\
Categories: %4<p>\
Title: %5<br>\
Creator: %6<br>\
Publisher: %7<br>\
Date: %8<br>\
Source: %9<br>\
Description: %10<br>\
Language: %11<br>\
Relation: %12<p>\
UUID: %13", "%1 is filename").replace("%1",backend.fileName).replace(
                    "%2",backend.getNamespaceCount('A')).replace(
                    "%3",backend.getNamespaceCount('I')).replace(
                    "%4",backend.getNamespaceCount('U')).replace(
                    "%5", backend.getMetaDataString("Title")).replace(
                    "%6", backend.getMetaDataString("Creator")).replace(
                    "%7", backend.getMetaDataString("Publisher")).replace(
                    "%8", backend.getMetaDataString("Date")).replace(
                    "%9", backend.getMetaDataString("Source")).replace(
                    "%10", backend.getMetaDataString("Description")).replace(
                    "%11", backend.getMetaDataString("Language")).replace(
                    "%12", backend.getMetaDataString("Relation")).replace(
                    "%13",backend.getUUIDString())
    }

    onStatusChanged: {
        if (status == PageStatus.Activating) {
            var currentZimFile = backend.fileName
            if (backend.openZimFile(fileName)) {
                setText();
                if (!backend.openZimFile(currentZimFile)) {
                    console.log("reopining zimfile "+currentZimFile+ " failed. Either nothing openend before, or bug")
                }
            } else {
                aboutZimFilePage.text = qsTr("Error opening file %1.\n\
Error: %2".replace(
                                                 "%1", fileName).replace(
                                                 "%2",backend.errorString()))
            }

        }
    }
}




