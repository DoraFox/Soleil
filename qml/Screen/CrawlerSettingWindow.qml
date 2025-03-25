import QtQuick 2.15
import QtQuick.Controls 2.15

import Qt.labs.platform 1.1 as Platform

import QtQuick.Controls 1.4 as Controls

import Context 1.0

import "../Widget"

ScreenBase {
    id: root

    signal closeWindow()

    // 文件对话框
    Platform.FileDialog {
        id: fileDialog
        title: "选择浏览器路径"
        onAccepted: {
            browserPathField.text = fileDialog.file.toString().replace("file:///", "")
        }
    }

    Platform.FolderDialog {
        id: folderDialog
        title: "选择用户数据文件夹"
        options: Platform.FolderDialog.ShowDirsOnly
        onAccepted: {
            userDataField.text = folderDialog.currentFolder.toString().replace("file:///", "")
        }
    }

    // 日期选择对话框
    Dialog {
        id: datePickerDialog
        anchors.centerIn: parent
        width: 400 * wFactor
        height: 500 * hFactor
        modal: true

        standardButtons: Dialog.Ok | Dialog.Cancel

        contentItem: Controls.Calendar {
            id: idCalendar
        }

        onAccepted: {
            let formattedDate = Qt.formatDate(idCalendar.selectedDate, "yyyy-MM-dd") // 强制转换为字符串格式

            if (datePickerDialog.title == "选择开始时间") {
                commentStartTimeField.text = formattedDate
            } else {
                commentEndTimeField.text = formattedDate
            }
        }

    }

    Column {
        spacing: 20 * hFactor
        anchors.centerIn: parent

        // 浏览器端口
        Row {
            height: 50 * hFactor
            spacing: 10 * wFactor

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "浏览器端口:"
            }

            TextField {
                id: portField
                width: 150 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: systemSettings.port
                selectByMouse: true
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 1; top: 65535 }
            }
        }

        // 浏览器文件路径
        Row {
            height:  50 * hFactor
            spacing: 10 * wFactor

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "浏览器路径:"
            }

            TextField {
                id: browserPathField
                width: 300 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                selectByMouse: true
                text: systemSettings.browserPath
            }

            Button {
                width: 100 * wFactor
                height: parent.height
                text: "选择浏览器"
                font.pixelSize: FontConst.fontSize_16
                onClicked: {
                    fileDialog.open()
                }
            }
        }

        // 用户数据文件夹路径
        Row {
            height: 50 * hFactor
            spacing: 10 * wFactor

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "用户数据文件夹路径:"
            }

            TextField {
                id: userDataField
                width: 300 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                selectByMouse: true
                text: systemSettings.userDataPath
            }

            Button {
                width: 100 * wFactor
                height: parent.height
                text: "选择文件夹"
                font.pixelSize: FontConst.fontSize_16
                onClicked: {
                    var userDataPath = userDataField.text
                    if (userDataPath) {
                        // 检查路径是否以 "file:///" 开头
                        if (userDataPath.startsWith("file:///")) {
                            // 如果已包含前缀，则直接使用
                            folderDialog.folder = userDataPath;
                        } else {
                            // 如果不包含前缀，则添加 "file:///"
                            folderDialog.folder = "file:///" + userDataPath;
                        }
                    } else {
                        console.log("userDataPath is empty or undefined");
                    }
                    folderDialog.open()
                }
            }
        }

        // 视频获取数量
        Row {
            height: 50 * hFactor
            spacing: 10 * wFactor

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "视频获取数量:"
            }

            TextField {
                id: videoCountField
                width: 150 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: systemSettings.videoCount
                selectByMouse: true
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 1; top: 1000 }
            }

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "是否获取二级评论:"
            }

            Switch {
                id: getSecondLevelCommentsSwitch
                width: 100 * wFactor
                height: parent.height
                checked: systemSettings.getSecondLevelComments

                indicator: Rectangle {
                    implicitWidth: 100 * wFactor
                    implicitHeight: 40 * hFactor
                    x: getSecondLevelCommentsSwitch.leftPadding
                    y: parent.height / 2 - height / 2
                    radius: height / 2
                    color: getSecondLevelCommentsSwitch.checked ? "#17a81a" : "#ffffff"
                    border.color: getSecondLevelCommentsSwitch.checked ? "#17a81a" : "#cccccc"

                    Rectangle {
                        x: getSecondLevelCommentsSwitch.checked ? parent.width - width : 0
                        width: parent.height
                        height: width
                        radius: height / 2
                        color: getSecondLevelCommentsSwitch.down ? "#cccccc" : "#ffffff"
                        border.color: getSecondLevelCommentsSwitch.checked ? (getSecondLevelCommentsSwitch.down ? "#17a81a" : "#21be2b") : "#999999"
                    }
                }
            }
        }

        Row {
            height: 50 * hFactor
            spacing: 10 * wFactor

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "一级评论数量:"
            }

            TextField {
                id: firstLevelCommentsCountField
                width: 150 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: systemSettings.firstLevelCommentsCount
                selectByMouse: true
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 1; top: 5000 }
            }

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "二级评论数量:"
            }

            TextField {
                id: secondLevelCommentsCountField
                width: 150 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: systemSettings.secondLevelCommentsCount
                selectByMouse: true
                inputMethodHints: Qt.ImhDigitsOnly
                validator: IntValidator { bottom: 1; top: 500 }
            }
        }


        // 评论开始时间
        Row {
            height: 50 * hFactor
            spacing: 10 * wFactor

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "评论开始时间:"
            }

            TextField {
                id: commentStartTimeField
                width: 300 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: systemSettings.commentStartTime

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        datePickerDialog.title = "选择开始时间"
                        idCalendar.selectedDate = commentStartTimeField.text
                        datePickerDialog.open()
                    }
                }
            }
        }

        // 评论结束时间
        Row {
            height: 50 * hFactor
            spacing: 10 * wFactor

            Text {
                width: 150 * wFactor
                height: parent.height
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: FontConst.fontSize_16
                text: "评论结束时间:"
            }

            TextField {
                id: commentEndTimeField
                width: 300 * wFactor
                height: parent.height
                font.pixelSize: FontConst.fontSize_16
                text: systemSettings.commentEndTime

                MouseArea{
                    anchors.fill: parent
                    onClicked: {
                        datePickerDialog.title = "选择结束时间"
                        idCalendar.selectedDate = commentEndTimeField.text
                        datePickerDialog.open()
                    }
                }
            }
        }

        Item{
            width: 1
            height: parent.spacing
        }

        Row{
            height: 50 * hFactor
            spacing: 10 * wFactor

            // 保存按钮
            Button {

                width: 200 * wFactor
                height: 50 * hFactor
                font.pixelSize: FontConst.fontSize_16
                text: "保存设置"
                onClicked: {
                    var startTime = commentStartTimeField.text
                    var endTime = commentEndTimeField.text

                    if (startTime > endTime) {
                        Context.showInfoMessageBox("错误: 评论开始时间不能大于结束时间!")
                        return
                    }

                    // 如果时间合法，则赋值
                    systemSettings.commentStartTime = startTime
                    systemSettings.commentEndTime = endTime

                    systemSettings.browserPath = browserPathField.text
                    systemSettings.port = portField.text
                    systemSettings.userDataPath = userDataField.text

                    systemSettings.videoCount = parseInt(videoCountField.text)
                    systemSettings.getSecondLevelComments = getSecondLevelCommentsSwitch.checked
                    systemSettings.firstLevelCommentsCount = parseInt(firstLevelCommentsCountField.text)
                    systemSettings.secondLevelCommentsCount = parseInt(secondLevelCommentsCountField.text)

                    Context.showInfoMessageBox("设置成功!", closeWindow)
                }
            }

            // 重置按钮
            Button {

                width: 200 * wFactor
                height: 50 * hFactor
                font.pixelSize: FontConst.fontSize_16
                text: "重置"
                onClicked: {
                    systemSettings.initializeSettings(true)
                    Context.showInfoMessageBox("重置成功!", closeWindow)
                }
            }

            // 取消按钮
            Button {
                width: 200 * wFactor
                height: 50 * hFactor
                font.pixelSize: FontConst.fontSize_16
                text: "返回"
                onClicked: {
                    closeWindow()
                }
            }
        }

    }
}

