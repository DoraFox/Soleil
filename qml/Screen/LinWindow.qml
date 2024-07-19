import QtQuick 2.15

import Qt.labs.platform 1.1
import QtQuick.Controls 2.15

import Context 1.0

import Component 1.0

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
                    id: idTextInput
                    anchors.fill: parent
                    font.pixelSize: FontConst.smallSize
                    selectByMouse: true
                    clip: true
                    verticalAlignment: TextInput.AlignVCenter
                }
            }

            Button{
                height: parent.height
                text: "init hardware"
                onClicked: {
                    var list = linMgr.initLinHardware()
                    var output = ""
                    for(var i = 0; i < list.length; ++i)
                    {
                        output = output + list[i] + '\n'
                    }
                    idTexeArea.text = output
                    Context.showInfoMessageBox("aaa")
                }
            }
        }

        ScrollView{
            id: idView
            width: parent.width
            height: 120 * hFactor
            wheelEnabled: true
            ScrollBar.vertical.policy: ScrollBar.AlwaysOn
            clip: true

            TextArea{
                id: idTexeArea
                width: implicitWidth > idView.width ? implicitWidth : idView.width
                height: implicitHeight > idView.height ? implicitHeight : idView.height
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
