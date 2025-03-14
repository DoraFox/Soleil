import QtQuick 2.15
import QtQuick.Controls 2.15

import Context 1.0

Text {
    id: root
    elide: Text.ElideRight // 超出部分显示省略号
    font.pixelSize: FontConst.fontSize_12
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    maximumLineCount: 2
    clip: true

    ToolTip {
        visible: root.truncated && mouseArea.containsMouse
        text: root.text
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
    }
}
