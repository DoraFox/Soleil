#include "translationmanager.h"

#include <QTranslator>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QRegularExpression>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>

#include <xlsxworkbook.h>
#include <xlsxdocument.h>

#include "qcorotask.h"
#include "qcoronetworkreply.h"
#include "qcorotimer.h"

#include "register/classregister.h"
#include <midware/define/pathdefine.h>

const QString TranslationManager::TRANSLATE_APP_KEY = "7cc41a4f2bb1c1bf";
const QString TranslationManager::TRANSLATE_APP_SECRET = "nas7tRjrFj782mrVwHK4WZCRzlglIbRj";

using QXlsx::Document;
using QXlsx::AbstractSheet;
using QXlsx::Worksheet;

// 定义全局静态 QHash
typedef QHash<int, QString> LanguageHash;
Q_GLOBAL_STATIC_WITH_ARGS(LanguageHash, languageTextHash,
                          ({
                            {TranslationManager::Translate_French,    "fr"},
                            {TranslationManager::Translate_Japanese,  "ja"},
                            {TranslationManager::Translate_Chinese,   "zh-CHS"},
                            {TranslationManager::Translate_ChineseTw, "zh-CHT"},
                            {TranslationManager::Translate_Korean,    "ko"},
                            {TranslationManager::Translate_English,   "en"},
                            {TranslationManager::Translate_Auto,      "auto"},
                            }))

TranslationManager::TranslationManager(QObject *parent)
    : QObject(parent)
    , m_translator(new QTranslator(this))
{
/*
    initLanguage();

    QString src_path = R"(/home/boolping/Git_project/ok-msg-desktop/src/)";

    // m_code_list = extractSourceContent(src_path + R"(modules/meet/ts/zh_CN.xml)", "unfinished");
    // writeCodes2Execl(m_code_list, src_path + R"(modules/meet/ts/translation.xlsx)");

    // m_code_list = extractSourceContent(src_path + R"(UI/window/main/ts/zh_CN.ts)", "unfinished");
    // writeCodes2Execl(m_code_list, src_path + R"(UI/window/main/ts/translation.xlsx)");

    // m_code_list = extractSourceContent(src_path + R"(UI/window/login/ts/zh_CN.ts)", "unfinished");
    // writeCodes2Execl(m_code_list, src_path + R"(UI/window/login/ts/translation.xlsx)");

    //extractTranslationContent(R"(/home/boolping/Git_project/ok-msg-desktop/src/modules/im/translations/zh_TW.xml)", Translate_ChineseTw);


    auto func = [this](QString ts_path){
        getTransInfoHash(ts_path + R"(translation.xlsx)");
        updateTsFileByExecl(ts_path + R"(ja.xml)", Translate_Japanese);
        updateTsFileByExecl(ts_path + R"(zh_TW.xml)", Translate_ChineseTw);
        updateTsFileByExecl(ts_path + R"(ko.xml)", Translate_Korean);
        updateTsFileByExecl(ts_path + R"(zh_CN.xml)", Translate_Chinese);
    };

    QString ts_path;

    ts_path = src_path + "UI/window/login/ts/";
    func(ts_path);
    ts_path = src_path + "UI/window/main/ts/";
    func(ts_path);
    ts_path = src_path + "UI/window/config/ts/";
    func(ts_path);
    ts_path = src_path + "modules/platform/ts/";
    func(ts_path);
    ts_path = src_path + "modules/im/ts/";
    func(ts_path);

    ts_path = src_path + "modules/meet/ts/";
    func(ts_path);
*/
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

QCoro::Task<bool> TranslationManager::translateText(TranslateTitle from, TranslateTitle to)
{
    if(!languageTextHash()->contains(to) || !languageTextHash->contains(from))
    {
        co_return false;
    }

    if(to == Translate_Auto) // can not set target language to "auto"
    {
        co_return false;
    }

    if(from == to) // target language euqals to source language, no need to translate
    {
        co_return false;
    }

    QString languageFrom = languageTextHash->value(from);
    QString languageTo = languageTextHash->value(to);

    for(auto& iter : m_transInfoHash)
    {
        const QString& sourceText = iter.value(from);
        QString& destText = iter[to];

        if(sourceText.isEmpty() || !destText.isEmpty())
        {
            continue;
        }

        qDebug() << "test:" << sourceText << destText;

        QString salt = QUuid::createUuid().toString();//QString::number(QDateTime::currentSecsSinceEpoch());
        QString curtime = QString::number(QDateTime::currentSecsSinceEpoch());
        QString sign = generateSign(sourceText, salt, curtime);

        static QUrl url;
        url.clear();
        url.setUrl("https://openapi.youdao.com/api");

        static QUrlQuery query;
        query.clear();

        query.addQueryItem("q", sourceText);
        query.addQueryItem("from", languageFrom);
        query.addQueryItem("to", languageTo);
        query.addQueryItem("appKey", TRANSLATE_APP_KEY);
        query.addQueryItem("salt", salt);
        query.addQueryItem("sign", sign);
        query.addQueryItem("signType", "v3");
        query.addQueryItem("curtime", curtime);

        url.setQuery(query);

        static auto isUtf8 = [](const QString& data){
            // 尝试将数据解码为 UTF-8
            QByteArray encoded = data.toUtf8();
            // 再次编码为 UTF-8，看是否与原始数据一致
            return encoded == data;
        };

        DEBUGPREFIX << isUtf8(sourceText);

        if(!isUtf8(sourceText))
        {
            continue;
        }

        QString result = co_await getTranslationByUrl(url);

        if(!result.isEmpty())
        {
            destText = result;
        }

        qDebug() << __FUNCTION__ << __LINE__ << result;

        co_await QCoro::sleepFor(std::chrono::seconds(1));
    }

    co_return true;

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
            bool ret = co_await translateText(Translate_English, Translate_Japanese);
            //co_await translateText(Translate_Chinese, Translate_ChineseTw);

            if(ret)
            {
                writeTrans2Execl(docPath);
            }
        }
    };

    innerFunc(url);
}

