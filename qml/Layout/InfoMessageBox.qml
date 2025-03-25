import QtQuick 2.15
import Context 1.0

import Component 1.0

import QtQuick.Controls 2.15

// path

MouseArea {
    id: root
    anchors.fill: parent

    property var callBack: null

    Rectangle{
        anchors.centerIn: parent
        width: SizeConst.dialogWidth
        height: SizeConst.dialogHeight

        color: "#CCFFFFFF"
        border.color: "grey"

        Text {
            id: idInfoText
            anchors.centerIn: parent
            font.pixelSize: FontConst.normalSize

        }

        Button{
            anchors{
                horizontalCenter: parent.horizontalCenter
                bottom: parent.bottom
                bottomMargin: 30 * SizeConst.hFactor
            }

            width: 150 * SizeConst.wFactor
            height: 50 * SizeConst.hFactor
            text: "确认"

            onClicked: {
                root.visible = false
                if(callBack)
                {
                    callBack()
                }
            }
        }
    }





    Connections{
        target: Context

        function onShowInfoMessageBox(text, callBack)
        {
            idInfoText.text = text
            root.visible = true
            root.callBack = callBack
        }
    }


}
