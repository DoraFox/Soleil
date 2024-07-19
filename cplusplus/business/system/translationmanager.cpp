#include "translationmanager.h"

#include <midware/define/pathdefine.h>

TranslationManager::TranslationManager(QObject *parent)
    : QObject(parent)
    , m_translator(new QTranslator(this))
{
    initLanguage();
    initialErrorCode();
}

void TranslationManager::selectLanguage(int type)
{
    QString tmPath;
    switch(type)
    {
    case Translate_ZH:
    {
        tmPath = TRANSLATION_PATH_ZH;
    }
        break;
    case Translate_EN:
    {
        tmPath = TRANSLATION_PATH_EN;
    }
        break;
    default:
    {
        DEBUGPREFIX << "error type:" << type;
    }
        return;
    }

    bool flag = m_translator->load(tmPath) && qApp->installTranslator(m_translator);

    if(!flag)
    {
        return;
    }

    //qDebug() << QDateTime::currentDateTime().toMSecsSinceEpoch();
    initialErrorCode();
    //qDebug() << QDateTime::currentDateTime().toMSecsSinceEpoch();
    ClassRegister::GetInstance()->engine()->retranslate();
}

QString TranslationManager::getTranslation(int code)
{
//    DEBUGPREFIX << m_errorCodeHash.value(code, QString());
    return m_errorCodeHash.value(code, QString());
}

void TranslationManager::translateProject(const QUrl &url)
{
    if(translateProFile("lupdate.exe"))
    {
        ClassRegister* reg = ClassRegister::GetInstance();
        reg->excelOpt()->convertExcelToTS(url);
        //translateProFile("lrelease");
    }
}

bool TranslationManager::translateProFile(QString command)
{
    QProcess process;
    QStringList arguments;
    arguments << "-extensions" << "qml,cpp" << "-recursive" << _STR(CPP_PATH) << _STR(QML_PATH)
              << "-ts" << _STR(TRANSLATION_RELATIVE_PATH) "../Soleil_en.ts" << "../Soleil_zh.ts";

    process.start("lupdate.exe", arguments);

//    process.waitForFinished();

    // arguments.clear();
    // arguments << "-extensions" << "qml,cpp" << "-recursive" << APP_DIR_RELATIVE_PATH << "-ts" << "../Soleil_zh.ts";

    // process.start("lupdate.exe", arguments);

    return process.waitForFinished();
}

void TranslationManager::initLanguage()
{
    QString locale = QLocale::system().name();
    QString fileName = CONTACT_STR(PROJECT_NAME, _) + locale.split('_').first();
    QString fileDir = TRANSLATION_PATH;
    bool flag = m_translator->load(fileName, fileDir);
    if(flag)
    {
        qApp->installTranslator(m_translator);
    }
    else
    {
        DEBUGPREFIX_ERROR << flag << locale << fileName << fileDir;
    }
}

void TranslationManager::initialErrorCode()
{
    //m_errorCodeHash;
}
