import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.android 1.1

import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
    id: helpPage
    anchors { fill: parent}
    tools: backOnlyTools
    function getHtmlLink(caption, url) {
        return "<a href=\"%1\">%2</a>".replace("%1",url).replace("%2",caption);
    }

    Flickable {
        anchors.fill: parent
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.topMargin:10
        anchors.bottomMargin:10

        contentHeight : column.height
        contentWidth : width
        flickableDirection: Flickable.VerticalFlick
        clip: true
        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            spacing: UI.LISTVIEW_MARGIN*2
            Label {
                id: donateText
                anchors.left: parent.left
                anchors.right:  parent.right

                text: qsTr("[TRANSLATOR] If you like WikiOnBoard\
consider donating to express your appreciation.<br>\
Click button below to donate easily via Nokia Store.\
")
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
                    temporaryText: qsTr("Donate something", "Displayed before actual value (~5€) loaded from store")
                }

                DonateButton {
                    id: donate_10
                    productId: "825912"
                    temporaryText: qsTr("Donate more", "Displayed before actual value (~10€) loaded from store")
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
                property string donateMediaWikiUrl : qsTr("https://donate.wikipedia.org/")
                property string donateMediaWikiCaption : qsTr("Donate to wikipedia")
                text: qsTr("[TRANSLATOR] If you rather want to donate WikiMedia,\
the organization operating Wikipedia, click  %1 to visit their webpage.<br>\
Note, that WikiOnBoard is not officially affiliated with WikiMedia.").replace(
                          "%1", getHtmlLink(donateMediaWikiCaption, donateMediaWikiUrl))
                onLinkActivated: {
                    openExternalLinkQueryDialog.askAndOpenUrlExternally(link, false);
                }

                wrapMode: Text.WordWrap
            }
        }
    }

    onStatusChanged: {
        if (status == PageStatus.Active) {
            console.log("DonatePage active. Call iap.initIAP()")
            iap.initIAP()            
        }
    }

    IAP {
        id: iap
        property string lastStatus
        lastStatus: ""
        onProductDataReceived: {
            console.log("onProductDataReceived:  requestId:"+requestId+
                        "status:"+ status+ " info:"+ info+ "shortdescription:"+shortdescription,
                        "price:"+ price+" result:" +result );
        }

        onPurchaseCompleted: {
            console.log("onPurchaseCompleted: requestId:"+requestId +" status:"+
                        status+" purchaseTicket:"+purchaseTicket );
            lastStatus =  status
        }
        onPurchaseFlowFinished: {
            console.log("onPurchaseFlowFinished. requestId:"+requestId+ " lastStatus:"+lastStatus)
            if (lastStatus == "OK") {
                pageStack.replace(thanksPage)
            }
            lastStatus = ""

        }

    }

    Component {
        id: thanksPage
        WikionboardPage {
            tools: backOnlyTools
            anchors { fill: parent}

            Flickable {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                anchors.topMargin:10
                anchors.bottomMargin:10
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
                        text: qsTr("[TRANSLATOR] Thank you for supporting WikiOnBoard")
                        wrapMode: Text.WordWrap
                    }
                }

            }
        }
    }
}

