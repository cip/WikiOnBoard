import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.symbian 1.1

import "UIConstants.js" as UI

TextPage {
    id: aboutPage
    anchors { fill: parent}

    property string wikionboardUrl : qsTr("http://cip.github.com/WikiOnBoard/")
    text: qsTr("WikiOnBoard %1<br>\
%5<br>\
Build date: %3<br>\
Author: %2<p>\
Uses zimlib (openzim.org) and liblzma.<p>\
%4<br>","Use html tags for new line/paragraphs").replace(
              "%1",appInfo.version).replace(
              "%2",qsTr("Christian Puehringer")).replace(
              "%3",appInfo.buildDate).replace(
              "%4",appInfo.isSelfSigned?qsTr("application is self-signed", "only displayed if application is self-signed"):"").replace(
              "%5",getHtmlLink(wikionboardUrl, wikionboardUrl))
    tools: ToolBarLayout {
        ToolButton {
            iconSource: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
    onLinkActivated: {
        //TODO perhaps show different.
        openExternalLinkQueryDialog.askAndOpenUrlExternally(link);
    }
}




