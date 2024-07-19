#ifndef LOGFILEOPT_H
#define LOGFILEOPT_H

#include <QObject>

#include <midware/define/basedefine.h>

#include <QUrl>

class LogFileOpt : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString keyDtcData READ keyDtcData WRITE set_keyDtcData NOTIFY keyDtcDataChanged)
    Q_PROPERTY(QString keyAdData READ keyAdData WRITE set_keyAdData NOTIFY keyAdDataChanged)

    Q_PROPERTY(QStringList dtcDisplayList READ dtcDisplayList NOTIFY dtcDisplayListChanged)
    Q_PROPERTY(QStringList dtcDisplayCommentList READ dtcDisplayCommentList NOTIFY dtcDisplayCommentListChanged)
    Q_PROPERTY(QStringList adDisplayList READ adDisplayList NOTIFY adDisplayListChanged)

    using DtcCommentMap = QMap<quint32, QString>;
public:
    typedef enum enumDtcType{
        DtcType_Key,
        DtcType_StraightDriveLumbar,
        DtcType_LinLumbar,

    }DtcType;
    Q_ENUM(enumDtcType)

public:

    explicit LogFileOpt(QObject *parent = nullptr);

    template<DtcType dtcType> QString parseDtc(const QString& data);

    Q_INVOKABLE QString parseDtc(enumDtcType dtcType, const QString& data);

    Q_INVOKABLE QString parseKeyAD(const QString& data);

    int parseBlfFile(const QUrl &url);


private:

    QString innerParseDtc(const DtcCommentMap& dtcCommentMap, const QString& data);

signals:

    DECLARE_PROPERTY_SIGNAL(QString, keyDtcData)
    DECLARE_PROPERTY_SIGNAL(QString, keyAdData)

    DECLARE_PROPERTY_SIGNAL(QStringList, dtcDisplayList)
    DECLARE_PROPERTY_SIGNAL(QStringList, dtcDisplayCommentList)
    DECLARE_PROPERTY_SIGNAL(QStringList, adDisplayList)

private:


    DECLARE_QML_PROPERTY(QString, keyDtcData)
    DECLARE_QML_PROPERTY(QString, keyAdData)

    DECLARE_QML_PROPERTY_READ_ONLY(QStringList, dtcDisplayList)
    DECLARE_QML_PROPERTY_READ_ONLY(QStringList, dtcDisplayCommentList)

    DECLARE_QML_PROPERTY_READ_ONLY(QStringList, adDisplayList)

};

#endif // LOGFILEOPT_H
