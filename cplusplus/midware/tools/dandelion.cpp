#include "dandelion.h"

#include <random>
#include <QDir>
#include <QDebug>

Dandelion::Dandelion(QObject *parent)
    : QObject{parent}
{

}

inline bool Dandelion::isLightColor(int (&rgb)[3])
{
    return (rgb[0] * 0.299 + rgb[1] * 0.587 + rgb[2] * 0.114) >= 100;
}

inline bool Dandelion::isDeepColor(int (&rgb)[3])
{
    return !isLightColor(rgb);
}

void Dandelion::makeDirectory(QString dirPath)
{
    QDir dir;
    if(!dir.exists(dirPath))
    {
        dir.mkdir(dirPath);
    }
}

QString Dandelion::getDirectoryAbsolutePath(QString path)
{
    if(!path.isEmpty())
    {
        if (path.startsWith("file:///")) {
            path.remove(0, 8);
        }
        if ("/" != path.back()) {
            path.append("/");
        }
    }
    return path;
}

const char Dandelion::convertU82Char(uint8_t value)
{
    static const char numberArray[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'A', 'B', 'C', 'D', 'E', 'F'
    };

    if(value < 0x0F)
    {
        return numberArray[value];
    }
    else
    {
        return 0;
    }
}

QColor Dandelion::getLightColor()
{
    static const double min = 0;
    static const double max = 255;
    static std::random_device rd; //使用硬件生成随机数种子
    static std::mt19937 engine(rd()); //使用种子生成随机数引擎
    static std::uniform_int_distribution<> distribution(min, max);//设置产生的随机数的类型以及范围

    int rgb[3] = {};
    do{
        rgb[0] = distribution(engine);
        rgb[1] = distribution(engine);
        rgb[2] = distribution(engine);
        //DEBUGPREFIX << rgb[0] << rgb[1] << rgb[2] << (rgb[0] * 0.299 + rgb[1] * 0.587 + rgb[2] * 0.114);
    }while(isDeepColor(rgb)); // 深色

    return QColor(rgb[0], rgb[1], rgb[2]);
}

QColor Dandelion::getDeepColor()
{
    static const double min = 0;
    static const double max = 255;
    static std::random_device rd; //使用硬件生成随机数种子
    static std::mt19937 engine(rd()); //使用种子生成随机数引擎
    static std::uniform_int_distribution<> distribution(min, max);//设置产生的随机数的类型以及范围

    int rgb[3] = {};
    do{
        rgb[0] = distribution(engine);
        rgb[1] = distribution(engine);
        rgb[2] = distribution(engine);
        //DEBUGPREFIX << rgb[0] << rgb[1] << rgb[2] << (rgb[0] * 0.299 + rgb[1] * 0.587 + rgb[2] * 0.114);
    }while(isLightColor(rgb)); // 浅色

    return QColor(rgb[0], rgb[1], rgb[2]);
}
