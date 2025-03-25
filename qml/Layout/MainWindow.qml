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
    anchors.fill: parent

    // HotReloadWidget{

    // }

    CrawlerWindow{

        onShowCommentWindow: {
            idCommentWindow.active = true
        }

        onShowCrawlerSettingWindow: {
            idCrawlerSettingWindow.active = true
        }

    }

    Loader{
        id: idCommentWindow

        anchors.fill: parent

        active: false

        sourceComponent: CommentWindow {
            onCloseWindow: {
                idCommentWindow.active = false
            }
        }
    }


    Loader{
        id: idCrawlerSettingWindow

        anchors.fill: parent

        active: false

        sourceComponent: CrawlerSettingWindow {
            onCloseWindow: {
                idCrawlerSettingWindow.active = false
            }
        }
    }

}


