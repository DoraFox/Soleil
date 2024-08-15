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

         // 0: 翻译Execl文件;
         readonly property var fileOpts:[0, 1]

         property int curOpt: -1

         folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)

         onAccepted: {
             switch(curOpt)
             {
             case fileOpts[0]:
             {
                translationMgr.translateExcel(file)
             }
                 break
             case fileOpts[1]:
             {

             }
                 break
             default:
                 break
             }
         }
     }

    FolderDialog{
        id: idFolderDialog

        // 0: 将Execl文件转换为TS文件;
        readonly property var folderOpts:[0, 1]

        property int curOpt: -1

        onAccepted: {
            switch(curOpt)
            {
            case folderOpts[0]:
            {

            }
                break
            case folderOpts[1]:
            {

            }
                break
            default:
                break
            }
        }
    }

    ListModel{
        id: idTranslationModel
        ListElement{
            opt: 0
            name: qsTr("_T1005") // "将Execl文件转换为TS文件"
            isFileOpt: false
        }
        ListElement{
            opt: 0
            name: qsTr("_T1003") //"翻译Execl文件"
            isFileOpt: true
        }
    }

    Grid{
        anchors.centerIn: parent

        columns: 2
        rowSpacing: 10 * hFactor
        columnSpacing: 10 * wFactor

        Button_Normal{
            text: qsTr("_T0002")
            onClicked: {
                translationMgr.selectLanguage(TranslationManager.Translate_Chinese)
            }
        }

        Button_Normal{
            text: qsTr("_T0003")
            onClicked: {
                translationMgr.selectLanguage(TranslationManager.Translate_English)
            }
        }

        Repeater{
            model: idTranslationModel
            Button_Normal{
                text: name
                onClicked: {
                    if(isFileOpt){
                        idFileDialog.curOpt = opt
                        idFileDialog.open()
                    }else{
                        idFolderDialog.curOpt = opt
                        idFolderDialog.open()
                    }
                }
            }
        }
    }

}
