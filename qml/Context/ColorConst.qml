pragma Singleton

import QtQuick 2.12
import Dandelion 1.0

QtObject {
    enum Color{
        LightBlue = 0,
        SkyBlue,
        Blue,
        Pink,
        HotPink,
        Count
    }

    property var colorArray: new Array(ColorConst.Color.Count)

    function getRamdom(min, max) // [min, max)
    {
        return Math.random() * (max - min) + min
    }

    function getLightColor()
    {
        return dandelion.getLightColor()
    }

    function getDeepColor()
    {
        return dandelion.getDeepColor()
    }

    Component.onCompleted: {
        colorArray[ColorConst.Color.LightBlue] = "#FFADD8E6"
        colorArray[ColorConst.Color.SkyBlue] = "SkyBlue"
        colorArray[ColorConst.Color.Blue] = "Blue"
        colorArray[ColorConst.Color.Pink] = "Pink"
        colorArray[ColorConst.Color.HotPink] = "HotPink"
    }

}
