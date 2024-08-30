#include <QGuiApplication>

#include <midware/define/pathdefine.h>
#include "register/classregister.h"
#include "business/layout/layoutmanager.h"

#include <QIcon>
#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(testlog)
Q_LOGGING_CATEGORY(testlog, "testlog")

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL); // 解决QML 窗口缩小放大时闪烁的问题

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

    DEBUGPREFIX << "C++:" << __cplusplus;

    EnhancedQmlApplicationEngine engine;
#ifdef NOT_SHOW_QML
    ClassRegister classRegister(nullptr);
#else
    ClassRegister classRegister(&engine);
#endif
    classRegister.initial();

#ifndef NOT_SHOW_QML
    QObject::connect(&app, &QCoreApplication::aboutToQuit, classRegister.layoutMgr(), &LayoutManager::deleteAllLayout);
#endif

    DEBUGPREFIX << TRANSLATION_PATH_ZH << _STR(APP_ICON_NAME) << APP_ICON;
    app.setWindowIcon(QIcon(APP_ICON));

    return app.exec();
}
