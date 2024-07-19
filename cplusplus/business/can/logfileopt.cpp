#include "logfileopt.h"

#include <tchar.h>
#include "include/binlog.h"

#include <QMetaEnum>

LogFileOpt::LogFileOpt(QObject *parent)
{
    // LPCTSTR pFileName = _T(R"(D:\ZWJ\Soleil\resource\data.blf)");
    // DWORD dwWritten;
    // DWORD dwRead;
    // DWORD dwSkip;

    //DEBUGPREFIX << read_test(pFileName, &dwRead) << dwRead;
}


template<>
QString LogFileOpt::parseDtc<LogFileOpt::DtcType_Key>(const QString &data)
{
    static QMap<quint32, QString> dtcCommentMap = {

    };

    return innerParseDtc(dtcCommentMap, data);
}

template<>
QString LogFileOpt::parseDtc<LogFileOpt::DtcType_StraightDriveLumbar>(const QString &data)
{
    static QMap<quint32, QString> dtcCommentMap = {

    };

    return innerParseDtc(dtcCommentMap, data);
}

template<>
QString LogFileOpt::parseDtc<LogFileOpt::DtcType_LinLumbar>(const QString &data)
{
    static QMap<quint32, QString> dtcCommentMap = {

    };

    return innerParseDtc(dtcCommentMap, data);
}

QString LogFileOpt::parseDtc(enumDtcType dtcType, const QString &data)
{
    QString result;
    switch(dtcType)
    {
    case DtcType_Key:
        result = parseDtc<DtcType_Key>(data);
        break;
    case DtcType_StraightDriveLumbar:
        result = parseDtc<DtcType_StraightDriveLumbar>(data);
        break;
    case DtcType_LinLumbar:
        result = parseDtc<DtcType_LinLumbar>(data);
        break;
    default:
        break;
    }

    return result;
}


QString LogFileOpt::parseKeyAD(const QString& data)
{
    static const QStringList KeyNames = {

    };

    QStringList adDisplayList;
    qDebug()  << data;
    QStringList dataList = data.trimmed().split(QRegExp("\\s+"));
    int listSize = dataList.size();

    if(listSize < 5)
    {
        return QString();
    }

    QString adFormat("Ad value: 0x%2 ( %3 ) %1");

    int keyNameIdx = 0;
    for(int i = 3; i < listSize; i = i + 2)
    {
        bool ok = false;
        uint ad_1 = dataList.at(i).toUInt(&ok, 16);
        if(!ok)
        {
            qDebug()  << "fault data:" << dataList.at(i) << "full data:" << data;
        }

        uint ad_2 = dataList.at(i + 1).toUInt(&ok, 16);
        if(!ok)
        {
            qDebug()  << "fault data:" << dataList.at(i + 1) << "full data:" << data;
        }
        uint adValue = (ad_1 << 8) + ad_2;

        QString adDisplay = adFormat.arg(KeyNames.at(keyNameIdx),
                                         dataList.at(i) + dataList.at(i + 1),
                                         QString::number(adValue, 10));

        adDisplayList.append(adDisplay);
        keyNameIdx++;
    }

    QString output;

    for(int i = 0; i < adDisplayList.size(); ++i)
    {
        output.append(adDisplayList.at(i) + "\n");
        qDebug()  << adDisplayList.at(i);
    }

    return output;
}

