// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.symbian 1.1

Button {
    property string productId
    property int requestId
    //TODO: rename to requestId and find out how to distinguish
    // between property and argument in signal handler
    signal productDataReceived( int requestIdA, string status,
                               string info, string shortdescription,
                               string price, string result )
    requestId : -1
    anchors.horizontalCenter: parent.horizontalCenter
    enabled: !iap.isApplicationBusy
    text: qsTr("Loading")
    onClicked: iap.purchaseProduct(productId)
    onEnabledChanged: {
        if (enabled) {
            requestId = iap.getProductData(productId)
        }
    }
    onProductDataReceived: {
        console.log("productId: "+productId+", requestId:"+requestId+
                    ": onProductDataReceived:  requestIdA:"+requestIdA+
                    "status:"+ status+ " info:"+ info+ "shortdescription:"+shortdescription,
                    "price:"+ price+" result:" +result );

        if (requestId==requestIdA) {
            text = info+" "+price
        }
    }

    Component.onCompleted: {
        iap.productDataReceived.connect(productDataReceived);
    }
}
