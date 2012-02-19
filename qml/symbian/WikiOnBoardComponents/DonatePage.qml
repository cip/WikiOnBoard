import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.symbian 1.1

import "UIConstants.js" as UI

WikionboardPage {
    id: helpPage
    anchors { fill: parent}
    tools: backOnlyTools
    Column {
        anchors.fill: parent
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
            Button {
                id: donate_1
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: !iap.isApplicationBusy
                text: qsTr("Donate ~1$")
                onClicked: iap.purchaseProduct(825910)
            }
            Button {
                id: donate_5
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: !iap.isApplicationBusy
                text: qsTr("Donate ~5$")
                onClicked: iap.purchaseProduct(825911)
            }
            Button {
                id: donate_10
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: !iap.isApplicationBusy
                text: qsTr("Donate ~10$")
                onClicked: iap.purchaseProduct(825912)
            }
            Button {
                id: donate_20
                anchors.horizontalCenter: parent.horizontalCenter
                enabled: !iap.isApplicationBusy
                text: qsTr("Donate ~20$")
                onClicked: iap.purchaseProduct(825916)
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

