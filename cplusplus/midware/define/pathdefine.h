#ifndef PATHDEFINE_H
#define PATHDEFINE_H

#include <QCoreApplication>

#include "basedefine.h"

#define FILE_PATH(path) "file:///" _STR(PROJECT_DIR) path

#define APP_DIR_RELATIVE_PATH   "../"

#define APP_DIR_PTH             QCoreApplication::applicationDirPath()

#define TRANSLATION_PATH        APP_DIR_PTH + CONTACT_STR(/, TRANSLATION_RELATIVE_PATH, /)
#define TRANSLATION_PATH_ZH     TRANSLATION_PATH + CONTACT_STR(ZH_NAME, .qm)
#define TRANSLATION_PATH_EN     TRANSLATION_PATH + CONTACT_STR(EN_NAME, .qm)
#define TRANSLATION_PATH_EXCEL  TRANSLATION_PATH + _STR(EXCEL_NAME)

#if DYNAMIC_QML_RESOURCE
#define GET_QML_PATH(name)      APP_DIR_RELATIVE_PATH "../qml/" name
#else
#define GET_QML_PATH(name)      QStringLiteral("qrc:/" name)
#endif

#define APP_ICON_PATH           APP_DIR_PTH + CONTACT_STR(/, IMAGE_RELATIVE_PATH, /)
#define Get_APP_ICON(name)      APP_ICON_PATH name
#define APP_ICON                Get_APP_ICON(_STR(APP_ICON_NAME))

#endif // PATHDEFINE_H
