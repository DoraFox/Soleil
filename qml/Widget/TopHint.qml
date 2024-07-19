import QtQuick 2.12
import Context 1.0


import Component 1.0

Button_Normal {
    property bool bStayOnTopHint: mainWindowCtl.getFlagSatus(Qt.WindowStaysOnTopHint)

    implicitWidth: 50 * SizeConst.wFactor
    implicitHeight: implicitWidth
    radius: height / 2
    onClicked: {
        mainWindowCtl.setFlag(Qt.WindowStaysOnTopHint, !bStayOnTopHint)
        bStayOnTopHint = !bStayOnTopHint
    }

    text: bStayOnTopHint ? "ON" : "OFF"
    font.pixelSize: FontConst.normalSize
}
