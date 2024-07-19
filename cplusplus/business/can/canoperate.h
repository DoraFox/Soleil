#ifndef CANOPERATE_H
#define CANOPERATE_H

#include <QObject>
#include <QCanBusDeviceInfo>
#include <midware/define/basedefine.h>

class CANOperate : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QStringList deviceNames READ deviceNames NOTIFY deviceNamesChanged)

    typedef enum enumAscTitle{
        AscTitle_Time,
        AscTitle_Chanenl,
        AscTitle_FrameID,
        AscTitle_Direction,
    }AscTitle;


public:
    explicit CANOperate(QObject *parent = nullptr);

    Q_INVOKABLE QStringList getCANPlugins();

    Q_INVOKABLE bool updateCurCANPlugin(QString plugin);

    Q_INVOKABLE QList<QString> getCANInterfaceProperty(int index);

    Q_INVOKABLE void dealAscFile(const QUrl &url, QVector<int> frameIDs);

signals:
    DECLARE_PROPERTY_SIGNAL(QStringList, deviceNames)

private:
    void printCanPlugins();

private:
    QString m_plugin;
    QList<QCanBusDeviceInfo> m_canDevices;



    DECLARE_QML_PROPERTY_READ_ONLY(QStringList, deviceNames)

};

#endif // CANOPERATE_H
