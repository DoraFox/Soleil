#include "workthread.h"

#include <midware/define/basedefine.h>

#ifdef Q_OS_WIN
#include <profileapi.h>
#include <synchapi.h>
#include <winnt.h>
#include <winsock2.h>
#endif

#include <chrono>
#include <thread>

#include <QTimer>

using namespace std::chrono;


#define BUFFER_SIZE 2048
#define TEST_TIMES 10
#define Interval_Microseconds 5000

void Precision_sleep_for()
{
    std::string buffer;
    buffer.assign(BUFFER_SIZE, 0);
    buffer.clear();
    int i = TEST_TIMES;
    uint64_t total_used = 0;
    while (i) {
        i--;
        steady_clock::time_point time_begin = steady_clock::now();
        std::this_thread::sleep_for(microseconds(Interval_Microseconds));
        steady_clock::time_point time_end = steady_clock::now();
        char tmp[128] = {0};
        uint64_t used = duration_cast<microseconds>(time_end - time_begin).count();
        snprintf(tmp, 128, "%s Sleep %d us, time used : %lld us\n",
                 __FUNCTION__, Interval_Microseconds, used);
        total_used += used;
        buffer += tmp;
    }
    printf("%s", buffer.c_str());
    printf("%s Sleep %d us , avatar %lld us\n\n",
           __FUNCTION__, Interval_Microseconds, total_used / TEST_TIMES);
}

#ifdef Q_OS_WIN
void test()
{
    LARGE_INTEGER litmp = {0};
    LONGLONG startTime = 0;
    LONGLONG endTime = 0;
    double gap = 0;
    double frequency = 0;
    double time = 0;
    QueryPerformanceFrequency(&litmp);
    frequency = (double)litmp.QuadPart;// 获得计数器的时钟频率
    QueryPerformanceCounter(&litmp);
    startTime = litmp.QuadPart;// 获得初始值
    do {
        QueryPerformanceCounter(&litmp);
        endTime = litmp.QuadPart;//获得终止值
        gap = (double)(endTime - startTime);
        time = gap / frequency;// 获得对应的时间值，单位为秒
    }while(time < 0.005);
    DEBUGPREFIX << QString::number(time, 'f', 16);
}

void test2()
{
    LARGE_INTEGER litmp = {0};
    LONGLONG startTime = 0;
    LONGLONG endTime = 0;
    LONGLONG frequency = 0;
    LONGLONG time = 0;
    QueryPerformanceFrequency(&litmp);
    frequency = litmp.QuadPart;// 获得计数器的时钟频率
    QueryPerformanceCounter(&litmp);
    startTime = litmp.QuadPart;// 获得初始值
    do {
        QueryPerformanceCounter(&litmp);
        endTime = litmp.QuadPart;//获得终止值
        time = (endTime - startTime) * 1000000 / frequency;// 获得对应的时间值，单位为微秒
    }while(time < 1000);

    DEBUGPREFIX << QString::number(time, 'f', 16) << QThread::currentThreadId();
}
#endif


WorkThread::WorkThread(QObject *parent)
    : QThread{parent}
{

}



void WorkThread::run() {
    //timeBeginPeriod(1);
    //while (1)
    {
        //Precision_sleep_for();
        //test();
        QTimer timer;
        timer.setInterval(1);
        connect(&timer, &QTimer::timeout, [&](){
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            system_clock::duration tp = start.time_since_epoch();
            milliseconds ms  = duration_cast<milliseconds>(tp);
            DEBUGPREFIX << ms.count();
        });
        timer.setTimerType(Qt::PreciseTimer);
        timer.start();

        exec();
    }

    //timeEndPeriod(1);
/*
    QString result;

    DEBUGPREFIX << QThread::currentThreadId();

    while (1) {
        test();


        emit resultReady(result);
    }
    */
}
