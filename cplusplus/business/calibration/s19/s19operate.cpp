#include "s19operate.h"

#include <QFile>
#include <QByteArray>
#include <QTextStream>
#include <QRegExp>
#include <QUrl>
#include <QThread>

S19Operate::S19Operate()
{

}

void S19Operate::setReplaceInfo(const QUrl &url, QVector<int> address, QVector<int> value)
{
    m_url = url;
    m_address = address;
    m_value = value;
}

void S19Operate::startReplace()
{
    QString path = m_url.isLocalFile() ? m_url.toLocalFile() : m_url.toString();

    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        DEBUGPREFIX_ERROR << "fail to open file:" << path << m_url;
        return;
    }

    QByteArray output;

    int addressSize = m_address.size();
    int valueSize = m_value.size();

    if ((addressSize != valueSize) || (addressSize == 0))
    {
        return;
    }

    while(!file.atEnd())
    {
        QByteArray line = file.readLine();
        if(!m_address.isEmpty())
        {
            for(int i = m_address.size() - 1; i >= 0 ; --i)
            {
                bool ret = replaceValueByAddress(line, static_cast<uint32_t>(m_address.at(i)), static_cast<uint8_t>(m_value.at(i)));
                if(ret)
                {
                    m_address.removeAt(i);
                    m_value.removeAt(i);
                }
            }
        }
        output.append(line);
    }

    file.close();

    if(!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        DEBUGPREFIX_ERROR << "fail to write file:" << path;
        return;
    }

    auto bytes = file.write(output);

    DEBUGPREFIX << "write bytes:" << bytes << path;

    file.close();
}

bool S19Operate::replaceValueByAddress(QByteArray &data, uint32_t addressInput,uint8_t value)
{
    // the last value in "data" is '\n'
    int dataSize = data.size() - 1;

    static const int typeLength = 2;
    const static int countLength = 2;
    static const int checksumLength = 2;

    if(dataSize < typeLength + countLength)
    {
        return false;
    }

    int addressLength = 0;

    if(data.at(0) == 'S')
    {
        char type = data.at(1);
        if (type == '1' || type == '9') {
            addressLength = 4;
        } else if (type == '2' || type == '8') {
            addressLength = 6;
        } else if (type == '3' || type == '7') {
            addressLength = 8;
        } else {
            return false;
        }
    }
    else
    {
        return false;
    }

    bool ok = false;
    int count = data.mid(0 + typeLength, countLength).toInt(&ok, 16);

    int dataCount = (count - (addressLength / 2) - 1) * 2;

    if (dataSize != (typeLength + countLength + addressLength + dataCount + checksumLength)) {
        DEBUGPREFIX << dataSize << (typeLength + countLength + addressLength + dataCount + checksumLength)
                    << addressLength << dataCount << data;
        return false;
    }

    if (!ok) {
        return false;
    }

    QByteArray addressArray = QByteArray::fromHex(data.mid(0 + typeLength + countLength, addressLength));

    uint32_t address = ((addressArray[0] << 24) & 0xFF000000) |
                   ((addressArray[1] << 16) & 0x00FF0000) |
                   ((addressArray[2] << 8) & 0x0000FF00) |
                   (addressArray[3] & 0x000000FF);

    int addressStart = 4 + addressLength;

    static const char NUMBER_ARRAY[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                                        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

    bool dataChanged = false;

    if (addressInput >= address && addressInput < address + dataCount / 2) {
        int idx = (addressInput - address) * 2;

        if(data[addressStart + idx] != NUMBER_ARRAY[(value >> 4) & 0x0F])
        {
            data[addressStart + idx] = NUMBER_ARRAY[(value >> 4) & 0x0F];
            dataChanged = true;
        }

        if(data[addressStart + idx + 1] != NUMBER_ARRAY[value & 0x0F])
        {
            data[addressStart + idx + 1] = NUMBER_ARRAY[value & 0x0F];
            dataChanged = true;
        }
    }
    else
    {
        return false;
    }

    if(dataChanged)
    {
        int checksumDatasLength = countLength + addressLength + dataCount;

        QVector<uint32_t> checksumDatas;
        checksumDatas.resize(checksumDatasLength / 2);

        uint8_t checksum = 0;

        bool checkSumError = false;

        for (int i = 0; i < checksumDatasLength; i = i + 2) {
            bool ok = false;
            checksumDatas[i / 2] = data.mid(2 + i, 2).toUInt(&ok, 16);
            if (!ok) {
                DEBUGPREFIX << "file data ERROR! toUInt fail:" << "data[" << 2 + i << "]" << data.mid(2 + i, 2) << "data:" << data;
                checkSumError = true;
                break;
            } else {
                checksum = (checksum + checksumDatas[i / 2]) & 0xFF;
            }
        }

        if(!checkSumError)
        {
            checksum = 0xFF - checksum;

            data[dataSize - 2] = NUMBER_ARRAY[(checksum >> 4) & 0x0F];
            data[dataSize - 1] = NUMBER_ARRAY[checksum & 0x0F];
        }

    }

    return true;
}

void S19Operate::run() {
    DEBUGPREFIX << "QThread::currentThread()" << QThread::currentThread() << QThread::currentThread();

    startReplace();
}
