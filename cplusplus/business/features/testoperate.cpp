#include "testoperate.h"
#include <QDebug>

#include <QUrl>
#include <QSslSocket>

#include "qcoroiodevice.h"
#include "qcorotask.h"

#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

#include "workthread.h"

using Array = const int (*) [12];

void variable_test();

class MyServer : public QObject {
    Q_OBJECT
public:
    explicit MyServer(QHostAddress addr, uint16_t port) {
        mServer.listen(addr, port);
        connect(&mServer, &QTcpServer::newConnection, this, &MyServer::handleConnection);
    }

private Q_SLOTS:
    QCoro::Task<> handleConnection() {
        auto socket = mServer.nextPendingConnection();
        while (socket->isOpen()) {
            const auto data = co_await socket;
            socket->write("PONG: " + data);
        }
    }

private:
    QTcpServer mServer;
};

class MyClient : public QObject {
    Q_OBJECT
public:
    explicit MyClient(QHostAddress addr, uint16_t port) {
        mSocket.connectToHost(addr, port);
        connect(&mTimer, &QTimer::timeout, this, &MyClient::sendPing);
        mTimer.start(1ms);
    }

private Q_SLOTS:
    QCoro::Task<> sendPing() {
        DEBUGPREFIX << "Sending ping...";
        mSocket.write(QByteArray("PING #") + QByteArray::number(++mPing));
        const auto response = co_await mSocket;
        DEBUGPREFIX << "Received pong: " << response.constData();
    }

private:
    int mPing = 0;
    QTcpSocket mSocket;
    QTimer mTimer;
};

#include <stdio.h>
#include <stdlib.h>

// 定义包含柔性数组成员的结构体
struct flexible_array {
    int count;
    int data[];  // 柔性数组成员
};

void flex_array()
{
    int n = 5;  // 数组大小
    // 分配结构体和数组所需的内存
    struct flexible_array *fa = (struct flexible_array *)malloc(sizeof(struct flexible_array) + n * sizeof(int));
    if (fa == NULL) {
        perror("malloc");
        return;
    }

    DEBUGPREFIX << sizeof(struct flexible_array);

    fa->count = n;
    // 初始化数组成员
    for (int i = 0; i < n; i++) {
        fa->data[i] = i * 10;
    }

    // 打印数组成员
    for (int i = 0; i < fa->count; i++) {
        printf("data[%d] = %d\n", i, fa->data[i]);
    }

    // 释放分配的内存
    free(fa);

}


TestOperate::TestOperate(QObject *parent)
    : QObject{parent}
    , m_enabled(true)
{
    MyServer* server = new MyServer{QHostAddress::LocalHost, 6666};
    Q_UNUSED(server)
    //MyClient* client = new MyClient{QHostAddress::LocalHost, 6666};

}

void TestOperate::qmlTestFunction()
{
    WorkThread *workThread = new WorkThread(this);
    //connect(workThread, &WorkThread::resultReady, this, &MyObject::handleResults);
    connect(workThread, &WorkThread::finished, workThread, &QObject::deleteLater);
    workThread->start();
}

void TestOperate::definedFunc()
{

    DEBUGPREFIX << __FUNCTION__
                << __func__
                //<< __PRETTY_FUNCTION__
                << __FILE__
                << __LINE__
        ;
}
/*
void variable_test()
{
    int aaa = 0x12345678;
    char* pa = (char*)&aaa;
    int q = 5;
    int ttt = 0xF1D8;
    int b1 = (ttt >> 8);
    int b2 = (ttt & 0xFF);

    //const int (*qq) [12] = &u16a_eepm_valid_ccp_index;
    //Array temp = &u16a_eepm_valid_ccp_index;

    QString str = QByteArray::fromHex("12345678");

    QString val = QString::number(0xF5 >> 4, 16);
    DEBUGPREFIX << str.toUtf8().toHex() << val << val.toUtf8() << val.toUtf8().toHex();

    uint16_t data = 0x0000;

    DEBUGPREFIX << Qt::hex << data;

    memset(&data, 0xF2, sizeof(data));

    DEBUGPREFIX << Qt::hex << data;
}
*/
void fun_ref(int (&p)[7])
{
    // Printing size of array
    qDebug() << __FUNCTION__
             << sizeof(p);
}

void fun_ptr(int (*p)[7])
{
    // Printing size of array
    qDebug() << __FUNCTION__
             << sizeof(*p)
             << sizeof(p);
}

void fun_const_ptr(const int (*p)[7])
{
    // Printing size of array
    qDebug() << __FUNCTION__
             << sizeof(*p);
}

void fun_test(){
    int arr[7] = {1,2,3,4,5,6,7};
    fun_ref(arr);
    fun_ptr(&arr);
    fun_const_ptr(&arr);

    QVariant var;
    int aaa = 10;
    var.setValue(aaa);

    QVariant var1;
    QString aaa1("10");
    var1.setValue(aaa1);

    qDebug() << "qqqq:"
             << var.canConvert(QMetaType::Int)
             << var.canConvert(QMetaType::QString)
             << var1.canConvert(QMetaType::Int)
             << var1.canConvert(QMetaType::QString)
             << var.type()
             << var1.type()
             << var.typeName()
             << var1.typeName()
        ;

}

#include "testoperate.moc"
