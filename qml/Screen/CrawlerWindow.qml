import QtQuick 2.15

import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.qmlmodels 1.0
import QtQuick.Controls 1.4 as Controls


import Context 1.0

import QAbstractSocket 1.0

import "../Widget"

ScreenBase {
    id: root

    signal showCommentWindow()

    signal showCrawlerSettingWindow()

    Column{
        id: idMainColumn
        anchors{
            fill: parent
            margins: 5
        }

        spacing: 10 * hFactor

        Row {
            id: idBtnRow
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10 * wFactor
            height: 50 * hFactor
            /*
            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "脚本状态：" + crawlerController.pythonState
            }*/

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: {
                    var state = ""
                    switch(crawlerController.socketState)
                    {
                    case QAbstractSocket.ConnectingState:
                        state = "连接中"
                        break;
                    case QAbstractSocket.ConnectedState:
                        state = "已连接"
                        break;
                    default:
                        state = "未连接"
                        break;
                    }

                    return "通信状态：" + state
                }
            }

            Button {
                width: 100 * wFactor
                height: parent.height
                text: "登录抖音"
                onClicked: {
                    crawlerController.launchEdge()
                }
            }

            TextField{
                id: idKeyword
                width: 200 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                placeholderText: "请输入搜索词"
            }

            Button {
                width: 100 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: "开始搜索"
                onClicked: {
                    // 调用 Python 后端的方法
                    var keyword = idKeyword.text.trim()
                    if(keyword != "")
                    {
                        crawlerController.startCrawler(keyword)
                    }
                }
            }

            Button {
                width: 100 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: "停止搜索"
                onClicked: {
                    crawlerController.stopCrawler()
                }
            }

            Button {
                width: 100 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: "设置"
                onClicked: {
                    showCrawlerSettingWindow()
                }
            }


        }

        Column{
            id: idView
            width: parent.width
            height: parent.height - idBtnRow.height - parent.spacing

            // 列宽比例
            readonly property var columnFactor: [1, 2, 3, 1, 2, 1, 1, 2, 1]

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

            // 表头
            Row {
                id: idHeaderRow
                width: parent.width
                height: 30 * hFactor
                spacing: 1 * wFactor

                // 列标题
                readonly property var columnTitles: ["", "用户名", "内容", "IP", "网址", "收藏", "评论", "时间", ""]

                Repeater {
                    model: idHeaderRow.columnTitles
                    delegate: Rectangle {
                        id: headerDelegate
                        width: idView.columnWidth[index]
                        height: parent.height
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

            ListView {
                id: idListView
                width: parent.width
                height: parent.height - idHeaderRow.height - parent.spacing
                clip: true

                model: awemeModel

                delegate: Item{
                    id: idDelegate
                    width: idListView.width
                    height: 50 * hFactor

                    Row{
                        height: parent.height
                        spacing: 1 * wFactor

                        Text {
                            width: idView.columnWidth[0]
                            height: parent.height
                            font.pixelSize: FontConst.fontSize_16
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: index + 1
                        }

                        TextToolTip{
                            width: idView.columnWidth[1]
                            height: parent.height

                            text: nickname
                        }

                        TextToolTip{
                            width: idView.columnWidth[2]
                            height: parent.height

                            text: title

                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    commentModel.setCurrentAwemeComment(aweme_id)
                                    showCommentWindow()
                                }
                            }
                        }

                        TextToolTip{
                            width: idView.columnWidth[3]
                            height: parent.height

                            text: ip_location
                        }

                        TextToolTip{
                            id: idUrl
                            width: idView.columnWidth[4]
                            height: parent.height

                            wrapMode: Text.WrapAnywhere
                            text: aweme_url
                            mouseArea{
                                cursorShape: Qt.OpenHandCursor
                            }

                            mouseArea.onClicked: {
                                Qt.openUrlExternally(idUrl.text)  // 打开网址
                            }
                        }

                        TextToolTip{
                            width: idView.columnWidth[5]
                            height: parent.height

                            text: collected_count
                        }

                        TextToolTip{
                            width: idView.columnWidth[6]
                            height: parent.height

                            text: comment_count
                        }

                        TextToolTip{
                            width: idView.columnWidth[7]
                            height: parent.height

                            text: create_time
                        }

                        Button{
                            anchors.verticalCenter: parent.verticalCenter
                            width: idView.columnWidth[8]
                            height: parent.height - 5 * hFactor

                            text: "删除"

                            onClicked: {
                                awemeModel.removeRow(index)
                            }
                        }
                    }
                }
            }
        }

    }

}
