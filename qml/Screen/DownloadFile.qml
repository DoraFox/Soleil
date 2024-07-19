import QtQuick 2.12

import Qt.labs.platform 1.1
import QtQuick.Controls 2.12

import Context 1.0

import Component 1.0

import NetworkManager 1.0

ScreenBase {

    FolderDialog {
        id: idFolderDialog

        onAccepted: {
            idDirInput.text = folder
        }
    }

    Column {
        anchors.fill: parent
        Row {
            spacing: 10 * wFactor
            height: 100 * hFactor

            Button_Normal {
                id: idSelectDirBtn
                //width: implicitWidth / 2
                height: parent.height
                text: "select save dirtory"
                onClicked: {
                    idFolderDialog.open()
                }
            }

            Rectangle {
                id: idDirInputRec
                width: 200 * wFactor
                height: parent.height

                color: "transparent"
                border.color: ColorConst.getDeepColor()
                border.width: 1

                TextInput {
                    id: idDirInput
                    anchors.fill: parent
                    anchors.margins: 2
                    font.pixelSize: FontConst.smallSize
                    wrapMode: Text.WordWrap
                    selectByMouse: true
                    clip: true

                }
            }

            Button_Normal {
                id: idSetDirBtn
                //width: implicitWidth / 2
                height: parent.height
                text: "create save dirtory"
                onClicked: {
                    var dirPath = dandelion.getDirectoryAbsolutePath(
                                idDirInput.text)
                    dandelion.makeDirectory(dirPath)
                }
            }
        }

        ScrollView {
            id: idViewOut
            width: parent.width
            height: 200 * hFactor
            wheelEnabled: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            clip: true

            TextArea {
                id: idDownloadInput
                width: implicitWidth > parent.width ? implicitWidth : parent.width
                height: implicitHeight > parent.height ? implicitHeight : parent.height
                selectByKeyboard: true
                selectByMouse: true
                placeholderText: "paths"
                placeholderTextColor: "white"
                selectionColor: "white"
                background: Rectangle {
                    anchors.fill: parent
                    color: "pink"
                }
            }
        }

        Button_Normal {
            id: idDownloadBtn
            width: parent.width
            height: 100 * hFactor
            text: (networkMgr.downloadState === NetworkManager.DownloadState_DownloadFinished) ? "start download" : "downloading"
            enabled: networkMgr.downloadState === NetworkManager.DownloadState_DownloadFinished
            onClicked: {
                var dirPath = dandelion.getDirectoryAbsolutePath(
                            idDirInput.text)

                networkMgr.downloadDirctory = dirPath
                console.log($msg, networkMgr.downloadDirctory)

                networkMgr.startDownloadFiles(idDownloadInput.text)
            }
        }
    }
}
