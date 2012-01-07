import QtQuick 1.1
//SYMBIAN_SPECIFIC_FILE Would work for harmattan as well, but there
// is appearantly a different (better, but not yet implemented) way to do this

Item {
    id: splashScreenContainer
    // image source is kept as an property
    property string imageSource
    // signal emits when splashscreen animation completes
    signal splashScreenCompleted()
    Image {
        id: splashImage
        source: splashScreenContainer.imageSource
        anchors.fill: splashScreenContainer // do specify the size and position
    }
    // simple QML animation to give a goof User Experience
    SequentialAnimation {
        id:splashanimation
        PauseAnimation { duration: 4200 }
        PropertyAnimation {
            target: splashImage
            duration: 700
            properties: "opacity"
            to:0
        }
        onCompleted: {
            splashScreenContainer.splashScreenCompleted()
        }
    }
    //starts the splashScreen
    Component.onCompleted: splashanimation.start()
 }
