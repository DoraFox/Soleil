import QtQuick 2.15
import QtQuick.Controls 2.15

import Context 1.0

Text {
    id: root

    property alias mouseArea: idMouseArea

    elide: Text.ElideRight // 超出部分显示省略号
    font.pixelSize: FontConst.fontSize_16
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    wrapMode: Text.Wrap  // 自动换行
    maximumLineCount: 2
    clip: true

    MouseArea {
        id: idMouseArea
        anchors.fill: parent
        hoverEnabled: true
    }

    ToolTip {
        visible: root.truncated && idMouseArea.containsMouse
        text: root.text
        delay: 500 // 0.5秒延迟显示，避免过于敏感
    }
}