void TranslationManager::translateExcel(const QString &path)
{
    static auto innerFunc = [this](QString path)->QCoro::Task<>{
        QString docPath = path;
        bool success = getTransInfoHash(docPath);
        if(success)
        {
            // use co_await to wait for the translation to finish one by one
            bool ret = co_await translateText(Translate_English, Translate_Korean);
            //co_await translateText(Translate_Chinese, Translate_ChineseTw);

            if(ret)
            {
                writeTrans2Execl(docPath);
            }
        }
    };

    innerFunc(path);
}

QStringList TranslationManager::extractSourceContent(const QString &fileName, const QString &filter)
{
    Q_UNUSED(filter)

    QStringList contents;

    QFile tsFile(fileName);
    if(!tsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DEBUGPREFIX << "Failed to open ts file:" << fileName;
        return QStringList();
    }

    QDomDocument doc;
    if(!doc.setContent(&tsFile))
    {
        DEBUGPREFIX << "Failed to parse .ts file:" << fileName;
        tsFile.close();
        return QStringList();
    }
    tsFile.close();

    QDomElement root = doc.documentElement();
    QDomNodeList messages = root.elementsByTagName("message");

    for(int i = 0; i < messages.count(); ++i)
    {
        QDomNode node = messages.at(i);
        QDomElement sourceElem = node.firstChildElement("source");
        QDomElement translationElem = node.firstChildElement("translation");

        QString type =  translationElem.attribute("type");

        //if(type == filter)
        {
            QString sourceText = sourceElem.text();
            QString translationText = translationElem.text();
            //if(translationText.isEmpty())
            {
                contents.append(sourceText);
            }
        }
    }

    DEBUGPREFIX << "find matched count:" << contents.size();

    return contents;
}

QStringList TranslationManager::extractTranslationContent(const QString &fileName, TranslateTitle language)
{
    QStringList contents;

    QFile tsFile(fileName);
    if(!tsFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DEBUGPREFIX << "Failed to open ts file:" << fileName;
        return QStringList();
    }

    QDomDocument doc;
    if(!doc.setContent(&tsFile))
    {
        DEBUGPREFIX << "Failed to parse .ts file:" << fileName;
        tsFile.close();
        return QStringList();
    }
    tsFile.close();

    QDomElement root = doc.documentElement();
    QDomNodeList messages = root.elementsByTagName("message");

    for(int i = 0; i < messages.count(); ++i)
    {
        QDomNode node = messages.at(i);
        QDomElement sourceElem = node.firstChildElement("source");
        QDomElement translationElem = node.firstChildElement("translation");

        QString sourceText = sourceElem.text();
        QString translationText = translationElem.text();

        TransInfoHash::iterator iter = m_transInfoHash.find(sourceText);
        if(iter != m_transInfoHash.end())
        {
            auto& languageHash = iter.value();
            if(languageHash[language].isEmpty())
            {
                languageHash[language] = translationText;
            }
        }
    }

    DEBUGPREFIX << "find matched count:" << contents.size();

    return contents;
}

