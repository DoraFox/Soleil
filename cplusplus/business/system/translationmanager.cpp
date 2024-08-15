#include "translationmanager.h"

#include <QUrlQuery>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QRegularExpression>

#include <QDomDocument>

#include "qcorotask.h"
#include "qcoronetworkreply.h"
#include "qcorotimer.h"

#include <midware/define/pathdefine.h>

const QString TranslationManager::TRANSLATE_APP_KEY = "";
const QString TranslationManager::TRANSLATE_APP_SECRET = "";

using QXlsx::Document;
using QXlsx::AbstractSheet;
using QXlsx::Worksheet;
using QXlsx::AbstractSheet;
using QXlsx::CellLocation;

TranslationManager::TranslationManager(QObject *parent)
    : QObject(parent)
    , m_translator(new QTranslator(this))
{
    initLanguage();

    // updateTsFileByExecl(R"(D:\Git_project\Soleil\resource\translation\translation.xlsx)",
    //                     R"(D:\Git_project\Soleil\resource\translation\Soleil_en.ts)",
    //                     Translate_English);

    // updateTsFileByExecl(R"(D:\Git_project\Soleil\resource\translation\translation.xlsx)",
    //                     R"(D:\Git_project\Soleil\resource\translation\Soleil_zh.ts)",
    //                     Translate_Chinese);
}

void TranslationManager::selectLanguage(int type)
{
    QString tmPath;
    switch(type)
    {
    case Translate_Chinese:
    {
        tmPath = TRANSLATION_PATH_ZH;
    }
        break;
    case Translate_English:
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

    ClassRegister::GetInstance()->engine()->retranslate();
}

QCoro::Task<> TranslationManager::translateText(TranslateTitle from, TranslateTitle to)
{
    static QHash<TranslateTitle, QString> languageTextHash = {
        {Translate_French,    "fr"},
        {Translate_Japanese,  "ja"},
        {Translate_Chinese,   "zh-CHS"},
        {Translate_ChineseTw, "zh-CHT"},
        {Translate_English,   "en"},
        {Translate_Auto,      "auto"},
    };

    if(!languageTextHash.contains(to) || !languageTextHash.contains(from))
    {
        co_return;
    }

    if(to == Translate_Auto) // can not set target language to "auto"
    {
        co_return;
    }

    if(from == to) // target language euqals to source language, no need to translate
    {
        co_return;
    }

    QString languageFrom = languageTextHash.value(from);
    QString languageTo = languageTextHash.value(to);

    TransInfoHash::iterator iter = m_transInfoHash.begin();

    while (iter != m_transInfoHash.end())
    {
        const QString& translateText = iter.value().value(from);
        QString& destText = iter.value()[to];

        if(translateText.isEmpty() || !destText.isEmpty())
        {
            ++iter;
            continue;
        }

        QString salt = QString::number(QDateTime::currentMSecsSinceEpoch());
        QString curtime = QString::number(QDateTime::currentSecsSinceEpoch());
        QString sign = generateSign(translateText, salt, curtime);

        QUrl url("https://openapi.youdao.com/api");
        QUrlQuery query;
        query.addQueryItem("q", translateText);
        query.addQueryItem("from", languageFrom);
        query.addQueryItem("to", languageTo);
        query.addQueryItem("appKey", TRANSLATE_APP_KEY);
        query.addQueryItem("salt", salt);
        query.addQueryItem("sign", sign);
        query.addQueryItem("signType", "v3");
        query.addQueryItem("curtime", curtime);

        url.setQuery(query);

        QString result = co_await translateWord(url);

        if(!result.isEmpty())
        {
            destText = result;
        }

        ++iter;
        co_await QCoro::sleepFor(std::chrono::seconds(1));
    }

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

QString TranslationManager::generateSign(const QString &q, const QString &salt, const QString &curtime)
{
    QString str = TRANSLATE_APP_KEY + q + salt + curtime + TRANSLATE_APP_SECRET;
    QByteArray hash = QCryptographicHash::hash(str.toUtf8(), QCryptographicHash::Sha256);

    return hash.toHex();
}

void TranslationManager::translateExcel(const QUrl &url)
{
    static auto innerFunc = [this](QUrl url)->QCoro::Task<>{
        QString docPath = url.isLocalFile() ? url.toLocalFile() : url.toString();
        bool success = getTransInfoHash(docPath);
        if(success)
        {
            // use co_await to wait for the translation to finish one by one
            co_await translateText(Translate_Chinese, Translate_English);
            co_await translateText(Translate_Chinese, Translate_ChineseTw);

            writeTrans2Execl(docPath);
        }
    };

    innerFunc(url);
}

QStringList TranslationManager::extractSourceContent(const QString &fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open file:" << fileName;
        return QStringList();
    }

    QTextStream in(&file);
    QString fileContent = in.readAll();
    file.close();

    // 正则表达式设置为跨行匹配
    static QRegularExpression regex("<source>(.*?)</source>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatchIterator matches = regex.globalMatch(fileContent);

    QStringList contents;
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        contents.append(match.captured(1)); // 提取第一个捕获组的内容
    }

    DEBUGPREFIX << "find matched count:" << contents.size();

    return contents;
}

QCoro::Task<QString> TranslationManager::translateWord(QUrl url)
{
    static QNetworkAccessManager mgr;
    static QNetworkRequest request;

    QString result;

    request.setUrl(url);

    auto* reply = co_await mgr.get(request);

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray response = reply->readAll();
        QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
        QJsonObject jsonObj = jsonDoc.object();

        if (jsonObj.contains("translation"))
        {
            QJsonArray translationArray = jsonObj["translation"].toArray();
            DEBUGPREFIX << translationArray.toVariantList().toVector();

            if(!translationArray.isEmpty())
            {
                result = translationArray.first().toString();
            }
        }
        else
        {
            DEBUGPREFIX << "Error:" << url << jsonObj;
        }
    } else
    {
        DEBUGPREFIX << "Error: " << url << reply->errorString();
    }

    reply->deleteLater();

    co_return result;
}

