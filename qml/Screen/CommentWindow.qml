import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0

import Context 1.0

import "../Widget"

ScreenBase {
    id: root

    // 列宽比例
    readonly property var columnFactor: [3, 2, 3, 2, 2, 1, 2]

    // 计算列宽总和
    readonly property int totalFactor: columnFactor.reduce((a, b) => a + b, 0)

    // 计算每列的实际宽度
    readonly property var columnWidth: {
        let widths = [];
        for (let i = 0; i < columnFactor.length; i++) {
            widths.push((columnFactor[i] / totalFactor) * parent.width);
        }
        return widths;
    }

    Component{
        id: idCommentDelegate

        Row{
            height: 50 * hFactor

            spacing: 1 * wFactor

            TextToolTip{
                width: columnWidth[0]
                height: parent.height

                text: nickname
            }

            TextToolTip{
                width: columnWidth[1]
                height: parent.height

                text: content
            }

            TextToolTip{
                width: columnWidth[2]
                height: parent.height

                text: ip_location
            }

            TextToolTip{
                width: columnWidth[3]
                height: parent.height

                text: user_signature
            }

            TextToolTip{
                width: columnWidth[4]
                height: parent.height

                text: create_time
            }

            CheckBox{
                width: columnWidth[5]
                height: parent.height

                checkable: false
                checkState: hadSentMessage ? Qt.Checked : Qt.Unchecked
            }

            Button{
                width: columnWidth[6]
                height: parent.height

                font.pixelSize: FontConst.fontSize_16
                text: "发送私信"

                onClicked: {
                    crawlerController.sendPrivateMessage("hello,,", "MS4wLjABAAAAvaVtiag4hcHNsiehxrsBrVuy7P5hZVQWkp-GxED6YMo")
                }
            }

        }

    }

    Column{
        anchors.fill: parent

        // 表头
        Row {
            id: idHeaderRow
            // width: parent.width
            height: 30 * hFactor
            spacing: 1 * wFactor

            // 列标题
            readonly property var columnTitles: ["用户名", "内容", "IP", "用户签名", "时间", "已私信"]

            Repeater {
                model: idHeaderRow.columnTitles
                delegate: Rectangle {
                    id: headerDelegate
                    width: columnWidth[index]
                    height: idHeaderRow.height
                    color: "#f0f0f0"

                    Text {
                        text: modelData
                        anchors.centerIn: parent
                        font.pixelSize: FontConst.fontSize_16
                        font.bold: true
                    }
                }
            }
        }

        ListView{
            id: idListView

            width: parent.width
            height: parent.height - idHeaderRow.height
            spacing: 5 * hFactor

            delegate: idCommentDelegate
            clip: true

            model: commentModel
        }
    }


}
