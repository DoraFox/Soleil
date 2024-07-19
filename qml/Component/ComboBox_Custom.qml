import QtQuick 2.12

import QtQuick.Controls 2.15

import Context 1.0


ComboBox {
     id: control

     property real wFactor: SizeConst.wFactor
     property real hFactor: SizeConst.hFactor
     property real rFactor: SizeConst.rFactor

     implicitWidth: 200 * wFactor
     implicitHeight: 40 * hFactor

     spacing: 10 * wFactor

     font{
         pixelSize: FontConst.normalSize
         bold: true
     }

     delegate: ItemDelegate {
         width: control.width
         contentItem: Text {
             text: modelData
             color: ColorConst.getLightColor()
             font: control.font
             elide: Text.ElideRight
             verticalAlignment: Text.AlignVCenter
         }
         highlighted: control.highlightedIndex === index
     }

     indicator: Canvas {
         id: canvas
         x: control.width - width - control.rightPadding
         y: control.topPadding + (control.availableHeight - height) / 2
         width: 12 * wFactor
         height: 8 * hFactor
         contextType: "2d"
         opacity: control.pressed ? 1 : 0.7

         onPaint: {
             context.reset();
             context.moveTo(0, 0);
             context.lineTo(width, 0);
             context.lineTo(width / 2, height);
             context.closePath();
             context.fillStyle = ColorConst.getDeepColor()
             context.fill();
         }
     }

     contentItem: Text {
         leftPadding: control.spacing
         rightPadding: control.indicator.width + control.spacing

         text: control.displayText
         font: control.font
         opacity: control.pressed ? 0.7 : 1
         verticalAlignment: Text.AlignVCenter
         elide: Text.ElideRight
     }

     background: Rectangle {
         anchors.fill: parent
         color: "white"
         border.color: ColorConst.getLightColor()
         border.width: 2 * wFactor
         radius: 2 * rFactor
     }

     popup: Popup {
         y: control.height
         width: control.width
         implicitHeight: contentItem.implicitHeight
         padding: 1 * wFactor

         contentItem: ListView {
             clip: true
             implicitHeight: contentHeight
             model: control.popup.visible ? control.delegateModel : null
             currentIndex: control.highlightedIndex

             ScrollIndicator.vertical: ScrollIndicator { }
         }
     }
 }
