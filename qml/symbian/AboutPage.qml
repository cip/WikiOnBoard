import QtQuick 1.0
////import com.nokia.meego 1.0

import com.nokia.symbian 1.0

import "UIConstants.js" as UI

TextPage {
    id: aboutPage

    text: qsTr("WikiOnBoard %1\n\
Author: %2\n\
Uses zimlib (openzim.org) and liblzma.\n\
Build date: %3\n\
%4\n").replace(
              "%1","version" /*TODO QString::fromLocal8Bit(__APPVERSIONSTRING__)*/).replace(
              "%2",qsTr("Christian Puehringer")).replace(
              "%3","Today" /*TODO QString::fromLocal8Bit(__DATE__)*/).replace(
              "%4","" /*TODO selfSignedText*/) //TODO register as properties (in object?) in c++
}



