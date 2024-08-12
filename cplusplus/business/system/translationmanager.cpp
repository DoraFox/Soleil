#include "translationmanager.h"

#include <QUrlQuery>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QCryptographicHash>
#include <QRegularExpression>

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

void TranslationManager::translateProject(const QUrl &url)
{
    if(translateProFile("lupdate.exe"))
    {
        ClassRegister* reg = ClassRegister::GetInstance();
        //reg->excelOpt()->convertExcelToTS(url);
        //translateProFile("lrelease");
    }
}

bool TranslationManager::translateProFile(QString /*command*/)
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

QCoro::Task<> TranslationManager::translateText(TransInfoHash& hash, TranslateTitle from, TranslateTitle to)
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

    QTimer timer;
    timer.setInterval(1000); // interfaces cannot be accessed frequently
    timer.start();

    TransInfoHash::iterator iter = hash.begin();
    while (iter != m_transInfoHash.end())
    {
        const QString& text = iter.key();
        if(!text.isEmpty())
        {
            QString salt = QString::number(QDateTime::currentMSecsSinceEpoch());
            QString curtime = QString::number(QDateTime::currentSecsSinceEpoch());
            QString sign = generateSign(text, salt, curtime);

            QUrl url("https://openapi.youdao.com/api");
            QUrlQuery query;
            query.addQueryItem("q", text);
            query.addQueryItem("from", languageFrom);
            query.addQueryItem("to", languageTo);
            query.addQueryItem("appKey", TRANSLATE_APP_KEY);
            query.addQueryItem("salt", salt);
            query.addQueryItem("sign", sign);
            query.addQueryItem("signType", "v3");
            query.addQueryItem("curtime", curtime);

            url.setQuery(query);

            DEBUGPREFIX << url;

            static QNetworkAccessManager mgr;
            static QNetworkRequest request;

            request.setUrl(url);
            auto* reply = co_await mgr.get(request);

            if (reply->error() == QNetworkReply::NoError) {
                QByteArray response = reply->readAll();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(response);
                QJsonObject jsonObj = jsonDoc.object();

                if (jsonObj.contains("translation")) {
                    QJsonArray translationArray = jsonObj["translation"].toArray();
                    DEBUGPREFIX << translationArray.toVariantList().toVector();

                    QHash<int, QString>& value = iter.value();
                    if(!translationArray.isEmpty())
                    {
                        value[to] = translationArray.first().toString();
                    }

                    DEBUGPREFIX << text << value[to];

                } else {
                    DEBUGPREFIX << "Error: Failed to get translation." << iter.key();
                    DEBUGPREFIX << jsonObj;
                }
            } else {
                DEBUGPREFIX << "Error: " << reply->errorString();
            }

            reply->deleteLater();
        }
        ++iter;

        co_await timer;
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
    translateExcel_inter(url);
}

