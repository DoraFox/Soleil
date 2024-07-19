import QtQuick 2.12

import Qt.labs.platform 1.1
import Context 1.0


import Component 1.0

Rectangle {
    property real wFactor: SizeConst.wFactor
    property real hFactor: SizeConst.hFactor

    implicitWidth: 400 * wFactor
    implicitHeight: 200 * hFactor

    color: ColorConst.colorArray[ColorConst.Color.LightBlue]

    FolderDialog{
        id: idFolderDialog

        onAccepted: {
            fileOpt.transformQMLConnection(folder, true)
        }
    }

    Grid{
        anchors.centerIn: parent

        columns: 2
        rowSpacing: 10 * hFactor
        columnSpacing: 10 * wFactor


        Button_Normal{
            text: "Connections 5.12 to 5.15"
            onClicked: {
                idFolderDialog.open()
            }
        }
    }

}
