//SYMBIAN_SPECIFIC_FILE

import QtQuick 1.1
import com.nokia.symbian 1.1

Window {
    anchors.fill: parent

    Visual {
        id: visual
    }


    Loader {
        id: splashScreenLoader
        anchors.fill: parent
        source: "SplashScreen.qml"
        function done() {
            // Done is called by SplashScreen after
            // splashscreen has been displayed
            wikionboardLoader.source = "WikiOnBoard.qml";
        }


    }


    Loader {
        id: wikionboardLoader
        anchors.fill: parent
        onLoaded: {
            console.log("wikiOnboard loaded, unload splashscreen");
            splashScreenLoader.source = "";
        }
    }
}
