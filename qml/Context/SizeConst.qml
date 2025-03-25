pragma Singleton

import QtQuick 2.12

QtObject {
    property real wFactor: 1
    property real hFactor: 1
    property real rFactor: hFactor

    readonly property int windowWidth: 1200
    readonly property int windowHeight: 800

    readonly property int dialogWidth: windowWidth / 2
    readonly property int dialogHeight: windowHeight / 2
}
