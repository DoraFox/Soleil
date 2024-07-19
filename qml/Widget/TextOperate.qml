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

    Row{
        id: idTextRow
        width: parent.width
        height: parent.height - idBottomColumn.height
        Item{
            width: (parent.width - parent.spacing) / 2
            height: parent.height

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

            Button_Normal{
                id: idPasteBtn
                anchors{
                    bottom: parent.bottom
                    bottomMargin: 5 * SizeConst.hFactor
                    right: parent.right
                    rightMargin: 10 * SizeConst.wFactor
                }
                width: btnWidth
                height: btnHeight
                font.pixelSize: btnFontSize
                text: "Clear&&Paste"

                onClicked: {
                    idEditIn.clear()
                    idEditIn.paste()
                    idPasteTip.opacity = 1
                    idPasteTimer.start()
                }

                Label{
                    id: idPasteTip
                    anchors{
                        bottom: idPasteBtn.top
                        horizontalCenter: idPasteBtn.horizontalCenter
                    }
                    text: "Clear&Paste"
                    color: "white"
                    opacity: 0

                    Behavior on opacity{
                        NumberAnimation{
                            duration: 2000
                            easing.type: Easing.InQuad
                        }
                    }

                    Timer{
                        id: idPasteTimer
                        interval: 3000
                        onTriggered: {
                            idPasteTip.opacity = 0
                        }
                    }
                }
            }

        }


        Item{
            width: idViewIn.width
            height: idViewIn.height

            ScrollView{
                id: idViewOut
                anchors.fill: parent
                wheelEnabled: true
                ScrollBar.vertical.policy: ScrollBar.AlwaysOn
                clip: true

                TextArea{
                    id: idEditOut
                    width: implicitWidth > idViewIn.width ? implicitWidth : idViewIn.width
                    height: implicitHeight > idViewIn.height ? implicitHeight : idViewIn.height
                    selectByKeyboard: true
                    selectByMouse: true
                    placeholderText: "OUTPUT"
                    placeholderTextColor: "white"
                    selectionColor: "white"
                    background: Rectangle{
                        anchors.fill: parent
                        color: "pink"
                    }
                }
            }

            Button_Normal{
                id: idCopyBtn
                anchors{
                    bottom: parent.bottom
                    bottomMargin: 5 * SizeConst.hFactor
                    right: parent.right
                    rightMargin: 10 * SizeConst.wFactor
                }
                width: btnWidth
                height: btnHeight
                font.pixelSize: btnFontSize
                text: "Copy"

                onClicked: {
                    idEditOut.selectAll()
                    idEditOut.copy()
                    idEditOut.deselect()
                    idCopyTip.opacity = 1
                    idCopyTimer.start()
                }

                Label{
                    id: idCopyTip
                    anchors{
                        bottom: idCopyBtn.top
                        horizontalCenter: idCopyBtn.horizontalCenter
                    }
                    text: "Copy All"
                    color: "white"
                    opacity: 0

                    Behavior on opacity{
                        NumberAnimation{
                            duration: 2000
                            easing.type: Easing.InQuad
                        }
                    }

                    Timer{
                        id: idCopyTimer
                        interval: 3000
                        onTriggered: {
                            idCopyTip.opacity = 0
                        }
                    }
                }
            }
        }
    }

    Column{
        id: idBottomColumn
        anchors.bottom: parent.bottom
        width: parent.width

        Rectangle{
            id: idBtnContent
            width: parent.width
            height: btnHeight
            color: "lightgreen"

            Row{
                id: idBtnRow
                anchors{
                    bottom: parent.bottom
                    right: parent.right
                }

                spacing: 2 * SizeConst.wFactor

                Button_Normal{
                    width: btnWidth
                    height: btnHeight
                    font.pixelSize: btnFontSize
                    text: "JS Arguments"
                    opacity: enabled ? 1 : 0.3
                    enabled: idEditIn.text.length > 0

                    onClicked: {
                        idEditOut.text = textOpt.getFuntionParamsList(idEditIn.text)
                    }
                }

                Button_Normal{
                    width: btnWidth * 2
                    height: btnHeight
                    font.pixelSize: btnFontSize
                    text: "hexToDecimal"
                    opacity: enabled ? 1 : 0.3
                    enabled: idEditIn.text.length > 0

                    onClicked: {
                        idEditOut.text = textOpt.hexToDecimal(idEditIn.text)
                    }
                }
            }
        }

        Rectangle{
            width: parent.width
            height: 20 * SizeConst.hFactor
            color: ColorConst.getLightColor()
        }
    }



}
