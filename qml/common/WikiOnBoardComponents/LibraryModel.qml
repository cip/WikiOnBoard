import QtQuick 1.1
import "settings.js" as Settings


ListModel {
    id: model


    function addZimFileEntry(fileName, title, creator, publisher, date, source, description, language, relation) {
        console.log("addZimFile: "+fileName+" "+title+" "+" "+creator+" "+date+" "+source+" "+description+" "+language+" "+relation)
        model.append({"fileName": fileName,
                     "title": title,
                     "creator": creator,
                     "publisher": publisher,
                     "date": date,
                     "source": source,
                     "description": description,
                     "language" : language,
                     "relation" : relation
                     })
    }

    function addZimFile(fileName) {
        var currentZimFile = backend.fileName;
        if (backend.openZimFile(fileName)) {
            addZimFileEntry(fileName,
                        backend.getMetaDataString("Title"), //TODO: would make sense to handle undefined entries differently.
                        backend.getMetaDataString("Creator"),
                        backend.getMetaDataString("Publisher"),
                        backend.getMetaDataString("Date"),
                        backend.getMetaDataString("Source"),
                        backend.getMetaDataString("Description"),
                        backend.getMetaDataString("Language"),
                        backend.getMetaDataString("Relation")
                        )
            if (!backend.openZimFile(currentZimFile)) {
                console.log("reopining zimfile "+currentZimFile+ " failed. Either nothing openend before, or bug")
            }
            return true
        } else {
            console.log("could not add zim file. error: "+ backend.errorString);
            return false
        }

    }

    Component.onCompleted: {
        Settings.restore(model);

    }

    Component.onDestruction: {
        Settings.store(model)
    }
}
