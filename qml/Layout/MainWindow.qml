// Qt
import QtQuick 2.12
import QtQuick.Controls 2.12

// C++

// qmldir
import Context 1.0


// path
import "../Widget"
import "../Screen"

Item{
    anchors.fill: parent

    SwipeView{
        id: idView
        anchors.fill: parent

        HotReloadWidget{

        }

        AscFile{

        }

        DownloadFile{

        }

        TextOperate{

        }

        FileOperate{

        }

        ExcelTransform{

        }

        SwapMouseButton{

        }

        TextDisplay{

        }
    }

    PageIndicator {
         id: idIndicator

         interactive: true
         count: idView.count
         currentIndex: idView.currentIndex

         anchors.bottom: idView.bottom
         anchors.horizontalCenter: parent.horizontalCenter

         onCurrentIndexChanged: {
             idView.currentIndex = currentIndex
         }
    }
}


