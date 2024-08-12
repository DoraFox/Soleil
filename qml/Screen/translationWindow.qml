import QtQuick 2.12
import Qt.labs.platform 1.1

import Context 1.0

import TranslationManager 1 .0

import Component 1.0

Rectangle {
    property real wFactor: SizeConst.wFactor
    property real hFactor: SizeConst.hFactor

    implicitWidth: 400 * wFactor
    implicitHeight: 200 * hFactor

    color: ColorConst.colorArray[ColorConst.Color.LightBlue]

    FileDialog {
        id: idFileDialog

        folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)

        onAccepted: {
            translationMgr.translateExcel(file)
        }
    }

    Button_Normal{
        text: qsTr("翻译")
        onClicked: {
            idFileDialog.open()
        }
    }

}
