// Visual style for Symbian
import QtQuick 1.1

Item {
    // General
    property string defaultFontFamily: "Helvetica"  // Defaults to correct ones in device
    property int defaultFontSize: platformStyle.fontSizeLarge
    //property color defaultFontColor: "#6f6a5c"
    property color defaultFontColor: "white"
    property color defaultFontColorLink: "#7c0505"
    property color defaultFontColorButton: "#ffffff"
    property color defaultBackgroundColor: "#343434"
    property double margins: 4
    property int spacing: 8
    property int defaultItemHeight: 48

    property int scrollBarWidth: 8
    property string removeListIconSource: "./gfx/list-icons/remove.svg"
    property string searchListIconSource: "./gfx/list-icons/search.svg"

    property string folderListIconSource: "./gfx/list-icons/folder.svg"
    property string closeToolbarIconSource: "./gfx/toolbar-icons/close_stop.svg"
    property string okToolbarIconSource: "./gfx/toolbar-icons/ok.svg"
    property string internetToolbarIconSource: "./gfx/toolbar-icons/internet.svg"
    property string documentToolbarIconSource: "./gfx/toolbar-icons/document.svg"

    // Title bar
    property int titleFontSize: 36
    property color titleFontColor: "#ffffff"
    property color titleBackgroundColor: "#7c0505"

    property string captionFontFamily: defaultFontFamily
    property int captionFontSize: 8
    property color captionFontColor: "#ffffff"
    property color captionBackgroundColor: "#770d0f"

    property string exitButtonSource: "content/exit_button.png"
    property string backButtonSource: "content/back_button.png"

    // Tab bar
    property string tabBarButtonFont: defaultFontFamily
    property int tabBarButtonFontSize: 10
    property color tabBarButtonFontColor: "#ffffff"

    property string infoButtonSource: "content/info_button.png"
    property string menuButtonSource: "content/menu_button.png"
    property string mapButtonSource: "content/map_button.png"
    property string bookingButtonSource: "content/booking_button.png"

    // Info view
    property int infoViewReservationFontSize: 14
    property int infoViewAddressFontSize: 18
    property int infoViewFontSize: 18

    // Menu grid view
    property int menuGridViewFontSize: 10

    // Menu list view
    property color menuListViewBackgroundColor: "#f9f6f6"
    property color menuListViewDishTitleFontColor: "#7c0505"
    property color menuListViewDishFontColor: "#524F46"
    property int menuListViewTitleSize: 12
    property int menuListTitleFontSize: 8
    property int menuListItemFontSize: 6

    // Map view
    property string zoomiInSource: "content/zoom_in.png"
    property string zoomiOutSource: "content/zoom_out.png"
    property int infoFontSize: 10

    // Booking view
    property int bookingViewFontSize: 16
}
