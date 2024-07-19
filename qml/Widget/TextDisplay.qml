import QtQuick 2.12

import Qt.labs.platform 1.1
import QtQuick.Controls 2.12

import Context 1.0


import Component 1.0

Rectangle {
    id: root
    property real wFactor: SizeConst.wFactor
    property real hFactor: SizeConst.hFactor

    implicitWidth: 400 * wFactor
    implicitHeight: 200 * hFactor

    property real btnWidth: 80 * wFactor
    property real btnHeight: 50 * wFactor
    property real btnFontSize: FontConst.fontSize_16

    color: ColorConst.colorArray[ColorConst.Color.LightBlue]

    ScrollView{
        id: idViewIn
        anchors.fill: parent
        wheelEnabled: true
        ScrollBar.vertical.policy: ScrollBar.AlwaysOn
        clip: true

        TextArea{
            id: idEditIn
            width: implicitWidth > idViewIn.width ? implicitWidth : idViewIn.width
            height: implicitHeight > idViewIn.height ? implicitHeight : idViewIn.height
            selectByKeyboard: true
            selectByMouse: true
            placeholderText: "INPUT"
            placeholderTextColor: "white"
            selectionColor: "white"
            background: Rectangle{
                anchors.fill: parent
                color: "skyblue"
            }
        }
    }


}
