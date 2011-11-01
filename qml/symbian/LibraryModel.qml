import QtQuick 1.0
import "settings.js" as Settings

ListModel {
    id: model

    function addZimFile(fileName, title, creator, date, source, description, language, relation) {
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

    function initDev() {
        addZimFile("C:\\Users\\Christian\\Downloads\\wikipedia_de_all_10_2010_beta1.zim", "Wikipedia DE", "", "10.08.2010",
            "", "Deutsche Wikipedia, 10.08.2010, 13455 Artikel", "DE", "");
        //addZimFile("Test", "Wikipedia DE", "", "10.08.2010",
          //             "", "Deutsche Wikipedia, 10.08.2010, 13455 Artikel", "DE", "");
    }

    Component.onCompleted: {
        initDev()
        //Settings.restore(model);
    }

    Component.onDestruction: {
        Settings.store(model)
    }
    /*
    ListElement {
        titleText: "Wikipedia DE (Simu)"
        subTitleText: "Deutsche Wikipedia, 10.08.2010, 13455 Artikel"
        fileName : "C:\\Users\\Christian\\Downloads\\wikipedia_de_all_10_2010_beta1.zim"
    }
    ListElement {
        titleText: "Wikipedia EN (Simu)"
        subTitleText: "English Wikipedia, 11.12.2009, 43455 Artikel"
        fileName : "C:\\Users\\Christian\\Downloads\\wikipedia_en_wp1_0.8_45000+_12_2010.zimaa"
    }
    ListElement {
        titleText: "Wikipedia DE (N8)"
        subTitleText: "Deutsche Wikipedia, 10.08.2010, 13455 Artikel"
        fileName : "f:\\wikipedia_de_all_10_2010_beta1\\wikipedia_de_all_10_2010_beta1.zimaa"
    }
    ListElement {
        titleText: "Wikipedia DE (N8)"
        subTitleText: "Alte deutsche wikipedia"
        fileName : "e:\\wikipedia-de.zim"
    }
    ListElement {
        titleText: "Wikipedia EN (N8)"
        subTitleText: "English Wikipedia, 11.12.2009, 43455 Artikel"
        fileName : "e:\\wikipedia_en_wp1_0.8_45000+_12_2010.zimaa"
    }
*/
}
