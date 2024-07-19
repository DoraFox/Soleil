pragma Singleton

import QtQuick 2.12

QtObject {
    property real wFactor: SizeConst.wFactor
    property real hFactor: SizeConst.hFactor

    property int fontSize_8:  8 * hFactor
    property int fontSize_16: 16 * hFactor
    property int fontSize_24: 24 * hFactor
    property int fontSize_32: 32 * hFactor


    property int smallSize:  fontSize_16
    property int normalSize: fontSize_24
    property int largeSize:  fontSize_32

}
