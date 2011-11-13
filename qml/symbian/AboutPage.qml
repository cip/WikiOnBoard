import QtQuick 1.1
////import com.nokia.meego 1.0

import com.nokia.symbian 1.1

import "UIConstants.js" as UI

TextPage {
    id: aboutPage
    text: qsTr("WikiOnBoard %1\n\
Author: %2\n\
Uses zimlib (openzim.org) and liblzma.\n\
Build date: %3\n\
%4\n").replace(
              "%1",appInfo.version).replace(
              "%2",qsTr("Christian Puehringer")).replace(
              "%3",appInfo.buildDate).replace(
              "%4",appInfo.isSelfSigned?qsTr("application is self-signed", "only displayed if application is self-signed"):"")
    anchors { fill: parent}
    tools: ToolBarLayout {
            ToolButton {
                iconSource: "toolbar-back"
                onClicked: pageStack.pop()
            }
        }    
    }




