import QtQuick 2.12
import Context 1.0


Rectangle {
    property real wFactor: SizeConst.wFactor
    property real hFactor: SizeConst.hFactor
    property real rFactor: SizeConst.rFactor

    implicitWidth: 600 * wFactor
    implicitHeight: 400 * hFactor

    color: ColorConst.colorArray[ColorConst.Color.LightBlue]
}
