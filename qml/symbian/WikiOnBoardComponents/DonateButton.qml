// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.symbian 1.1

Button {

    id: donateButton
    property string productId
    property int requestId
    signal productDataReceived( int requestId, string status,
                               string info, string shortdescription,
                               string price, string result )
    property string temporaryText
    property string storeText
    property bool initialized
    property bool busy
    //TODO perhaps better use signal
    initialized : !iap.isApplicationBusy
    onInitializedChanged: if (initialized && state=="initializing") state="fetchingdata"

    requestId : -1
    width: busy?implicitWidth+busyIndicator.width*2:implicitWidth
    anchors.horizontalCenter: parent.horizontalCenter
    text: qsTr("Loading")
    onClicked: iap.purchaseProduct(productId)
    onProductDataReceived: {
        if (donateButton.requestId==requestId) {
            console.log("productId: "+productId+", donateButton.requestId:"+donateButton.requestId+
                        ": onProductDataReceived:  requestId:"+requestId+
                        "status:"+ status+ " info:"+ info+ "shortdescription:"+shortdescription,
                        "price:"+ price+" result:" +result );
            storeText = info+" "+price
            state = "ready"
        }

    }
    states: [
             State {
                 name: "initializing"
                 PropertyChanges { target: donateButton; enabled: false; text: temporaryText;busy:true }
             },
              State {
              name: "fetchingdata"
              StateChangeScript {
                  script: requestId = iap.getProductData(productId);
              }
              PropertyChanges { target: donateButton; enabled: true; text: temporaryText;busy:true}
              },
              State {
              name: "ready"
              PropertyChanges { target: donateButton; enabled: true; text: storeText;busy:false}
              }
         ]
    Component.onCompleted: {
        iap.productDataReceived.connect(productDataReceived);
        state="initializing"
    }

    BusyIndicator {
        id: busyIndicator
        running: donateButton.busy
        visible: donateButton.busy
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 10

    }

}
