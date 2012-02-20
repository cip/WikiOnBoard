import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.symbian 1.1

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
    id: helpPage
    anchors { fill: parent}
    tools: backOnlyTools
    Flickable {
        anchors.fill: parent
        contentHeight : column.height
        contentWidth : width
        flickableDirection: Flickable.VerticalFlick
        clip: true
        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            spacing: UI.LISTVIEW_MARGIN
            anchors.topMargin:10
            anchors.bottomMargin:10
            Label {
                id: donateText
                anchors.left: parent.left
                anchors.right:  parent.right

                text: qsTr("[Translator] If you like WikiOnBoard consider donating. Click button below. Note that exact amount  depends on country, youu will see the exact amount befoire f inally making payment.")
                wrapMode: Text.WordWrap
            }

            Column {
                id: donateButtons
                anchors.left: parent.left
                anchors.right:  parent.right
                spacing: UI.LISTVIEW_MARGIN
                DonateButton {
                    id: donate_1
                    productId: "825910"
                    temporaryText: qsTr("Donate a little", "Displayed before actual value (~1€) loaded from store")
                }
                DonateButton {
                    id: donate_5
                    productId : "825911"
                    temporaryText: qsTr("Donate more", "Displayed before actual value (~5€) loaded from store")
                }

                DonateButton {
                    id: donate_10
                    productId: "825912"
                    temporaryText: qsTr("Donate even more", "Displayed before actual value (~10€) loaded from store")
                }

                DonateButton {
                    id: donate_20
                    productId: "825916"
                    temporaryText: qsTr("Donate a lot", "Displayed before actual value (~20€) loaded from store")
                }
            }

            Label {
                id: donateMediaWikiText
                anchors.left: parent.left
                anchors.right: parent.right
                text: qsTr("[Translator] Donate media wiki")
                wrapMode: Text.WordWrap
            }
        }
    }
    IAP {
        id: iap
        onProductDataReceived: {
            console.log("onProductDataReceived:  requestId:"+requestId+
                        "status:"+ status+ " info:"+ info+ "shortdescription:"+shortdescription,
                        "price:"+ price+" result:" +result );
        }

        onPurchaseCompleted: {
            console.log("onPurchaseCompleted: requestId:"+requestId +" status:"+
                        status+" purchaseTicket:"+purchaseTicket );
        }
        onPurchaseFlowFinished: {
            console.log("onPurchaseFlowFinished. requestId:"+requestId)
        }
    }
}