bool TranslationManager::getTransInfoHash(const QString &excelFilePath)
{
    m_transInfoHash.clear();

    Document xlsx(excelFilePath);

    if(!xlsx.load())
    {
        DEBUGPREFIX << "ERROR: fail to load xlsx doc:";
        return false;
    }

    const QStringList& sheetNames = xlsx.sheetNames();

    if(sheetNames.isEmpty())
    {
        DEBUGPREFIX << "ERROR: no sheets found in xlsx file";
        return false;
    }

    Worksheet* currentSheet = reinterpret_cast<Worksheet*>(xlsx.sheet( sheetNames.first()));
    if ( nullptr == currentSheet )
    {
        DEBUGPREFIX << "currentSheet is NULL";
        return false;
    }

    // get full cells of current sheet
    int rowCount = currentSheet->dimension().lastRow();
    int columnCount = currentSheet->dimension().lastColumn();

    if(columnCount < TranslationManager::ColumnBegin || rowCount < 1)
    {
        DEBUGPREFIX << rowCount << columnCount;
        return false;
    }

    for(int i = TranslationManager::ColumnBegin; i < rowCount + 1; ++i)
    {
        QString key = currentSheet->read(i, TranslationManager::Translate_Code).toString();
        if(key.isEmpty())
        {
            continue;
        }

        QString strZh = currentSheet->read(i, TranslationManager::Translate_Chinese).toString();
        QString strZhTw = currentSheet->read(i, TranslationManager::Translate_ChineseTw).toString();
        QString strEn = currentSheet->read(i, TranslationManager::Translate_English).toString();

        m_transInfoHash.insert(key,
                               {
                                   {TranslationManager::Translate_Chinese, strZh},
                                   {TranslationManager::Translate_ChineseTw, strZhTw},
                                   {TranslationManager::Translate_English, strEn}
                               });
        DEBUGPREFIX << key << strZh << strEn;
    }
    return true;
}

