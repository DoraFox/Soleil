import QtQuick 2.12

import Context 1.0


Loader {
    id: idLoader

    source: "../HotReload/hot_reload_widget.qml"


    function reload()
    {
        var sourcrUrl = idLoader.source
        idLoader.source = ""
        qmlEngine.clearCache()
        idLoader.source = sourcrUrl
    }

    Connections{
        target: Context

        function onTrigger_hot_reload(mouse)
        {
            if(mouse.accepted)
            {
                return
            }

            reload()
        }
    }

    Connections{
        id: idConnections
        target: testOpt

//        onEnabledChanged:{
//            console.log($msg, "Connections", _enabled, enabled)
//        }

        // error fuction
//        function onEnabledChanged(_enabled){
//            console.log($msg, testOpt.enabled, _enabled)
//        }
    }



}
