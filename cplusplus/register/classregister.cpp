#include "classregister.h"

#include <midware/tools/dandelion.h>
#include <midware/qtenhanced/mainwindowcontrol.h>

#include "componentregister.h"
#include "business/system/translationmanager.h"
#include "business/layout/layoutmanager.h"
#include "business/network/networkmanager.h"

#include "business/file/exceloperate.h"
#include "business/system/systemsettings.h"
#include "business/file/fileoperate.h"
#include "business/file/textoperate.h"
#include "business/features/testoperate.h"
#include "business/calibration/s19/s19manager.h"

#include "business/calibration/ccpcalibration.h"
#include "business/opengl/learnopengl.h"

#ifdef USE_CAN_LIN
#include "business/lin/linmanager.h"
#include "business/can/canoperate.h"
#include "business/can/logfileopt.h"
#endif

ClassRegister* ClassRegister::m_instance = nullptr;

ClassRegister::ClassRegister(EnhancedQmlApplicationEngine *engine, QObject *parent)
    : QObject{parent}
    , m_engine(engine)
    , m_dandelion(nullptr)
    , m_translationMgr(nullptr)
    , m_networkMgr(nullptr)
    , m_mainWindowCtl(nullptr)
    , m_systemSettings(nullptr)
    , m_excelOpt(nullptr)
    , m_fileOpt(nullptr)
    , m_textOpt(nullptr)
    , m_testOpt(nullptr)
    , m_ccpCal(nullptr)
    , m_s19Mgr(nullptr)
    , m_learnopengl(nullptr)
#ifdef USE_CAN_LIN
    , m_canOpt(nullptr)
    , m_linMgr(nullptr)
    , m_logFileOpt(nullptr)
#endif
{
    m_instance = this;
}

ClassRegister::~ClassRegister()
{
    m_instance = nullptr;
}

void ClassRegister::initial()
{
    registerClass();
    registerType();
    registerContext();

    testFunction();

    emit finishRegister();
}

void ClassRegister::testFunction()
{

    m_systemSettings->testFunc();

#ifdef Q_OS_LINUX
    system("sync");
#endif
}

void ClassRegister::registerClass()
{
    m_dandelion = new Dandelion(this);
    m_excelOpt = new ExcelOperate(this);
    m_systemSettings = new SystemSettings(this);
    m_translationMgr = new TranslationManager(this);
    m_networkMgr = new NetworkManager(this);

    //m_learnopengl = new LearnOpengl(this);
#ifdef USE_CAN_LIN
    m_linMgr = new LinManager(this);
    m_canOpt = new CANOperate(this);
    m_logFileOpt = new LogFileOpt(this);
#endif

    m_mainWindowCtl = new MainWindowControl(this);
    m_fileOpt = new FileOperate(this);
    m_textOpt = new TextOperate(this);
    m_testOpt = new TestOperate(this);

    m_ccpCal = new CCPCalibration(this);
    m_s19Mgr = new S19Manager(this);

    if(m_engine)
    {
        m_layoutMgr = new LayoutManager(m_engine, this);
        connect(m_layoutMgr, &LayoutManager::qmlMainWindowCreated, this, [&]()
                {
                    m_mainWindowCtl->set_mainWindow(m_layoutMgr->getMainWindow());
                    m_layoutMgr->initLayout();
                });

        connect(this, &ClassRegister::finishRegister, m_layoutMgr, &LayoutManager::loadMainQML);
    }
}

void ClassRegister::registerType()
{
    RegisterQmlType(Dandelion, 1, 0);
    RegisterQmlType(ExcelOperate, 1, 0);
    RegisterQmlType(SystemSettings, 1, 0);
    RegisterQmlType(TranslationManager, 1, 0);
    //RegisterQmlType(LayoutManager, 1, 0);
    RegisterQmlType(NetworkOperate, 1, 0);
    RegisterQmlType(NetworkManager, 1, 0);

#ifdef USE_CAN_LIN
    RegisterQmlType(LogFileOpt, 1, 0);
#endif
}

void ClassRegister::registerContext()
{
    if(nullptr == m_engine)
    {
        return;
    }

    QQmlContext* ptrContext = m_engine->rootContext();

    ComponentRegister::RegistQMLComponent(ptrContext);

#ifdef QT_DEBUG

    ptrContext->setContextProperty("$msg", _STR(MSGPREFIX));

    ptrContext->setContextProperty("qmlEngine", m_engine);

    ptrContext->setContextProperty("dandelion", m_dandelion);

    ptrContext->setContextProperty("excelOpt", m_excelOpt);
    ptrContext->setContextProperty("funcMgr", m_systemSettings);
    ptrContext->setContextProperty("translationMgr", m_translationMgr);
    ptrContext->setContextProperty("layoutMgr", m_layoutMgr);

    ptrContext->setContextProperty("mainWindowCtl", m_mainWindowCtl);
    ptrContext->setContextProperty("fileOpt", m_fileOpt);
    ptrContext->setContextProperty("textOpt", m_textOpt);
    ptrContext->setContextProperty("testOpt", m_testOpt);
    ptrContext->setContextProperty("networkMgr", m_networkMgr);
    ptrContext->setContextProperty("s19Mgr", m_s19Mgr);


#ifdef USE_CAN_LIN
    ptrContext->setContextProperty("linMgr", m_linMgr);
    ptrContext->setContextProperty("canOpt", m_canOpt);
    ptrContext->setContextProperty("logFileOpt", m_logFileOpt);
#endif

#else

    QVector<QQmlContext::PropertyPair> properties = {
        PropertyPair("$msg", QString(_STR(MSGPREFIX))),

        PropertyPair("qmlEngine", m_engine),
        PropertyPair("dandelion", m_dandelion),
        PropertyPair("excelOpt", m_excelOpt),
        PropertyPair("funcMgr", m_systemSettings),
        PropertyPair("translationMgr", m_translationMgr),
        PropertyPair("layoutMgr", m_layoutMgr),
        PropertyPair("mainWindowCtl", m_mainWindowCtl),
        PropertyPair("fileOpt", m_fileOpt),
        PropertyPair("textOpt", m_textOpt),
        PropertyPair("networkMgr", m_networkMgr),
        PropertyPair("s19Mgr", m_s19Mgr),
        PropertyPair("logFileOpt", m_logFileOpt),
    };

    ptrContext->setContextProperties(properties);

#endif

}
