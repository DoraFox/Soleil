import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQml 2.12

import Context 1.0


Text {

    id: root

    text: "<font color='red'>Hello </font><font color='green'>Qt!</font>"

    font.pixelSize: FontConst.normalSize
    style: Text.Raised
    styleColor: "gray"

    function setText()
    {
        var message = "There are %1 items %2"
        var count = 20
        console.log(message.arg(count).arg("abc"))
    }

    Component.onCompleted: {
        setText()
    }

}
