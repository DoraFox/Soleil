import QtQuick 2.15

import Qt.labs.platform 1.1
import QtQuick.Controls 2.15

import Context 1.0

import Component 1.0
import LogFileOpt 1.0

ScreenBase {
    id: root

    Column{
        anchors.fill: parent

        spacing: 10 * hFactor

        Item{
            width: 1
            height: 1 * hFactor
        }

        Row{
            anchors.horizontalCenter: parent.horizontalCenter
            height: 50
            Rectangle {
                width: 400 * wFactor
                height: parent.height
                color: "white"

                TextInput {
                    id: idDtcTextInput
                    anchors.fill: parent
                    font.pixelSize: FontConst.smallSize
                    selectByMouse: true
                    clip: true
                    verticalAlignment: TextInput.AlignVCenter
                }
            }

            Button{
                height: parent.height
                text: "parse DTC"
                onClicked: {
                    idDtcTextArea.text = logFileOpt.parseDtc(LogFileOpt.DtcType_StraightDriveLumbar, idDtcTextInput.text)
                }
            }
        }

        ScrollView{
            id: idDtcView
            width: parent.width
            height: 120 * hFactor
            wheelEnabled: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            clip: true

            TextArea{
                id: idDtcTextArea
                width: implicitWidth > idDtcView.width ? implicitWidth : idDtcView.width
                height: implicitHeight > idDtcView.height ? implicitHeight : idDtcView.height
                selectByKeyboard: true
                selectByMouse: true
                placeholderText: ""
                placeholderTextColor: "white"
                selectionColor: "white"
                background: Rectangle{
                    anchors.fill: parent
                    color: "skyblue"
                }
            }
        }

        Row{
            anchors.horizontalCenter: parent.horizontalCenter
            height: 50
            Rectangle {
                width: 400 * wFactor
                height: parent.height
                color: "white"

                TextInput {
                    id: idAdTextInput
                    anchors.fill: parent
                    font.pixelSize: FontConst.smallSize
                    selectByMouse: true
                    clip: true
                    verticalAlignment: TextInput.AlignVCenter
                }
            }

            Button{
                height: parent.height
                text: "Key AD"
                onClicked: {
                    idAdTextArea.text = logFileOpt.parseKeyAD(idAdTextInput.text)
                }
            }
        }

        ScrollView{
            id: idAdView
            width: parent.width
            height: 120 * hFactor
            wheelEnabled: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            clip: true

            TextArea{
                id: idAdTextArea
                width: implicitWidth > idAdView.width ? implicitWidth : idAdView.width
                height: implicitHeight > idAdView.height ? implicitHeight : idAdView.height
                selectByKeyboard: true
                selectByMouse: true
                placeholderText: ""
                placeholderTextColor: "white"
                selectionColor: "white"
                background: Rectangle{
                    anchors.fill: parent
                    color: "skyblue"
                }
            }
        }

    }

}
