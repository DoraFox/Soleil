#ifndef PATHDEFINE_H
#define PATHDEFINE_H

#include <QCoreApplication>

#include "basedefine.h"

#define FILE_PATH(path) "file:///" _STR(PROJECT_DIR) path

#define APP_DIR_PTH             QCoreApplication::applicationDirPath()

#define TRANSLATION_PATH        APP_DIR_PTH + CONTACT_STR(/, TRANSLATION_RELATIVE_PATH, /)
#define TRANSLATION_PATH_ZH     TRANSLATION_PATH + CONTACT_STR(ZH_NAME, .qm)
#define TRANSLATION_PATH_EN     TRANSLATION_PATH + CONTACT_STR(EN_NAME, .qm)
#define TRANSLATION_PATH_EXCEL  TRANSLATION_PATH + _STR(EXCEL_NAME)

#if DYNAMIC_QML_RESOURCE
#ifdef QT_DEBUG
#define GET_QML_PATH(name)      "file:///" + APP_DIR_PTH + "/../../qml/" name
#else
#define GET_QML_PATH(name)      "file:///" + APP_DIR_PTH + "/qml/" name
#endif
#else
#define GET_QML_PATH(name)      QStringLiteral("qrc:/" name)
#endif

#define APP_ICON_PATH           APP_DIR_PTH + CONTACT_STR(/, IMAGE_RELATIVE_PATH, /)
#define Get_APP_ICON(name)      APP_ICON_PATH name
#define APP_ICON                Get_APP_ICON(_STR(APP_ICON_NAME))

#define SQL_DRIVER_DIR          APP_DIR_PTH + "/sqldrivers"
#define SQLITE_DB_PATH          APP_DIR_PTH + "/crawler.db"


#define CONFIG_FILE_PATH        APP_DIR_PTH + "/config.ini"
#define MESSAGE_DATA_PATH       APP_DIR_PTH + "/message_data.ini"

#define DEFAULT_MESSAGE_DATA QStringList({"消息1", "消息2", "消息3"})

#endif // PATHDEFINE_H
