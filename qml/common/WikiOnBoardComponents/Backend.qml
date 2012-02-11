import QtQuick 1.1
import WikiOnBoardModule 1.0

Item {
    id: backend
    Component.onCompleted: console.log("onCompleted in Backend")

    property string fileName
    property string errorString: zimFileWrapper.errorString;
    property bool isTooLargeError: zimFileWrapper.isTooLargeError;

    ZimFileWrapper {
        id:zimFileWrapper
    }

    function closeZimFile() {
        fileName = ""
    }

    function openZimFile(filename) {
        console.log("in backend openZimFile: "+filename);
        if (zimFileWrapper.openZimFile(filename)) {
            //IF open fails, old file stays open
            backend.fileName = filename
            return true
        }
        return false

    }


    function getZimFileWrapper() {
        return zimFileWrapper
    }

    function getMetaDataString(key) {
        return zimFileWrapper.getMetaDataString(key);
    }

    function zimFileIterator(path, recurseSubdirs) {
        return zimFileWrapper.zimFileIterator(path, recurseSubdirs);
    }

    function nextZimFile() {
        return zimFileWrapper.nextZimFile();
    }

    function getNamespaceCount(nameSpace) {
        return zimFileWrapper.getNamespaceCount(nameSpace);
    }

    function getUUIDString() {
        return zimFileWrapper.getUUIDString();
    }

}
