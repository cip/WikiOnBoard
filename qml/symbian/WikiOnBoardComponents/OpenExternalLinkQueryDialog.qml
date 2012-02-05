// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1

QueryDialogWrapMode {

    property url url
    property bool isLinkInArticle : true

    icon: visual.internetToolbarIconSource
    titleText: qsTr("Open link in browser")
    //To ensure url is wrappd. QueryDialogWrapMode is just a copy of the symbian component,
    // but with message wrapMode exposed.
    messageWrapMode: Text.WrapAtWordBoundaryOrAnywhere
    property string linkInArticleMessage : qsTr("[TRANSLATOR] Explain that link \"%1\" clicked in article is not contained in ebook and needs to be opened in webrowser. Ask if ok.\n%2").replace(
                                               "%1", url).replace(
                                               "%2", appInfo.isSelfSigned?qsTr("[TRANLATOR]Explain that may not work if browser running.","only displayed if self_signed"):"")
    property string linkMessage : qsTr("[TRANSLATOR] Ask whether ok to open link \"%1\" in webrowser.\n%2").replace(
                                      "%1", url).replace(
                                      "%2", appInfo.isSelfSigned?qsTr("[TRANLATOR]Explain that may not work if browser running.","only displayed if self_signed"):"")
    message: isLinkInArticle?linkInArticleMessage:linkMessage;
    acceptButtonText: qsTr("Open")
    rejectButtonText: qsTr("Cancel")
    //SYMBIAN_ONLY
    onClickedOutside: reject()
    onAccepted:  {
        if (!Qt.openUrlExternally(url)) {
            banner.showMessage(qsTr("Opening link \"%1\" in system web browser failed.").replace("%1",url));
        }
        closed()
    }
    onRejected: closed()

    }
