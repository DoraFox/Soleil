#include "linmanager.h"

#include "PLinApi.h"

LinManager::LinManager(QObject *parent)
    : QObject{parent}
{

}


QStringList LinManager::initLinHardware()
{
    int availableHwCount = 0;
    TLINError linError = LIN_GetAvailableHardware(NULL, 0, &availableHwCount);

    // use default value if either no hw is connected or an unexpected error occured
    if(availableHwCount == 0)
    {
        availableHwCount = 16;
    }

    HLINHW *hwHandles = new HLINHW[availableHwCount];
    WORD buffSize = availableHwCount * sizeof(HLINHW);

    linError = LIN_GetAvailableHardware(hwHandles, buffSize, &availableHwCount);

    QStringList textList;

    if (linError == errOK)
    {
        if (availableHwCount == 0)
        {
            // No hardware found
        }
        else
        {
            HLINHW hw = INVALID_LIN_HANDLE;
            int type = 0;
            int devNo = 0;
            int channel = 0;
            TLINHardwareMode mode = modNone;

            static const QString format("%1 - device: %2, channel: %3 Mode: %4");

            textList.reserve(availableHwCount);

            for (int i = 0; i < availableHwCount; i++)
            {
                hw = hwHandles[i];

                LIN_GetHardwareParam(hw, hwpType, &type, 0);
                LIN_GetHardwareParam(hw, hwpDeviceNumber, &devNo, 0);
                LIN_GetHardwareParam(hw, hwpChannelNumber, &channel, 0);
                LIN_GetHardwareParam(hw, hwpMode, &mode, 0); // mode: Master, Slave or None

                QString typeName;
                switch(type)
                {
                case LIN_HW_TYPE_USB_PRO:
                    typeName = "PCAN-USB Pro";
                    break;
                case LIN_HW_TYPE_USB_PRO_FD:
                    typeName = "PCAN-USB Pro FD";
                    break;
                case LIN_HW_TYPE_PLIN_USB:
                    typeName = "PLIN-USB";
                    break;
                default:
                    typeName = "Unknown";
                    break;
                }

                QString modeName;
                switch(mode)
                {
                case modNone:
                    modeName = "NONE";
                    break;
                case modMaster:
                    modeName = "Master";
                    break;
                case modSlave:
                    modeName = "Slave";
                    break;
                default:
                    break;
                }

                textList.append(format.arg(typeName).arg(devNo).arg(channel).arg(modeName));
            }
        }
    }

    return textList;
}

