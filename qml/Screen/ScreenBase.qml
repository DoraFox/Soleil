import QtQuick 2.12
import Context 1.0


Rectangle {
    property real wFactor: SizeConst.wFactor
    property real hFactor: SizeConst.hFactor
    property real rFactor: SizeConst.rFactor

    property alias mouseArea: idMouseArea

    implicitWidth: SizeConst.windowWidth * wFactor
    implicitHeight: SizeConst.windowHeight * hFactor

    color: ColorConst.colorArray[ColorConst.Color.LightBlue]

    MouseArea {
        id: idMouseArea
        anchors.fill: parent
        hoverEnabled: true
    }
}
