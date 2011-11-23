import QtQuick 1.1

Rectangle {
    id: listItem
    signal clicked();
    signal pressAndHold();
    width: parent.width
    property string mode: ""
    MouseArea {
        anchors.fill:parent
        onClicked:
        {
            console.log("onClicked: "+mouse.wasHeld)
            if (mouse.wasHeld) {
                //FIXME: Does not work
                listItem.pressAndHold();
            } else {
                listItem.clicked();
            }


        }

    }
}
