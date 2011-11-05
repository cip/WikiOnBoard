import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI

TextPage {
    id: aboutPage

    //% %4 is only displayed if application is self-signed"
    text: qsTr("WikiOnBoard %1\n\
Author: %2\n\
Uses zimlib (openzim.org) and liblzma.\n\
Build date: %3\n\
%4\n").replace(
              "%1",appInfo.version).replace(
              "%2",qsTr("Christian Puehringer")).replace(
              "%3",appInfo.buildDate).replace(
              "%4",appInfo.isSelfSigned?qsTr("application is self-signed"):"")
}



