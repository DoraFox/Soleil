import QtQuick 2.15

import Qt.labs.platform 1.1
import QtQuick.Controls 2.15

import Context 1.0

import Component 1.0

ScreenBase {
    id: root
    height: 800 * hFactor

    Row{
        id: idFileRow
        anchors{
            top: parent.top
            topMargin: 10 * hFactor
            horizontalCenter: parent.horizontalCenter
        }

        height: 40 * hFactor

        Rectangle{
            width: 400 * wFactor
            height: parent.height

            color: "white"

            TextInput {
                id: idFileInput
                anchors.fill: parent
                font.pixelSize: FontConst.smallSize
                selectByMouse: true
                clip: true
                verticalAlignment: TextInput.AlignVCenter
            }
        }

        Button {
            id: idSelectDirBtn
            width: 50 * wFactor
            height: parent.height
            font.pixelSize: FontConst.smallSize

            text: "📂"
            onClicked: {
                idFileDialog.open()
            }

            FileDialog {
                id: idFileDialog
                nameFilters: [ "ASC file (*.asc)" ]
                folder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)

                onAccepted: {
                    idFileInput.text = file
                }
            }
        }
    }

    Column {
        anchors{
            top: idFileRow.bottom
            topMargin: 20 * hFactor
            horizontalCenter: parent.horizontalCenter
        }
        spacing: 5 * hFactor

        Repeater{
            id: idFrameIdRepeater
            model: idFrameIdModel

            delegate: idComponent
        }
    }

    Button {
        anchors{
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: 50 * hFactor
        }

        width: 100 * wFactor
        height: 50 * hFactor
        font.pixelSize: FontConst.smallSize

        text: "Start"
        onClicked: {
            var frameIDs = []
            for(var i = 0; i < idFrameIdModel.count; ++i)
            {
                frameIDs.push(parseInt(idFrameIdModel.get(i).ID, 16))
            }
            canOpt.dealAscFile(idFileInput.text, frameIDs)
            console.log($msg, frameIDs)
        }

    }


    ListModel{
        id: idFrameIdModel

        ListElement{
            ID: ""
        }

    }

    Component {
        id: idComponent
        Row {
            id: idComp
            height: 40 * hFactor
            spacing: 10 * wFactor

            Row {
                height: parent.height

                Text{
                    height: parent.height
                    font.pixelSize: FontConst.smallSize
                    verticalAlignment: Text.AlignVCenter
                    text: "Frame ID:"
                }

                Rectangle {
                    width: 80 * wFactor
                    height: parent.height

                    color: "white"

                    TextInput {
                        id: idFrameIdInput
                        anchors.fill: parent
                        font.pixelSize: FontConst.smallSize
                        selectByMouse: true
                        clip: true
                        //horizontalAlignment: TextInput.AlignRight
                        verticalAlignment: TextInput.AlignVCenter
                        leftPadding: 30 * wFactor
                        rightPadding: 15 * wFactor

                        validator: RegExpValidator { regExp: /[0-9A-Fa-f]+/ }
                        maximumLength: 3

                        text: ID

                        onTextChanged: {
                            ID = text
                        }

                        Text{
                            height: parent.height
                            font: parent.font
                            verticalAlignment: Text.AlignVCenter
                            text: "0x"
                            leftPadding: 10 * wFactor
                        }
                    }
                }
            }

            RoundButton{
                height: parent.height
                width: height
                font.pixelSize: FontConst.largeSize
                text: "\u002B" // Unicode Character '+'
                enabled: (idFrameIdModel.count < 5)

                onClicked: {
                    idFrameIdModel.insert(index + 1,{ "ID":""})
                }
            }

            RoundButton{
                height: parent.height
                width: height
                font.pixelSize: FontConst.largeSize
                text: "\u002D" // Unicode Character '-'
                enabled: idFrameIdModel.count > 1
                opacity: enabled ? 1 : 0.3

                onClicked: {
                    idFrameIdModel.remove(index, 1)
                }
            }
        }

    }

}
