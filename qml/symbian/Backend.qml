import QtQuick 1.0
import WikiOnBoardModule 1.0

Item {

    Component.onCompleted: console.log("onCompleted in Backend")

    ZimFileWrapper {
        id:zimFileWrapper
    }

    function openZimFile(filename) {
        console.log("in backend openZimFile: "+filename)
        zimFileWrapper.openZimFile(filename)
    }

    function getZimFileWrapper() {
        return zimFileWrapper
    }

}
