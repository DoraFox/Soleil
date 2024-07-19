import QtQuick 2.12
import QtQuick.Shapes 1.12
import Context 1.0


import cplusplus.Component 1.0

import Component 1.0

Button_Normal{
    id: root

    implicitWidth: 400 * SizeConst.wFactor
    implicitHeight: 200 * SizeConst.hFactor

    text: qsTr("_T1007")

    mouseArea.enabled: false

    signal trigger_hot_reload(var mouse)

    Button_Normal{
        implicitWidth: 100 * SizeConst.wFactor
        implicitHeight: implicitWidth
        text: qsTr("_T1007")
        radius: width / 2
        onClicked: {
            mouse.accepted = false
            trigger_hot_reload(mouse)
        }
    }

    signal btnClicked(var mouse)

    Row{
        x: 50 * SizeConst.wFactor
        y: 50 * SizeConst.hFactor
        spacing: 10 * SizeConst.wFactor

        Torus{
            id: idTorus

            width: 58 * SizeConst.wFactor
            height: width

            strokeWidth: 3 * SizeConst.wFactor
            startAngle: 0
            arcLength: 360
            color: "#F57C64"

            visible: true

            PropertyAnimation{
                duration: 5000
                from: 0
                to: 360
                loops: 10
                running: false
                target: idTorus
                property: "arcLength"
            }
        }

        Canvas{
            antialiasing: true

            width: 58 * SizeConst.wFactor
            height: width

            onWidthChanged: {
                requestPaint()
            }

            onHeightChanged: {
                requestPaint()
            }

            onPaint: {
                var ctx = getContext("2d")
                ctx.lineWidth = 3 * SizeConst.hFactor
                ctx.strokeStyle = "#F57C64"

                ctx.clearRect(0, 0, width, height)
                ctx.arc(width / 2, height / 2, (width - ctx.lineWidth) / 2, 0, 2 * Math.PI, false)
                ctx.stroke()
            }
        }

    }

    Arc{
        id: idArc1
        anchors.centerIn: parent
        width: 200 * SizeConst.wFactor
        height: width
        strokeWidth: 15 * SizeConst.hFactor
        startAngle: 0
        arcLength: 180
        color: ColorConst.getDeepColor()
        visible: true

        PropertyAnimation{
            target: idArc1
            property: "arcLength"
            from: 0
            to: 360
            loops: 100
            running: true
            duration: 10000
        }

    }

    Arc{
        id: idArc2
        anchors.centerIn: parent
        width: idArc1.width
        height: idArc1.height
        strokeWidth: idArc1.strokeWidth
        startAngle: idArc1.startAngle + idArc1.arcLength
        arcLength: 360 - idArc1.arcLength
        color: ColorConst.getDeepColor()
        //visible: false
    }

    Loader{
        id: idLoader
        active: false
        sourceComponent: Item{
            property bool bEnabled: testOpt.enabled

            function onBEnabled() {
                console.log(StringConst.msg, "bEnabled1111:", bEnabled)
            }

            Component.onCompleted: {
                console.log(StringConst.msg, "complete:", testOpt.enabled, bEnabled)

                testOpt.enabledChanged.connect(onBEnabled)
            }
        }

        Timer{
            id: idTimer
            running: false
            interval: 1000
            onTriggered: {
                idLoader.active = true
            }
        }
    }

    onClicked:{
        testOpt.enabled = !testOpt.enabled
        mouse.accepted = false
        btnClicked(mouse)
    }

    Component.onCompleted: {
        idTimer.start()
    }



}
