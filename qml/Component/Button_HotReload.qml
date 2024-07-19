import QtQuick 2.12
import Context 1.0


Button_Normal{
    id: root

    implicitWidth: 100 * SizeConst.wFactor
    implicitHeight: implicitWidth
    text: qsTr("_T1007")
    radius: width / 2
    onClicked: {
        mouse.accepted = false
        Context.trigger_hot_reload(mouse)
    }
}