int LogFileOpt::parseBlfFile(const QUrl &url)
{
    QString path = url.isLocalFile() ? url.toLocalFile() : url.toString();

    HANDLE hFile;
    VBLObjectHeaderBase base;
    VBLCANMessage message;
    VBLCANMessage2 message2;
    VBLEnvironmentVariable variable;
    VBLEthernetFrame ethframe;
    VBLAppText appText;
    VBLFileStatisticsEx statistics = { sizeof( statistics)};
    BOOL bSuccess;

    QString output;

    /* open file */
    QByteArray pathByteArray = path.toUtf8();
    hFile = BLCreateFile( pathByteArray.constData(), GENERIC_READ);

    if ( INVALID_HANDLE_VALUE == hFile)
    {
        return -1;
    }

    BLGetFileStatisticsEx( hFile, &statistics);

    bSuccess = TRUE;

    /* read base object header from file */
    while ( bSuccess && BLPeekObject( hFile, &base))
    {
        switch ( base.mObjectType)
        {
        case BL_OBJ_TYPE_CAN_MESSAGE:
            /* read CAN message */
            message.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &message.mHeader.mBase, sizeof(message));
            /* free memory for the CAN message */
            if( bSuccess) {
                BLFreeObject( hFile, &message.mHeader.mBase);
            }
            break;
        case BL_OBJ_TYPE_CAN_MESSAGE2:
            /* read CAN message */
            message2.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &message2.mHeader.mBase, sizeof(message2));
            /* free memory for the CAN message */
            if( bSuccess) {
                BLFreeObject( hFile, &message2.mHeader.mBase);
            }
            break;
        case BL_OBJ_TYPE_ENV_INTEGER:
        case BL_OBJ_TYPE_ENV_DOUBLE:
        case BL_OBJ_TYPE_ENV_STRING:
        case BL_OBJ_TYPE_ENV_DATA:
            /* read environment variable */
            variable.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &variable.mHeader.mBase, sizeof(variable));
            /* free memory for the environment variable */
            if( bSuccess) {
                BLFreeObject( hFile, &variable.mHeader.mBase);
            }
            break;
        case BL_OBJ_TYPE_ETHERNET_FRAME:
            /* read ethernet frame */
            ethframe.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &ethframe.mHeader.mBase, sizeof(ethframe));
            /* free memory for the frame */
            if( bSuccess) {
                BLFreeObject( hFile, &ethframe.mHeader.mBase);
            }
            break;
        case BL_OBJ_TYPE_APP_TEXT:
            /* read text */
            appText.mHeader.mBase = base;
            bSuccess = BLReadObjectSecure( hFile, &appText.mHeader.mBase, sizeof(appText));
            if ( NULL != appText.mText)
            {
                printf( "%s\n", appText.mText);
            }
            /* free memory for the text */
            if( bSuccess) {
                BLFreeObject( hFile, &appText.mHeader.mBase);
            }
            break;
        default:
            /* skip all other objects */
            bSuccess = BLSkipObject( hFile, &base);
            break;
        }

        if ( bSuccess)
        {

        }
    }

    /* close file */
    if ( !BLCloseHandle( hFile))
    {
        return -1;
    }

    return bSuccess ? 0 : -1;
}


QString LogFileOpt::innerParseDtc(const DtcCommentMap& dtcCommentMap, const QString& data)
{
    qDebug()  << data;

    QStringList dtcDisplayList;
    QStringList dtcDisplayCommentList;

    QStringList dataList = data.trimmed().split(QRegExp("\\s+"));
    int listSize = dataList.size();

    if(listSize < 6)
    {
        return QString();
    }

    QString dtcFormat("DTC: 0x%1%2%3 Value: 0x%4  binary: %5");

    for(int i = 3; i < listSize; i = i + 4)
    {
        bool ok = false;
        uint dtcValue = dataList.at(i + 3).toUInt(&ok, 16);


        if(ok)
        {
            QString dtcDisplayText = dtcFormat.arg(dataList.at(i), dataList.at(i + 1), dataList.at(i + 2), dataList.at(i + 3))
                                         .arg(QString::number(dtcValue, 2), 8, QLatin1Char('0'));

            uint dtc1 = dataList.at(i).toUInt(nullptr, 16);
            uint dtc2 = dataList.at(i + 1).toUInt(nullptr, 16);
            uint dtc3 = dataList.at(i + 2).toUInt(nullptr, 16);
            quint32 dtc = (dtc1 << 16) + (dtc2 << 8) + dtc3;
            QString dtcComment = dtcCommentMap.value(dtc);

            if(!dtcComment.isEmpty())
            {
                dtcDisplayList.append(dtcDisplayText);
                dtcDisplayCommentList.append(dtcComment);
            }

        }
    }

    QString output;

    for(int i = 0; i < dtcDisplayCommentList.size(); ++i)
    {
        output.append(dtcDisplayList.at(i) + " " + dtcDisplayCommentList.at(i) + "\n");
        qDebug()  << dtcDisplayList.at(i) << dtcDisplayCommentList.at(i);
    }

    return output;
}
