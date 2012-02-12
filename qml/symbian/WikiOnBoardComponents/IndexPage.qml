import QtQuick 1.0

//SYMBIAN_SPECIFIC. For harmattan use: import com.nokia.meego 1.0
import com.nokia.symbian 1.1
import "UIConstants.js" as UI
import WikiOnBoardModule 1.0

WikionboardPage {
    id: index
    signal openArticle(string articleUrl)
    focus: true
    function init() {
        indexListQML.setZimFileWrapper(backend.getZimFileWrapper())
    }
    Rectangle {
        anchors { fill: parent; bottomMargin: parent.height-articleName.height }

        TextField {
            id: articleName
            anchors.verticalCenter: parent.verticalCenter
            width: parent.width
            //SYMBIAN_ONLY
            platformLeftMargin: search.width + platformStyle.paddingSmall
            //SYMBIAN_ONLY            
            platformRightMargin: clearText.width + platformStyle.paddingMedium * 2
            onInputMethodHintsChanged: console.log("inputMethodHints:" +inputMethodHints)
            onFocusChanged: console.log("focus: "+focus)
            onActiveFocusChanged: console.log("activeFocus:" +activeFocus)
            onTextChanged: {
                console.log("TODO:Update search: "+text)
                indexListQML.searchArticle(text)
            }            

            Keys.onUpPressed: {
                    console.log("Key up pressed")
                    indexListQML.selectPreviousEntry()
                    event.accepted = true
                }

            Keys.onDownPressed: {
                    console.log("Key down pressed")
                    indexListQML.selectNextEntry()
                    event.accepted = true
                }
            Keys.onSelectPressed: {
                    console.log("Key select pressed")
                    indexListQML.openCurrentEntry()
                    event.accepted = true
                }

            //Symbian Workaround for always-upperercase after once clicked on article ListElement
            // If length > 0 force autouppercase off
            // (Issue 67)
            //In addition disable Predictive Text. (Else at least on harmattan article
            // list not updated before complete word is entered)
            //inputMethodHints:(text.length===0)?Qt.ImhNoPredictiveText:Qt.ImhNoAutoUppercase|Qt.ImhNoPredictiveText
            Image {
                id: search
                anchors { top: parent.top; left: parent.left; margins: platformStyle.paddingMedium }
                smooth: true
                fillMode: Image.PreserveAspectFit
                source: visual.searchToolbarIconSource
                height: parent.height - platformStyle.paddingMedium * 2
                width: parent.height - platformStyle.paddingMedium * 2
            }
            Image {
                anchors { top: parent.top; right: parent.right; margins: platformStyle.paddingMedium }
                id: clearText
                fillMode: Image.PreserveAspectFit
                smooth: true; visible: articleName.text
                source: clear.pressed ? visual.clearPressedIconSource
                                        : visual.clearNormalIconSource
                height: parent.height - platformStyle.paddingMedium * 2
                width: parent.height - platformStyle.paddingMedium * 2

                MouseArea {
                    id: clear
                    anchors { horizontalCenter: parent.horizontalCenter; verticalCenter: parent.verticalCenter }
                    height: articleName.height; width: articleName.height
                    onClicked: {
                        articleName.text = ""
                        articleName.forceActiveFocus()
                    }
                }                
            }            
        }
    }
    onStatusChanged: {
        if (PageStatus.Activating == status) {
            console.log("IndexPage onStatusChanged: PageStatus.Activating")
            indexListQML.searchArticle(articleName.text)
        }
        if (PageStatus.Active == status) {
            console.log("IndexPage onStatusChanged: PageStatus.Active")
            //Set focus to allow textinput without clicking.
            // (i.p. relevant for devices like Nokia E6)
            //Note that doing this in "Activating"-phase it does not work reliable
            // (focus sometimes "stolen".
            articleName.forceActiveFocus()
            //Does not work
            //articleName.openSoftwareInputPanel()
        }
    }

    function openMenu() {
        menu.open()
    }
    Menu {
        id: menu
        // define the items in the menu and corresponding actions
        content: MenuLayout {
            ExitMenuItem {}
        }
    }
    Rectangle {
        anchors { fill: parent; topMargin: articleName.height }
        IndexListQML {
            id: indexListQML
            anchors { fill: parent}
            onOpenArticle: {
                index.openArticle(articleUrl);
            }
        }
    }
}


