pragma Singleton

import QtQuick 2.12

QtObject {
    property real wFactor: 1
    property real hFactor: 1
    property real rFactor: hFactor

    property int windowWidth: 600
    property int windowHeight: 400
}
