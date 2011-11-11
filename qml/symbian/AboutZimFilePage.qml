import QtQuick 1.1
////import com.nokia.meego 1.0

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
        aboutZimFilePage.text = qsTr("%1\n\
Articles : %2, Images: %3, Categories: %4\n",
                                                 "Add new line after text").replace(
                                    "%1",backend    .fileName).replace(
                                    "%2",backend.getNamespaceCount('A')).replace(
                                    "%3",backend.getNamespaceCount('I')).replace(
                                    "%4",backend.getNamespaceCount('U'));
        aboutZimFilePage.text+= qsTr("Title: %1\n\
Creator: %2\n\
Date: %3\n\
Source: %4\n\
Description: %5\n\
Language: %6\n\
Relation: %7\n", "Add newline after Text").replace(
                    "%1", backend.getMetaDataString("Title")).replace(
                    "%2", backend.getMetaDataString("Creator")).replace(
                    "%3", backend.getMetaDataString("Date")).replace(
                    "%4", backend.getMetaDataString("Source")).replace(
                    "%5", backend.getMetaDataString("Description")).replace(
                    "%6", backend.getMetaDataString("Language")).replace(
                    "%7", backend.getMetaDataString("Relation"));
        aboutZimFilePage.text+= qsTr("UUID: %1\n"
                                     ).replace(
                    "%1",backend.getUUIDString());
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

            } else if (status == PageStatus.Deactivating) {
                toolBar.tools = defaultTools;
            }
        }
    }




