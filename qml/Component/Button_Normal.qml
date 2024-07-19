import QtQuick 2.15
import Context 1.0


Rectangle{
    id: root

    property alias mouseArea: idMouseArea
    property alias text: idText.text
    property alias font: idText.font

    implicitWidth: 200 * SizeConst.wFactor
    implicitHeight: 100 * SizeConst.hFactor

    color: ColorConst.getLightColor()

    opacity: mouseArea.containsMouse ? 0.6 : 1

    signal clicked(var mouse)

    MouseArea{
       id: idMouseArea
        anchors.fill: parent

        onClicked: {
            root.clicked(mouse)
        }

        Text{
            id: idText
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            font.pixelSize: FontConst.normalSize
        }
    }
}
