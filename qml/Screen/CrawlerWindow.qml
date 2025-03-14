import QtQuick 2.15
import QtQuick.Controls 2.15
import Qt.labs.qmlmodels 1.0

import Context 1.0

import "../Widget"

ScreenBase {

    Column{
        anchors{
            fill: parent
            margins: 5
        }

        spacing: 10

        Row {
            spacing: 20
            height: 50

            Button {
                width: 100
                height: parent.height
                text: "登录抖音"
                onClicked: {
                    crawlerController.launchEdge()
                }
            }

            TextField{
                id: idKeyword
                width: 200
                height: parent.height
                placeholderText: "请输入搜索词"
            }

            Button {
                width: 100
                height: parent.height
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
                width: 100
                height: parent.height
                text: "停止搜索"
                onClicked: {
                    pythonBackend.stopCrawler()
                }
            }
        }

        Column{
            id: idView
            width: parent.width
            height: 400 * hFactor

            // 列宽比例
            readonly property var columnFactor: [2, 3, 1, 2, 1, 1, 2]

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
                id: headerRow
                width: parent.width
                height: 30 * hFactor
                spacing: 1 * wFactor

                // 列标题
                readonly property var columnTitles: ["用户名", "内容", "IP", "网址", "收藏", "评论", "时间"]

                Repeater {
                    model: headerRow.columnTitles
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
                height: 300 * hFactor
                clip: true

                model: awemeModel

                delegate: Item{
                    width: idListView.width
                    height: 50 * hFactor

                    Row{
                        height: parent.height
                        spacing: 1 * wFactor

                        TextToolTip{
                            width: idView.columnWidth[0]
                            height: parent.height

                            text: nickname
                        }

                        TextToolTip{
                            width: idView.columnWidth[1]
                            height: parent.height

                            text: title
                        }

                        TextToolTip{
                            width: idView.columnWidth[2]
                            height: parent.height

                            text: ip_location
                        }

                        TextToolTip{
                            width: idView.columnWidth[3]
                            height: parent.height

                            text: aweme_url
                        }

                        TextToolTip{
                            width: idView.columnWidth[4]
                            height: parent.height

                            text: collected_count
                        }

                        TextToolTip{
                            width: idView.columnWidth[5]
                            height: parent.height

                            text: comment_count
                        }

                        TextToolTip{
                            width: idView.columnWidth[6]
                            height: parent.height

                            text: create_time
                        }
                    }

                    MouseArea{
                        anchors.fill: parent
                        onClicked: {
                            commentModel.setCurrentAwemeComment(aweme_id)
                            Context.showCommentWindow()
                        }
                    }
                }
            }
        }

    }

}
