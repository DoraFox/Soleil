#include "s19manager.h"

#include <midware/define/basedefine.h>
#include "business/file/fileoperate.h"
#include "s19operate.h"
#include <QUrl>
#include <QThreadPool>

S19Manager::S19Manager(QObject *parent)
    : QObject{parent}
{
}

void S19Manager::startSwapNumbeUpdate() {
    QUrl url = QUrl::fromUserInput(
        R"(D:\Git_project\Soleil\VBF_S19)");

    QStringList pathList;
    FileOperate::getAllFiles(pathList, url, QStringList({"*.s19"}), true);

    QVector<int> address;
    QVector<int> value;

    int addressStart = 0x00000000;

        // SMD HIGH
        // SMD LOW
        // SMD FULL
    value << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00
          << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00
          << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00

        // SMP HIGH
        // SMP LOW
        // SMP FULL
          << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00
          << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00
          << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00

        // SMB HIGH
        // SMB LOW
        // SMB FULL
          << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00
          << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00
          << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00 << 0x00
        ;

    int size = value.size();
    address.reserve(size);

    for (int i = 0; i < size; ++i) {
        address.append(addressStart + i);
    }

    QThreadPool* pool = QThreadPool::globalInstance();

    DEBUGPREFIX <<"QThreadPool:" << pool->maxThreadCount() << pool->expiryTimeout() << pool->stackSize();

    for (const QString &path : qAsConst(pathList))
    {
        S19Operate* opt = new S19Operate();
        opt->setReplaceInfo(path, address, value);
        pool->start(opt);
    }
}

void S19Manager::startSwapNumbeUpdate(const QUrl &url, QVector<int> address, QVector<int> value)
{
    QStringList pathList;
    FileOperate::getAllFiles(pathList, url, QStringList({"*.s19"}), true);

    QThreadPool* pool = QThreadPool::globalInstance();

    DEBUGPREFIX <<"QThreadPool:" << pool->maxThreadCount() << pool->expiryTimeout() << pool->stackSize();

    for (const QString &path : qAsConst(pathList))
    {
        S19Operate* opt = new S19Operate();

        opt->setReplaceInfo(path, address, value);
        pool->start(opt);
    }
}

void S19Manager::s19OptFinished()
{

}
