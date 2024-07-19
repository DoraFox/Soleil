#include "systemsettings.h"

#include <QFuture>
#include <QEventLoop>
#include <QTimer>

#include <Poco/DoubleByteEncoding.h>
#include <Poco/BasicEvent.h>
#include <Poco/Delegate.h>

#include "cplusplus/midware/define/basedefine.h"

#ifdef Q_OS_WIN
#include <winuser.h>
#else
#include <QProcess>
#endif


using Poco::BasicEvent;
using Poco::delegate;

class Source
{
public:
    BasicEvent<int> theEvent;

    void fireEvent(int n)
    {
        theEvent(this, n);
    }
};


class Target
{
public:
    void onEvent(const void* /*pSender*/, int& arg)
    {
        DEBUGPREFIX << "onEvent: " << arg;
    }
};



SystemSettings::SystemSettings(QObject *parent)
    : QObject{parent}
{
    connect(this, &SystemSettings::asySignal,
            this, &SystemSettings::asyFunction,
            Qt::QueuedConnection);
}

void SystemSettings::testFunc()
{

#if 0

    Source source;
    Target target;

    source.theEvent += delegate(&target, &Target::onEvent);

    source.fireEvent(42);

    source.theEvent -= delegate(&target, &Target::onEvent);


    emit asySignal();

#endif
}

void SystemSettings::swapMouseButton(bool isLeft)
{
#ifdef Q_OS_WIN
    SwapMouseButton(!isLeft);
#else
    QProcess process;
    static QStringList leftList({"-e", "pointer = 1 2 3 4 5 6 7 8 9 10"});
    static QStringList rightList({"-e", "pointer = 3 2 1 4 5 6 7 8 9 10"});
    process.start("xmodmap", isLeft ? leftList : rightList);
    process.waitForFinished();
    //DEBUGPREFIX << process.readAllStandardOutput() << process.readAllStandardError();
    process.close();
#endif

}

bool SystemSettings::isCurrentMouseLeft()
{
#ifdef Q_OS_WIN
    bool flag = SwapMouseButton(false);
    if(flag)
    {
        SwapMouseButton(true); // restore to right button
        return false;
    }
    else
    {
        return true;
    }
#else
    QProcess process;
    process.start("xmodmap", {"-pp"});
    process.waitForFinished();
    QStringList output = QString::fromUtf8(process.readAllStandardOutput()).split('\n');
    //DEBUGPREFIX << process.readAllStandardOutput() << process.readAllStandardError();
    process.close();

    QChar leftBtnCode = output.at(4).back();
    QChar rightBtnCode = output.at(6).back();
    static QChar stdLeftBtnCode('1');
    //DEBUGPREFIX << leftBtnCode << rightBtnCode;
    static QChar stdRightBtnCode('3');
    if(stdLeftBtnCode == leftBtnCode && stdRightBtnCode == rightBtnCode)
    {
        return true;
    }
    else if(stdRightBtnCode == leftBtnCode && stdLeftBtnCode == rightBtnCode)
    {
        return false;
    }
    else
    {
        return true;
    }
#endif
}

void SystemSettings::asyFunction()
{
    /*
    QFuture<void> asyFunction = QtConcurrent::run([](){
        QTimer timer;
        QEventLoop loop;
        timer.start(5 * 1000);

        timer.callOnTimeout([&]{
            timer.stop();
            loop.exit(1);
        });

        int ret = loop.exec();

        DEBUGPREFIX << ret;

    });
    */

    DEBUGPREFIX;

    //asyFunction.waitForFinished();

    DEBUGPREFIX;

}


