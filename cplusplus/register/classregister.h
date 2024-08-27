#ifndef CLASSREGISTER_H
#define CLASSREGISTER_H

#include "business/can/logfileopt.h"
#include <QObject>
#include <QQmlContext>

#include <midware/qtenhanced/enhancedqmlapplicationengine.h>
#include <midware/define/basedefine.h>

#define PropertyPair(name, value) QQmlContext::PropertyPair{QString(name), QVariant::fromValue(value)}

class Dandelion;
class SystemSettings;
class TranslationManager;
class LayoutManager;
class NetworkManager;
class MainWindowControl;
class FileOperate;
class TextOperate;
class TestOperate;
class NetworkOperate;
class S19Manager;
class CCPCalibration;

class LearnOpengl;

#ifdef USE_CAN_LIN
class LinManager;
class CANOperate;
class LogFileOpt;
#endif

class ClassRegister : public QObject
{
    Q_OBJECT
public:
    Q_DISABLE_COPY(ClassRegister)

    static ClassRegister* GetInstance()
    {
        return m_instance;
    }

    explicit ClassRegister(EnhancedQmlApplicationEngine *engine, QObject *parent = nullptr);

    ~ClassRegister();

    void initial();

    void testFunction();

    void tessheetstyletFunction();

    void registerClass();

    void registerType();

    void registerContext();

signals:
    void finishRegister();

private:

    static ClassRegister* m_instance;

    DECLARE_PROPERTY(EnhancedQmlApplicationEngine*, engine)

    DECLARE_PROPERTY(Dandelion*, dandelion)

    DECLARE_PROPERTY(TranslationManager*, translationMgr)
    DECLARE_PROPERTY(LayoutManager*, layoutMgr)
    DECLARE_PROPERTY(NetworkManager*, networkMgr)

    DECLARE_PROPERTY(MainWindowControl*, mainWindowCtl)

    DECLARE_PROPERTY(SystemSettings*, systemSettings)
    DECLARE_PROPERTY(FileOperate*, fileOpt)
    DECLARE_PROPERTY(TextOperate*, textOpt)

    DECLARE_PROPERTY(TestOperate*, testOpt)

    DECLARE_PROPERTY(CCPCalibration*, ccpCal)
    DECLARE_PROPERTY(S19Manager*, s19Mgr)

    DECLARE_PROPERTY(LearnOpengl*, learnopengl)

#ifdef USE_CAN_LIN
    DECLARE_PROPERTY(LinManager*, linMgr)
    DECLARE_PROPERTY(CANOperate*, canOpt)
    DECLARE_PROPERTY(LogFileOpt*, logFileOpt)
#endif

};

#endif // CLASSREGISTER_H
