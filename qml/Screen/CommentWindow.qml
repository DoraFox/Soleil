import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0

import Context 1.0
import CommentModel 1.0

import "../Widget"

ScreenBase {
    id: root

    readonly property real margins : 10 * wFactor
    readonly property real rowSpacing : 1 * wFactor
    // 列宽比例
    readonly property var columnFactor: [1, 2, 4, 1, 2, 2, 1, 2]

    // 计算列宽总和
    readonly property int totalFactor: columnFactor.reduce((a, b) => a + b, 0)

    // 计算每列的实际宽度
    readonly property var columnWidth: {
        let widths = [];
        let realWidth = parent.width - rowSpacing * (columnFactor.length - 1) - margins * 2
        for (let i = 0; i < columnFactor.length; i++) {
            widths.push((columnFactor[i] / totalFactor) * realWidth);
        }
        return widths;
    }

    signal closeWindow()

    Component{
        id: idCommentDelegate

        Row{
            height: 50 * hFactor
            spacing: rowSpacing

            TextToolTip{
                width: columnWidth[0]
                height: parent.height

                text: index + 1
            }

            TextToolTip{
                width: columnWidth[1]
                height: parent.height

                text: nickname

                mouseArea{
                    cursorShape: Qt.OpenHandCursor
                    onClicked: {
                        Qt.openUrlExternally("https://www.douyin.com/user/" + sec_uid)  // 打开网址
                    }
                }
            }

            TextToolTip{
                width: columnWidth[2]
                height: parent.height

                text: content
            }

            TextToolTip{
                width: columnWidth[3]
                height: parent.height

                text: ip_location
            }

            TextToolTip{
                width: columnWidth[4]
                height: parent.height

                text: user_signature
            }

            TextToolTip{
                width: columnWidth[5]
                height: parent.height

                text: create_time
            }

            CheckBox{
                id: idCheckBox
                width: columnWidth[6]
                height: parent.height

                checkable: false
                checkState: hadSentMessage ? Qt.Checked : Qt.Unchecked
            }

            Button{
                width: columnWidth[7]
                height: parent.height

                font.pixelSize: FontConst.fontSize_16
                text: "发送私信"

                onClicked: {
                    crawlerController.sendPrivateMessage(idMsgTextCombo.editText, sec_uid)
                    commentModel.setData(commentModel.index(index, 0), true, CommentModel.HadSentMessageRole);
                }
            }
        }
    }

    Column{
        id: idColumn
        anchors.fill: parent
        anchors.margins: margins

        spacing: 10 * hFactor

        Item{
            id: idBtnRow
            width: parent.width
            height: 50 * hFactor

            Button {
                anchors.left: parent.left
                width: 200 * wFactor
                height: parent.height
                text: "返回"
                onClicked: {
                    if(idMsgTextCombo.editText != idMsgTextCombo.currentText)
                    {
                        var newText = idMsgTextCombo.editText.trim()
                        if(newText.length != 0)
                        {
                            systemSettings.setMessageDataModel(idMsgTextCombo.currentIndex, idMsgTextCombo.editText)
                        }
                    }

                    root.closeWindow()
                }
            }

            Row{
                anchors.horizontalCenter: parent.horizontalCenter
                height: parent.height
                spacing: 5 * wFactor

                Text{
                    width: 100 * wFactor
                    height: parent.height
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignVCenter
                    font.pixelSize: FontConst.fontSize_16

                    text: "私信内容："
                }

                ComboBox{
                    id: idMsgTextCombo
                    width: 500 * wFactor
                    height: parent.height
                    font.pixelSize: FontConst.fontSize_16

                    selectTextByMouse: true
                    editable: true

                    model: systemSettings.messageDataModel

                    onPressedChanged: {
                        if(pressed && editText != currentText)
                        {
                            var newText = editText.trim()
                            if(newText.length != 0)
                            {
                                systemSettings.setMessageDataModel(currentIndex, editText)
                            }
                        }
                    }
                }
            }


            Button {
                anchors.right: parent.right
                width: columnWidth[7]
                height: parent.height
                text: "删除所有评论"
                onClicked: {
                    commentModel.deleteCurrentAwemeComment()
                }
            }

        }


        // 表头
        Row {
            id: idHeaderRow
            height: 30 * hFactor
            spacing: rowSpacing

            // 列标题
            readonly property var columnTitles: ["", "用户名", "内容", "IP", "用户签名", "时间", "已私信"]

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
            height: parent.height - idHeaderRow.height - idBtnRow.height - idColumn.spacing * 2
            spacing: 5 * hFactor

            delegate: idCommentDelegate
            clip: true

            model: commentModel
        }
    }


}