QCoro::Task<QString> TranslationManager::getTranslationByUrl(QUrl url)
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

    if(columnCount < ColumnBegin || rowCount < RowBegin)
    {
        DEBUGPREFIX << rowCount << columnCount;
        return false;
    }

    for(int i = RowBegin; i < rowCount + 1; ++i)
    {
        QString key = currentSheet->read(i, TranslationManager::Translate_Code).toString();
        if(key.isEmpty())
        {
            continue;
        }

        QHash<int, QString> dest_hash;

        for(int title = Translate_Code + 1; title < Translate_Auto; ++title )
        {
            QString dest = currentSheet->read(i, title).toString();
            dest_hash.insert(title, dest);
        }

        if(m_transInfoHash.contains(key) && m_transInfoHash.value(key) == dest_hash)
        {
            //DEBUGPREFIX << key << '\n' << m_transInfoHash.value(key) << '\n' << dest_hash;
        }
        else
        {
            // auto iter = std::find_if(m_code_list.begin(), m_code_list.end(), [key](const QString& code){
            //     return key == code;
            // });
            //if(iter != m_code_list.end())
            {
                m_transInfoHash.insert(key, std::move(dest_hash));
            }
        }

        //DEBUGPREFIX << key << strZh << strEn;
    }
    return true;
}

void TranslationManager::writeCodes2Execl(const QStringList &codes, const QString &excelFilePath)
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

    currentSheet->write(Title, Translate_Code, "code:");

    for(int i = Translate_Chinese ; i <= Translate_Japanese ; ++i)
    {
        currentSheet->write(Title, i, languageTextHash->value(i));

    }

    for(int i = 0; i < codes.size(); ++i)
    {
        currentSheet->write(i + RowBegin, Translate_Code, codes.at(i));
    }

    bool succeed = xlsx.save();
    DEBUGPREFIX << "Save xlsx:" << succeed;
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

    if(columnCount < ColumnBegin || rowCount < RowBegin)
    {
        DEBUGPREFIX << rowCount << columnCount;
        return;
    }

    for(int i = RowBegin; i < rowCount + 1; ++i)
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

void insertLineInFile(const QString &tsFilePath, const QString &newLineContent)
{
    // 打开文件进行读取
    QFile file(tsFilePath);
    if (!file.open(QIODevice::ReadWrite | QIODevice::Text)) {
        qDebug() << "Failed to open .ts file:" << tsFilePath;
        return;
    }

    QTextStream inStream(&file);

    // 读取文件所有内容
    QStringList lines;
    while (!inStream.atEnd()) {
        lines.append(inStream.readLine());
    }

    if(lines.at(1) == "<!--")
    {
        file.close();
        return;
    }

    // 插入新行到第二行
    if (lines.size() >= 2) {
        lines.insert(1, newLineContent); // 插入到第二行
    } else {
        lines.append(newLineContent); // 如果文件少于两行，直接添加
    }

    // 清空文件内容
    file.resize(0);

    // 使用一个新的 QTextStream 来写入文件
    QTextStream outStream(&file);
    outStream.setCodec("UTF-8");
    outStream.setGenerateByteOrderMark(false);

    // 将修改后的内容写回文件
    for (const QString &line : lines) {
        outStream << line << "\n";
    }

    file.close();
}

void TranslationManager::updateTsFileByExecl(const QString& tsFilePath, TranslateTitle language)
{
    // 加载 .ts 文件
    QFile tsFile(tsFilePath);
    QFileInfo tsFileInfo(tsFile);
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

    root.setAttribute("language", tsFileInfo.baseName());

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
            QString newText = m_transInfoHash.value(sourceText).value(language);
            if(newText.isEmpty())
            {
                continue;
            }

            QString type =  translationElem.attribute("type");
            if(type == "unfinished")
            {
                translationElem.removeAttribute("type");
            }

            QDomElement numElem = translationElem.firstChildElement("numerusform");

            if(numElem.isNull()) // 这个子元素不存在
            {
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
            else
            {
                QDomNode numNode = numElem.firstChild();
                if(numNode.nodeValue().isEmpty())
                {
                    QDomText textNode = doc.createTextNode(newText);
                    numElem.appendChild(textNode);
                }
                else
                {
                    numNode.setNodeValue(newText);
                }
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
    outStream.setCodec("UTF-8"); // 设置编码
    outStream.setGenerateByteOrderMark(false); // 不生成 BOM
    doc.save(outStream, 4); // 使用缩进级别 4

    outFile.close();

    QString comment = R"(<!--
  ~ Copyright (c) 2022 船山信息 chuanshaninfo.com
  ~ The project is licensed under Mulan PubL v2.
  ~ You can use this software according to the terms and conditions of the Mulan
  ~ PubL v2. You may obtain a copy of Mulan PubL v2 at:
  ~          http://license.coscl.org.cn/MulanPubL-2.0
  ~ THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
  ~ EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
  ~ MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
  ~ See the Mulan PubL v2 for more details.
  -->
)";

    insertLineInFile(tsFilePath, comment);

    DEBUGPREFIX << "Translations updated successfully: " << tsFilePath;
}