void TranslationManager::writeTrans2Execl(const QString &excelFilePath)
{
    Document xlsx(excelFilePath);

    if(!xlsx.load())
    {
        DEBUGPREFIX << "ERROR: fail to load xlsx doc:" << excelFilePath;
        return;
    }

    const QStringList& sheetNames = xlsx.sheetNames();

    if(sheetNames.isEmpty())
    {
        DEBUGPREFIX << "ERROR: no sheets found in xlsx file";
        return;
    }

    Worksheet* currentSheet = reinterpret_cast<Worksheet*>(xlsx.sheet( sheetNames.first()));
    if ( nullptr == currentSheet )
    {
        DEBUGPREFIX << "currentSheet is NULL";
        return;
    }

    // get full cells of current sheet
    int rowCount = currentSheet->dimension().lastRow();
    int columnCount = currentSheet->dimension().lastColumn();

    if(columnCount < TranslationManager::ColumnBegin || rowCount < 1)
    {
        DEBUGPREFIX << rowCount << columnCount;
        return;
    }

    for(int i = TranslationManager::ColumnBegin; i < rowCount + 1; ++i)
    {
        QString code = currentSheet->read(i, TranslationManager::Translate_Code).toString();

        if(m_transInfoHash.contains(code))
        {
            const QHash<int, QString>& value = m_transInfoHash[code];
            QHash<int, QString>::ConstIterator iter = value.cbegin();

            while (iter != value.cend())
            {
                int language = iter.key();
                QString targetText = iter.value();

                currentSheet->write(i, language, targetText);

                ++iter;
            }
        }
    }

    bool succeed = xlsx.save();
    DEBUGPREFIX << "Save xlsx:" << succeed;
}

void TranslationManager::updateTsFileByExecl(const QString& excelFilePath, const QString& tsFilePath, TranslateTitle language)
{
    bool succeed = getTransInfoHash(excelFilePath);

    if(!succeed)
    {
        return;
    }

    // 加载 .ts 文件
    QFile tsFile(tsFilePath);
    if(!tsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DEBUGPREFIX << "Failed to open ts file:" << tsFilePath;
        return;
    }

    QDomDocument doc;
    if(!doc.setContent(&tsFile))
    {
        DEBUGPREFIX << "Failed to parse .ts file:" << tsFilePath;
        tsFile.close();
        return;
    }
    tsFile.close();

    QDomElement root = doc.documentElement();
    QDomNodeList messages = root.elementsByTagName("message");

    // 更新翻译内容
    for(int i = 0; i < messages.count(); ++i)
    {
        QDomNode node = messages.at(i);
        QDomElement sourceElem = node.firstChildElement("source");
        QDomElement translationElem = node.firstChildElement("translation");

        QString sourceText = sourceElem.text();
        if (!sourceText.isEmpty() && m_transInfoHash.contains(sourceText))
        {
            QString type =  translationElem.attribute("type");
            if(type == "unfinished")
            {
                translationElem.removeAttribute("type");
            }

            QString newText = m_transInfoHash.value(sourceText).value(language);

            QDomNode childNode = translationElem.firstChild();
            if(childNode.nodeValue().isEmpty())
            {
                QDomText textNode = doc.createTextNode(newText);
                translationElem.appendChild(textNode);
            }
            else
            {
                childNode.setNodeValue(newText);
            }
        }
    }

    // 保存修改后的 .ts 文件
    QFile outFile(tsFilePath);
    if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        DEBUGPREFIX << "Failed to save .ts file:" << tsFilePath;
        return;
    }

    QTextStream outStream(&outFile);
    doc.save(outStream, 4); // 使用缩进级别 4
    outFile.close();

    DEBUGPREFIX << "Translations updated successfully: " << tsFilePath;
}

