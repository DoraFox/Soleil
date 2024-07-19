import QtQuick 2.12
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Context 1.0


ApplicationWindow {
    id: window
    width: SizeConst.windowWidth
    height: SizeConst.windowHeight
    visible: true

    menuBar: MenuBar {
        Menu {
            id: menu
            title: "&File"
            Action { text: "&New..." }
            Action { text: "&Open..." }
            Action { text: "&Save" }
            Action { text: "Save &As..." }
            MenuSeparator { }
            Action { text: "&Quit" }
        }
        Menu {
            title: "&Edit"
            Action { text: "Cu&t" }
            Action { text: "&Copy" }
            Action { text: "&Paste" }
        }
        Menu {
            title: "&Help"
            Action { text: "&About" }
        }
    }

    header: ToolBar {
             RowLayout {
                 anchors.fill: parent
                 ToolButton {
                     text: "‹"
                     onClicked: stack.pop()
                 }
                 Label {
                     text: "Title"
                     elide: Label.ElideRight
                     horizontalAlignment: Qt.AlignHCenter
                     verticalAlignment: Qt.AlignVCenter
                     Layout.fillWidth: true
                 }
                 ToolButton {
                     text: "⋮"
                     onClicked: menu.open()
                 }
             }
         }

         StackView {
             id: stack
             anchors.fill: parent
         }
}
