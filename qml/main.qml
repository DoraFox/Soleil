import QtQuick 2.12
import QtQuick.Controls 2.12
import Context 1.0
import QtQuick.Window 2.12

Window {
    id: root
    width: SizeConst.windowWidth
    height: SizeConst.windowHeight
    visible: true
    title: qsTr("_T0001")

    objectName: "mainObject"

    Component.onCompleted: {
        console.log($msg, "window size:", SizeConst.windowWidth, "x",
                    SizeConst.windowHeight)
        recalculateFactor()
    }

    onWidthChanged: {
        recalculateFactor()
    }

    onHeightChanged: {
        recalculateFactor()
    }

    function recalculateFactor() {
        SizeConst.wFactor = width / SizeConst.windowWidth
        SizeConst.hFactor = height / SizeConst.windowHeight
    }

    Connections {
        target: root
        function onClosing(close) {
            close.accepted = false
            console.log($msg, "Closing QML Application...")
            visible = false
            Qt.quit()
        }
    }

}
