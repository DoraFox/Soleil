import QtQuick 2.12

import Context 1.0


import Component 1.0

Rectangle {
    property real wFactor: SizeConst.wFactor
    property real hFactor: SizeConst.hFactor

    implicitWidth: 400 * wFactor
    implicitHeight: 200 * hFactor
    focus: true

    color: ColorConst.colorArray[ColorConst.Color.LightBlue]

    property bool isMouseLeft: true

    Shortcut{
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Left"

        onActivated: {
            changeMouseButton(true)
        }
    }

    Shortcut{
        context: Qt.ApplicationShortcut
        sequence: "Ctrl+Right"

        onActivated: {
            changeMouseButton(false)
        }
    }

    Component.onCompleted: {
        isMouseLeft = funcMgr.isCurrentMouseLeft()
    }

    function changeMouseButton(isLeft)
    {
        isMouseLeft = isLeft
        funcMgr.swapMouseButton(isLeft)
    }

    Column{
        id: idColumn
        anchors.centerIn: parent
        spacing: 10 * SizeConst.hFactor

        Text{
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: FontConst.largeSize
            text: qsTr("_T1000")
        }

        Row{
            spacing: 10 * SizeConst.wFactor

            Button_Normal{
                id: idRecLeft
                color: isMouseLeft ? "gray" : "lightgray"
                mouseArea{
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    cursorShape: Qt.OpenHandCursor

                }

                onClicked: {
                    changeMouseButton(true)
                }

                text: qsTr("_T1001")
            }

            Button_Normal{
                id: idRecRight
                color: isMouseLeft ? "lightgray" : "gray"
                mouseArea{
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    cursorShape: Qt.OpenHandCursor

                }
                onClicked: {
                    changeMouseButton(false)
                }

                text: qsTr("_T1002")
            }
        }

    }


}
