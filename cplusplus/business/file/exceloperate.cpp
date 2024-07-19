#include "exceloperate.h"

#include "business/system/translationmanager.h"
#include "midware/define/pathdefine.h"

using namespace QXlsx;

ExcelOperate::ExcelOperate(QObject *parent)
    : QObject(parent)
{

}

void ExcelOperate::convertExcelToTS(const QUrl& url)
{
    QDir dir(url.toLocalFile().append('/'));
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);

    QFileInfo xlsxFileInfo;

    for(int i = 0; i < fileInfoList.size(); ++i)
    {
        if(fileInfoList.at(i).fileName().endsWith(".xlsx"))
        {
            xlsxFileInfo = fileInfoList.at(i);
            fileInfoList.removeAt(i);
            break;
        }
    }

    QUrl docUrl(xlsxFileInfo.absoluteFilePath());

    //DEBUGPREFIX << xlsxFileInfo.absoluteFilePath() << docUrl;

    auto transInfoHash = getTransInfoHash(docUrl);

    bool ok = false;

    foreach(auto tsFileInfo, fileInfoList)
    {
        QFile file(tsFileInfo.absoluteFilePath());
        if(!file.open(QIODevice::ReadWrite))
        {
            DEBUGPREFIX << "error";
            return;
        }

        QString fileName = tsFileInfo.fileName().remove(".ts");
        if(fileName.contains('_'))
        {
            fileName = fileName.split('_').last();
        }
        QString strType = QStringLiteral("Translate_") + fileName.toUpper();

        QMetaEnum translationEnum = QMetaEnum::fromType<TranslationManager::TranslateTitle>();
        int type = translationEnum.keyToValue(strType.toStdString().c_str(), &ok);
        if(!ok)
        {
            continue;
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
                int left = line.indexOf('T');
                int right = line.indexOf('/');
                QString code = line.midRef(left + 1, right - left - 2).toString();

                auto val = transInfoHash.value(code);
                qDebug() << val;
                auto str = val[type].trimmed();
                // <translation type="unfinished"></translation>
                QString str2 = QStringLiteral("\n        <translation>") + str + QStringLiteral("</translation>\n");
                inStream << line << str2;
                out.readLine();
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

}

void ExcelOperate::transformCode(const QUrl &url)
{
    TransInfoHash transInfoHash = getTransInfoHash(url);

    auto codeList = transInfoHash.keys();

    std::sort(codeList.begin(), codeList.end());

    int size = codeList.size();

    QString result;

    QString temp("{ %1, tr(\"_T%1\") },\n");
    for(int i = 0; i < size; ++i)
    {
        result.append(temp.arg(codeList.at(i)));
    }

    QFile file(QStringLiteral("code.txt"));
    bool flag = file.open(QFile::WriteOnly | QFile::Truncate);

    if(!flag)
    {
        DEBUGPREFIX << "failed to open file:" << file.fileName();
        return;
    }

    qint64 outputSize = file.write(result.toUtf8());
    if(outputSize == -1)
    {
        DEBUGPREFIX << "failed to write file";
    }

}

ExcelOperate::TransInfoHash ExcelOperate::getTransInfoHash(const QUrl &url) const
{
    QString docPath = url.isLocalFile() ? url.toLocalFile() : url.toString();
    Document xlsxDoc(docPath);

    //DEBUGPREFIX << "xlsx doc:" << url << url.toString() << url.path() << url.toLocalFile() << docPath;

    if(!xlsxDoc.load())
    {
        DEBUGPREFIX << "ERROR: fail to load xlsx doc:" << url.toLocalFile();
        return TransInfoHash();
    }

    TransInfoHash transInfoHash;

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
            return TransInfoHash();
        }

        QVector<CellLocation> clList = wsheet->getFullCells( &maxRow, &maxCol );
        Q_UNUSED(clList)

        if(maxCol < TranslationManager::ColumnEnd || maxRow < 1)
        {
            DEBUGPREFIX << maxCol << maxRow;
            return TransInfoHash();
        }

        for(int i = TranslationManager::ColumnBegin; i < maxRow + 1; ++i)
        {
            QString code = xlsxDoc.read(i, TranslationManager::Code).toString();

            QString strZh = xlsxDoc.read(i, TranslationManager::Translate_ZH).toString();
            QString strEn = xlsxDoc.read(i, TranslationManager::Translate_EN).toString();

            transInfoHash.insert(code,
                                 {
                                     {TranslationManager::Translate_ZH, strZh},
                                     {TranslationManager::Translate_EN, strEn}
                                 });
            DEBUGPREFIX << code << strZh << strEn;
        }
        sheetIndexNumber++;
    }

    return transInfoHash;
}
