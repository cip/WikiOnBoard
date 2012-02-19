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

    property bool initialized
    //TODO perhaps better use signal
    initialized : !iap.isApplicationBusy
    onInitializedChanged: if (initialized) state="fetchingdata"

    requestId : -1
    anchors.horizontalCenter: parent.horizontalCenter
    text: qsTr("Loading")
    onClicked: iap.purchaseProduct(productId)
    onEnabledChanged: {
        if (enabled) {

        }
    }
    onProductDataReceived: {
        console.log("productId: "+productId+", donateButton.requestId:"+donateButton.requestId+
                    ": onProductDataReceived:  requestId:"+requestId+
                    "status:"+ status+ " info:"+ info+ "shortdescription:"+shortdescription,
                    "price:"+ price+" result:" +result );

        if (donateButton.requestId==requestId) {
            text = info+" "+price
        }
        state = "ready"
    }
    states: [
             State {
                 name: "initializing"
                 PropertyChanges { target: donateButton; enabled: false }
             },
              State {
              name: "fetchingdata"
              StateChangeScript {
                  script: requestId = iap.getProductData(productId);
              }
              PropertyChanges { target: donateButton; enabled: false }
              },
              State {
              name: "ready"
              PropertyChanges { target: donateButton; enabled: true }
              }
         ]
    Component.onCompleted: {
        state="initializing"
        iap.productDataReceived.connect(productDataReceived);
    }
}
