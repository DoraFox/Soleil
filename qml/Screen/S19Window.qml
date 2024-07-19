import QtQuick 2.15

import Qt.labs.platform 1.1
import QtQuick.Controls 2.15

import Context 1.0

import Component 1.0

ScreenBase {
    id: root

    signal updateData()

    ListModel{
        id: idAddressModel

        ListElement{ name: ""; address: ""; value:"" }
        ListElement{ name: ""; address: ""; value:"" }
        ListElement{ name: ""; address: ""; value:"" }
        ListElement{ name: ""; address: ""; value:"" }
        ListElement{ name: ""; address: ""; value:"" }
        ListElement{ name: ""; address: ""; value:"" }
        ListElement{ name: ""; address: ""; value:"" }
        ListElement{ name: ""; address: ""; value:"" }
        ListElement{ name: ""; address: ""; value:"" }

    }

    Item{
        anchors.fill: parent
        anchors.margins: 20

        Column {
            width: parent.width
            spacing: 5 * hFactor

            Row {
                height: 40 * hFactor

                Rectangle {
                    width: 250 * wFactor
                    height: parent.height

                    color: "white"

                    TextInput {
                        id: idDirInput
                        anchors.fill: parent
                        font.pixelSize: FontConst.smallSize
                        selectByMouse: true
                        clip: true
                        verticalAlignment: TextInput.AlignVCenter
                    }
                }

                Button {
                    id: idSelectDirBtn
                    width: 50
                    height: parent.height
                    font.pixelSize: FontConst.smallSize

                    text: "📂"
                    onClicked: {
                        idFolderDialog.open()
                    }

                    FolderDialog {
                        id: idFolderDialog
                        onAccepted: {
                             idDirInput.text = folder
                        }
                    }
                }
            }

            ListView{
                id: idAddressListView
                anchors.horizontalCenter: parent.horizontalCenter
                width: 500 * wFactor
                height: 270 * hFactor
                model: idAddressModel

                spacing: 5 * hFactor
                delegate: idAddressComponent
                clip: true
            }

        }

        Button {
            anchors{
                top: parent.top
                right: parent.right
            }

            width: 100 * wFactor
            height: 50 * hFactor
            font.pixelSize: FontConst.smallSize

            text: "Start"
            onClicked: {
                root.updateData()

                var addressList = []
                var valueList = []

                for(var i = 0; i < idAddressModel.count; ++i)
                {
                    var item = idAddressModel.get(i)
                    var list = item.value.split('-')

                    for(var j = 0; j < list.length; ++j)
                    {
                        if(list[j] !== "")
                        {
                            addressList.push(parseInt(item.address))
                            valueList.push(parseInt(list[j], 16))
                        }
                    }
                }

                console.log(addressList, valueList)

                s19Mgr.startSwapNumbeUpdate(idDirInput.text, addressList, valueList)
            }

        }
    }

    Component{
        id: idAddressComponent
        Row{
            id: idComp
            height: 50 * hFactor
            spacing: 20 * wFactor

            Row{
                height: parent.height
                spacing: 10 * wFactor
                Text{
                    width: 80 * wFactor
                    height: parent.height
                    font.pixelSize: FontConst.smallSize
                    verticalAlignment: Text.AlignVCenter
                    text: name
                }
                TextInput{
                    id: idAddress
                    anchors.verticalCenter: parent.verticalCenter
                    width: 100 * wFactor
                    height: parent.height * 0.75
                    font.pixelSize: FontConst.smallSize
                    verticalAlignment: Text.AlignVCenter
                    selectByMouse: true
                    text: address
                    clip: true

                    Rectangle{
                        anchors.fill: parent
                        color: "white"
                        z: -1
                    }
                }
            }

            Row{
                height: parent.height
                spacing: 10 * wFactor

                Text{
                    height: parent.height
                    font.pixelSize: FontConst.smallSize
                    verticalAlignment: Text.AlignVCenter
                    text: "Value:"
                    clip: true
                }

                TextInput{
                    id: idValue
                    anchors.verticalCenter: parent.verticalCenter
                    height: parent.height * 0.75
                    font.pixelSize: FontConst.smallSize
                    verticalAlignment: Text.AlignVCenter
                    selectByMouse: true
                    inputMask: "HH-HH-HH-HH-HH-HH-HH-HH"
                    clip: true
                    text: value

                    Rectangle{
                        anchors.fill: parent
                        color: "white"
                        z: -1
                    }
                }

            }

            Connections{
                target: root

                function onUpdateData()
                {
                    address = idAddress.text
                    value = idValue.text
                }
            }


        }
    }



}
