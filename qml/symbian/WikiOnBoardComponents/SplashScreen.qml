import QtQuick 1.1
//SYMBIAN_SPECIFIC_FILE Would work for harmattan as well, but there
// is appearantly a different (better, but not yet implemented) way to do this


Rectangle {
    id: splashScreenContainer
    anchors.fill: parent
    //Probably not really necessary
    z: 10

    // image source is kept as an property
    property string imageSource : visual.launcherIconSource
    // Actually anyway black background, but without
    // this a small 3g is displayed on top left on device.
    color: "black"
    // signal emits when splashscreen animation completes
    signal splashScreenCompleted()
    Image {
        id: splashImage
        source: splashScreenContainer.imageSource
        //Image is SVG. To ensure nice scaling set sourceSize to
        // display size
        sourceSize.width: width
        sourceSize.height: height
        //Works for both landscape and portrait. Note
        // that switching between landscape and portrait
        // while splashscreen is shown does not work correctly.
        width: Math.min(parent.width,parent.height)
        height: width
        anchors.centerIn: parent
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
