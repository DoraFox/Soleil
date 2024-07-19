pragma Singleton

import QtQuick 2.12

QtObject {
    signal trigger_hot_reload(var mouse)

    signal showInfoMessageBox(var text)
}
