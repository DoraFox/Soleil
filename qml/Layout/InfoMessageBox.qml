import QtQuick 2.15
import Context 1.0

import Component 1.0

import QtQuick.Controls 2.15

// path

MouseArea {
    id: root
    anchors.fill: parent

    Rectangle{
        anchors.centerIn: parent
        width: 200
        height: 100
        border.color: "green"

        Dialog {
            id: idDialog
            anchors.centerIn: parent
            width: 180
            height: 90
            title: "Title"
            standardButtons: Dialog.Ok | Dialog.Cancel

            onAccepted: console.log("Ok clicked")
            onRejected: console.log("Cancel clicked")
        }
    }





    Connections{
        target: Context

        function onShowInfoMessageBox(text)
        {
            root.visible = true
            idDialog.title = text
            console.log("sss", text)
        }
    }


}
