import QtQuick 1.0
import "settings.js" as Settings


ListModel {
    id: model


    function addZimFileEntry(fileName, title, creator, date, source, description, language, relation) {
        console.log("addZimFile: "+fileName+" "+title+" "+" "+creator+" "+date+" "+source+" "+description+" "+language+" "+relation)
        model.append({"fileName": fileName,
                     "title": title,
                     "creator": creator,
                     "date": date,
                     "source": source,
                     "description": description,
                     "language" : language,
                     "relation" : relation
                     })
    }

    function addZimFile(fileName) {
        if (backend.openZimFile(fileName)) { //TODO: This closes currently open zim file if any, which man not be desired
            addZimFileEntry(fileName,
                        backend.getMetaDataString("Title"), //TODO: would make sense to handle undefined entries differently.
                        backend.getMetaDataString("Creator"),
                        backend.getMetaDataString("Date"),
                        backend.getMetaDataString("Source"),
                        backend.getMetaDataString("Description"),
                        backend.getMetaDataString("Language"),
                        backend.getMetaDataString("Relation")
                        )
        } else {
            console.log("could not add zim file. error: "+ backend.errorString());
        }

    }


    function initDev() {
        addZimFile("C:\\Users\\Christian\\Downloads\\wikipedia_de_all_10_2010_beta1.zim");
        addZimFile("C:\\Users\\Christian\\Downloads\\wikipedia_en_wp1_0.8_45000+_12_2010.zimaa");
        addZimFile("C:\\Users\\Christian\\Downloads\\Graz.zim");
        addZimFile("f:\\wikipedia_de_all_10_2010_beta1\\wikipedia_de_all_10_2010_beta1.zimaa");
        addZimFile("f:\\graz.zim");
        addZimFile("e:\\wikipedia-de.zim");
        addZimFile("e:\\wikipedia_en_wp1_0.8_45000+_12_2010.zimaa");
    }

    Component.onCompleted: {
        //initDev()
        Settings.restore(model);

    }

    Component.onDestruction: {
        Settings.store(model)
    }
}