QStringList TranslationManager::extractSourceContentFromFile(const QString &fileName) {
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

bool TranslationManager::getTransInfoHash(const QString &docPath)
{
    m_transInfoHash.clear();

    //QString docPath = url.isLocalFile() ? url.toLocalFile() : url.toString();
    Document xlsxDoc(docPath);

    //DEBUGPREFIX << "xlsx doc:" << url << url.toString() << url.path() << url.toLocalFile() << docPath;

    if(!xlsxDoc.load())
    {
        DEBUGPREFIX << "ERROR: fail to load xlsx doc:";
        return false;
    }

    int sheetIndexNumber = 0;

    foreach(const QString& currentSheetName, xlsxDoc.sheetNames() )
    {
        AbstractSheet* currentSheet = xlsxDoc.sheet( currentSheetName );
        DEBUGPREFIX << "currentSheetName" << currentSheetName;
        if ( nullptr == currentSheet )
        {
            DEBUGPREFIX << "currentSheet is NULL";
            continue;
        }

        // get full cells of current sheet
        int maxRow = -1;
        int maxCol = -1;
        currentSheet->workbook()->setActiveSheet( sheetIndexNumber );
        Worksheet* wsheet = reinterpret_cast<Worksheet*>(currentSheet->workbook()->activeSheet());
        if ( nullptr == wsheet )
        {
            DEBUGPREFIX << "wsheet is NULL";
            return false;
        }

        QVector<CellLocation> clList = wsheet->getFullCells( &maxRow, &maxCol );
        Q_UNUSED(clList)

        DEBUGPREFIX << maxCol << maxRow;

        if(maxCol < TranslationManager::ColumnBegin || maxRow < 1)
        {
            //DEBUGPREFIX << maxCol << maxRow;
            return false;
        }

        for(int i = TranslationManager::ColumnBegin; i < maxRow + 1; ++i)
        {
            QString code = xlsxDoc.read(i, TranslationManager::Code).toString();

            QString strZh = xlsxDoc.read(i, TranslationManager::Translate_Chinese).toString();
            QString strZhTw = xlsxDoc.read(i, TranslationManager::Translate_ChineseTw).toString();
            QString strEn = xlsxDoc.read(i, TranslationManager::Translate_English).toString();

            m_transInfoHash.insert(code,
                                   {
                                       {TranslationManager::Translate_Chinese, strZh},
                                       {TranslationManager::Translate_ChineseTw, strZhTw},
                                       {TranslationManager::Translate_English, strEn}
                                   });
            DEBUGPREFIX << code << strZh << strEn;
        }
        sheetIndexNumber++;
    }

    return true;
}

void TranslationManager::writeTrans2Execl(const QUrl &url, TransInfoHash &hash)
{
    QString docPath = url.isLocalFile() ? url.toLocalFile() : url.toString();
    Document xlsxDoc(docPath);

    //DEBUGPREFIX << "xlsx doc:" << url << url.toString() << url.path() << url.toLocalFile() << docPath;

    if(!xlsxDoc.load())
    {
        DEBUGPREFIX << "ERROR: fail to load xlsx doc:" << url.toLocalFile();
        return;
    }

    int sheetIndexNumber = 0;

    foreach(const QString& currentSheetName, xlsxDoc.sheetNames() )
    {
        AbstractSheet* currentSheet = xlsxDoc.sheet( currentSheetName );
        DEBUGPREFIX << "currentSheetName" << currentSheetName;
        if ( nullptr == currentSheet )
        {
            DEBUGPREFIX << "currentSheet is NULL";
            continue;
        }

        // get full cells of current sheet
        int maxRow = -1;
        int maxCol = -1;
        currentSheet->workbook()->setActiveSheet( sheetIndexNumber );
        Worksheet* wsheet = reinterpret_cast<Worksheet*>(currentSheet->workbook()->activeSheet());
        if ( nullptr == wsheet )
        {
            DEBUGPREFIX << "wsheet is NULL";
            return;
        }

        QVector<CellLocation> clList = wsheet->getFullCells( &maxRow, &maxCol );
        Q_UNUSED(clList)

        DEBUGPREFIX << maxCol << maxRow;

        if(maxCol < TranslationManager::ColumnBegin || maxRow < 1)
        {
            //DEBUGPREFIX << maxCol << maxRow;
            return;
        }

        for(int i = TranslationManager::ColumnBegin; i < maxRow + 1; ++i)
        {
            QString code = xlsxDoc.read(i, TranslationManager::Code).toString();

            if(hash.contains(code))
            {
                const QHash<int, QString>& value = hash[code];
                QHash<int, QString>::ConstIterator iter = value.cbegin();

                while (iter != value.cend())
                {
                    int language = iter.key();
                    QString targetText = iter.value();

                    xlsxDoc.write(i, language, targetText);

                    ++iter;
                }
            }
        }
        sheetIndexNumber++;
    }

    xlsxDoc.save();
}

QCoro::Task<> TranslationManager::translateExcel_inter(QUrl url)
{
    bool success = getTransInfoHash(url.toLocalFile());
    if(success)
    {
        // use co_await to wait for the translation to finish
        co_await translateText(m_transInfoHash, Translate_English, Translate_Chinese);
        co_await translateText(m_transInfoHash, Translate_English, Translate_ChineseTw);

        writeTrans2Execl(url, m_transInfoHash);
    }
}

void TranslationManager::convertExcelToTS(const QString &execlFileName, const QString& tsFileName, TranslateTitle type)
{
    QFile file(tsFileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open file:" << tsFileName;
        return;
    }

    if(!getTransInfoHash(execlFileName))
    {
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");

    QString ret;
    QTextStream inStream(&ret);
    inStream.setCodec("UTF-8");

    QRegExp reg("<source>.*</source>");

    while(!out.atEnd())
    {
        QString line = out.readLine();
        if(line.contains(reg))
        {
            int left = line.indexOf('>');
            int right = line.indexOf('/');
            QString code = line.midRef(left + 1, right - left - 2).toString();

            auto val = m_transInfoHash.value(code);
            qDebug() << val;
            auto str = val[type];

            QString nextLine = out.readLine();
            if(nextLine.contains(R"(<translation type="unfinished"></translation>)"))
            {
                // <translation type="unfinished"></translation>
                QString str2 = QStringLiteral("\n        <translation type=\"unfinished\">") + str + QStringLiteral("</translation>\n");
                inStream << line << str2;
            }
            else
            {
                inStream << line << "\n" << nextLine << "\n";
            }

        }
        else
        {
            inStream << line << "\n";
        }
    }

    file.close();

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        DEBUGPREFIX << "error";
        return;
    }

    DEBUGPREFIX << "string wrote to file size:" << ret.size();
    file.write(ret.toUtf8());
    file.close();
}

void TranslationManager::writeToTranslationExcel(const QStringList &contents, const QString &fileName) {
    Document xlsx;

    for (int i = 0; i < contents.size(); ++i) {
        xlsx.write(i + 1, 1, contents[i]); // 写入每一行内容到 Excel 文件
    }

    if (xlsx.saveAs(fileName)) {
        qDebug() << "Successfully wrote to Excel file:" << fileName;
    } else {
        qWarning() << "Failed to write to Excel file:" << fileName;
    }
}


