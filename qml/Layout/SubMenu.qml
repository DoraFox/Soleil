// Qt
import QtQuick 2.15
import QtQuick.Controls 2.15

// C++

// qmldir
import Context 1.0


// path
import "../Widget"
import "../Screen"

LayoutBase{
    id: root
    anchors.fill: parent

    CommentWindow{
        id: idCommentWindow
        Connections{
            target: Context

            function onShowCommentWindow(){
                root.visible = true
                idCommentWindow.visible = true
                console.log($msg, idCommentWindow.visible, root.width, root.height)
            }
        }

        Button{
            width: 100
            height: 50
            text: "返回"
            onClicked: {
                root.visible = false
            }
        }
    }

}
