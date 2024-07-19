#include "ccpcalibration.h"

#include "midware/define/basedefine.h"
#include "midware/define/mathdefine.h"


void ccp_test()
{

}


union LIN_PID
{
    unsigned char PID;
    struct
    {
        unsigned char  bit0 : 1;
        unsigned char  bit1 : 1;
        unsigned char  bit2 : 1;
        unsigned char  bit3 : 1;
        unsigned char  bit4 : 1;
        unsigned char  bit5 : 1;
        unsigned char  bit6 : 1;
        unsigned char  bit7 : 1;
    }bit;

    unsigned char Get_Bit(unsigned char idx)
    {
        switch (idx) {
        case 0x00:
            return bit.bit0;
        case 0x01:
            return bit.bit1;
        case 0x02:
            return bit.bit2;
        case 0x03:
            return bit.bit3;
        case 0x04:
            return bit.bit4;
        case 0x05:
            return bit.bit5;
        case 0x06:
            return bit.bit6;
        case 0x07:
            return bit.bit7;
        default:
            return bit.bit0;
        }
    }

    void Set_Bit(unsigned char idx, bool value)
    {
        switch (idx) {
        case 0x00:
            bit.bit0 = value;
        case 0x01:
            bit.bit1 = value;
        case 0x02:
            bit.bit2 = value;
        case 0x03:
            bit.bit3 = value;
        case 0x04:
            bit.bit4 = value;
        case 0x05:
            bit.bit5 = value;
        case 0x06:
            bit.bit6 = value;
        case 0x07:
            bit.bit7 = value;
        default:
            bit.bit0 = value;
        }
    }
};

unsigned char GetLinPID(unsigned char PID)
{
    union LIN_PID Pid_data;
    Pid_data.PID = PID;
    Pid_data.bit.bit6 = (Pid_data.bit.bit0 ^ Pid_data.bit.bit1 ^ Pid_data.bit.bit2 ^ Pid_data.bit.bit4);
    Pid_data.bit.bit7 = ~(Pid_data.bit.bit1 ^ Pid_data.bit.bit3 ^ Pid_data.bit.bit4 ^ Pid_data.bit.bit5);
    return Pid_data.PID;
}

CCPCalibration::CCPCalibration(QObject *parent)
    : QObject{parent}
{
    auto func = [&](){

        quint8 d = 0xAA;
        for(int i = 0; i < 8; ++i)
        {
            DEBUGPREFIX << BitArrayGetBit(d, i);
        }

        ccp_test();
    };
}
