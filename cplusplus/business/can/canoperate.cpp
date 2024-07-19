#include "canoperate.h"

#include <midware/define/basedefine.h>

#include <QCanBus>
#include <QtSerialBus>
#include <QtSerialPort/QSerialPort>

void printCanPlugins()
{
    QStringList plugins = QCanBus::instance()->plugins();
    DEBUGPREFIX << "Available plugins:" << plugins;

    for(const QString& plugin : qAsConst(plugins))
    {
        QString errorString;
        const QList<QCanBusDeviceInfo> devices = QCanBus::instance()->availableDevices(
            plugin, &errorString);
        if (!errorString.isEmpty())
        {
            DEBUGPREFIX << plugin << errorString;
        }
        else
        {
            for(const QCanBusDeviceInfo& device : qAsConst(devices))
            {
                DEBUGPREFIX << "device:" << plugin << ":" << device.name() << device.channel() << device.description() << device.serialNumber();
            }
        }
    }
}

CANOperate::CANOperate(QObject *parent)
    : QObject{parent}
{
    //::printCanPlugins();
    //CANOperate::printCanPlugins();
}

QStringList CANOperate::getCANPlugins()
{
    return QCanBus::instance()->plugins();
}

bool CANOperate::updateCurCANPlugin(QString plugin)
{
    bool ret = false;

    m_plugin = plugin;
    m_canDevices.clear();
    m_deviceNames.clear();

    do {
        if(plugin.isEmpty())
        {
            break;
        }

        QString errorString;
        QList<QCanBusDeviceInfo> devices = QCanBus::instance()->availableDevices(plugin, &errorString);
        if (!errorString.isEmpty())
        {
            DEBUGPREFIX_ERROR << plugin << errorString;
        }
        else
        {
            m_canDevices = devices;

            for(auto& canDevice : qAsConst(m_canDevices))
            {
                m_deviceNames.append(canDevice.name());
            }

            ret = true;
        }
    } while (0);

    emit deviceNamesChanged(m_deviceNames);

    return ret;
}

QList<QString> CANOperate::getCANInterfaceProperty(int index)
{
    QList<QString> ret;
    if(index < 0 || index >= m_canDevices.size())
    {
        return ret;
    }

    const QCanBusDeviceInfo& deviceInfo = m_canDevices.at(index);
    ret << deviceInfo.name()
        << QString::number(deviceInfo.channel())
        << deviceInfo.description()
        << deviceInfo.serialNumber()
        << QString::number(deviceInfo.hasFlexibleDataRate())
        << QString::number(deviceInfo.isVirtual());

    DEBUGPREFIX << "device:" << m_plugin << ":" << ret;
    return ret;
}

void CANOperate::dealAscFile(const QUrl &url, QVector<int> frameIDs)
{
    QThreadPool* pool = QThreadPool::globalInstance();

    DEBUGPREFIX <<"QThreadPool:" << pool->maxThreadCount() << pool->expiryTimeout() << pool->stackSize() << QThread::currentThreadId();

    auto func = [=, this](){
        QString path = url.isLocalFile() ? url.toLocalFile() : url.toString();
        QFile file(path);

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            DEBUGPREFIX_ERROR << "fail to open file:" << path;
            return;
        }

        QByteArray output;

        int dataLineStart = 4;

        while(!file.atEnd())
        {
            QByteArray line = file.readLine();
            if(!dataLineStart)
            {
                QString strLine = line;
                auto strList = strLine.trimmed().split(QRegExp("\\s+"));
                bool ok = false;
                uint frameID = strList.at(AscTitle_FrameID).toUInt(&ok, 16);
                //DEBUGPREFIX << frameID << frameIDs << strList;
                if(ok && frameIDs.contains(frameID))
                {
                    output.append(line);
                }
            }
            else
            {
                output.append(line);
                dataLineStart--;
            }

        }

        file.close();
        int insertPos = path.lastIndexOf('.');

        file.setFileName(path.insert(insertPos, "_new"));
        if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            DEBUGPREFIX_ERROR << "fail to write file:" << path;
            return;
        }

        auto bytes = file.write(output);

        DEBUGPREFIX << "write bytes:" << bytes << path;

        file.close();

        DEBUGPREFIX << "currentThreadId:" << QThread::currentThreadId();
    };

    pool->start(func);
}

void CANOperate::printCanPlugins()
{
    QStringList plugins = getCANPlugins();
    for(const QString& plugin : qAsConst(plugins))
    {
        if(updateCurCANPlugin(plugin))
        {
            for(int j = 0; j < m_canDevices.size(); ++j)
            {
                getCANInterfaceProperty(j);
            }
        }
    }
}

