import QtQuick 1.1

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.symbian 1.1

import "UIConstants.js" as UI

TextPage {
    id: helpPage
    anchors { fill: parent}
    signal findEbookClicked();
    signal openExternalLink(string url);
    //% "Change link to page with localized zim files." (e.g http://cip.github.com/WikiOnBoard/Get-eBooks-DE.html"
    property string zimDownloadUrl : qsTr("http://cip.github.com/WikiOnBoard/Get-eBooks.html");
    property string getEBookLinkCaption : qsTr("Download zimfile");
    property string findEBookUrl : "internal://findEBook";
    property string findEBookCaption : qsTr("Find eBook on Phone");
    //TODO: Find out whether is alternative to replacing %1 twice.
    text: qsTr("[TRANSLATOR] Click %1 for information how to put e-Books like the Wikipedia on your phone. <p>\
Click %2 to open an eBook you've copied to your mobile. <p>\
Explain that clicking %1 opens the website \"%3\" with further instructions.").replace(
              "%1",getHtmlLink(getEBookLinkCaption, zimDownloadUrl)).replace(
              "%2",getHtmlLink(findEBookCaption, findEBookUrl)).replace(
              "%3",zimDownloadUrl).replace(
              "%1",getHtmlLink(getEBookLinkCaption, zimDownloadUrl))
    onLinkActivated: {
        console.log("helpPage.linkActivated: "+link)
        //TODO internal links to text page?
        if (link == findEBookUrl){
            findEbookClicked()
        } else {
            openExternalLink(link)
        }
    }
    tools: backOnlyTools


    onFindEbookClicked: {
        //pageStack.pop(); FIXME: works as expected regarding page, but toolbar is cleared :(
        pageStack.push(Qt.resolvedUrl("ZimFileSelectPage.qml"))
    }    
    onOpenExternalLink: {
        openExternalLinkQueryDialog.askAndOpenUrlExternally(url, false);
    }



}

