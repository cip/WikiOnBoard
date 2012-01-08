import QtQuick 1.1
//SYMBIAN_SPECIFIC_FILE Would work for harmattan as well, but there
// is appearantly a different (better, but not yet implemented) way to do this


Item {
    id: splashScreenContainer
    anchors.fill: parent
    //Probably not really necessary
    z: 10

    // image source is kept as an property
    property string imageSource : visual.launcherIconSource

    // signal emits when splashscreen animation completes
    signal splashScreenCompleted()
    Image {
        id: splashImage
        source: splashScreenContainer.imageSource
        //Image is SVG. To ensure nice scaling set sourceSize to
        // display size
        sourceSize.width: width
        sourceSize.height: height
        anchors.left: splashScreenContainer.left
        anchors.right: splashScreenContainer.right
        anchors.verticalCenter: splashScreenContainer.verticalCenter
        height: width
    }
    // Basically just to get event for loading
    // application after splash screen has been painted
    //TODO: Safe? Better solution?
    SequentialAnimation {
        id:splashanimation
        PauseAnimation { duration: 1 }
        onCompleted: {
            splashScreenContainer.splashScreenCompleted()
        }
    }
    //starts the splashScreen
    Component.onCompleted: splashanimation.start()
    //Call done() in parent to trigger load of main page
    onSplashScreenCompleted: done();
 }
