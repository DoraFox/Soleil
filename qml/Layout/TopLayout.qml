import QtQuick 2.12
import Context 1.0

import Component 1.0

// path
import "../Widget"

Item{
    id: root
    anchors.fill: parent

    TopHint{
        id: idTopHint
        x: 0
        y: (parent.height - height) / 2

        mouseArea{
            drag{
                target: idTopHint
                axis: Drag.XAndYAxis
                //filterChildren: true
                minimumX: 0
                maximumX: parent.width - width
                minimumY: 0
                maximumY: parent.height - height
            }
        }
    }

    Button_HotReload{
        id: idBtn_hotreload
        x: parent.width - width
        y: (parent.height - height) / 2

        mouseArea{
            drag{
                target: idBtn_hotreload
                axis: Drag.XAndYAxis
                //filterChildren: true
                minimumX: 0
                maximumX: root.width - idBtn_hotreload.width
                minimumY: 0
                maximumY: root.height - idBtn_hotreload.height
            }
        }

    }
}
