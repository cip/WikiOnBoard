//SYMBIAN_SPECIFIC_FILE

import QtQuick 1.1

Rectangle {
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
            pageLoader.source = "MainPage.qml";
        }


    }


    Loader {
        id: pageLoader
        anchors.fill: parent
        onLoaded: {
            console.log("pageLoader loaded, unload splashscreen");
            splashScreenLoader.source = "";
        }
    }
}
