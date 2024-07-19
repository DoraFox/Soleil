import QtQuick 2.12
import "../Widget/"

Rectangle {
    id: root
    anchors.fill: parent

    color: "pink"

    MouseArea {
        anchors.fill: parent
        pressAndHoldInterval: 100
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        property bool bPressedAndHold: false

        onPressAndHold: {
            bPressedAndHold = true
        }

        onReleased: {
            bPressedAndHold = false
            root.visible = false
        }

        ColorLabel {
            anchors.centerIn: parent
            scale: parent.bPressedAndHold ? 0.8 : 1.0
        }
    }
}
