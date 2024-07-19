import QtQuick 2.12

import Qt.labs.platform 1.1
import QtQuick.Controls 2.15

import Context 1.0
import Component 1.0

ScreenBase {
    id: root

    Column{
        anchors{
            left: parent.left
            leftMargin: 10 * wFactor
            top: parent.top
            topMargin: 10 * hFactor
        }

        spacing: 10* hFactor

        GroupBox{
            title: "Select CAN plugin"
            ComboBox_Custom {
                id: idComboBox_plugins

                model: canOpt.getCANPlugins()
                onCurrentTextChanged: {
                    canOpt.updateCurCANPlugin(currentText)
                }
            }
        }

        GroupBox{
            title: "Specify CAN interface name"
            ComboBox_Custom {
                id: idComboBox_interfaces

                model: canOpt.deviceNames
            }
        }

        GroupBox{
            title: "Specify CAN interface name"

            Text {
                id: name
                text: qsTr("text")
            }


        }

    }

}
