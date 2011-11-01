import QtQuick 1.0
import WikiOnBoardModule 1.0

Item {

    Component.onCompleted: console.log("onCompleted in Backend")

    ZimFileWrapper {
        id:zimFileWrapper
    }

    function openZimFile(filename) {
        console.log("in backend openZimFile: "+filename);
        return zimFileWrapper.openZimFile(filename);
    }

    function errorString() {
        return zimFileWrapper.errorString();
    }

    function getZimFileWrapper() {
        return zimFileWrapper
    }

    function getMetaDataString(key) {
        return zimFileWrapper.getMetaDataString(key);
    }
}
