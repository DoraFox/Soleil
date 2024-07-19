#include "networkmanager.h"

#include <QDebug>
#include <iostream>
//#include <coroutine>
#include <thread>
#include <chrono>
//#include <asio.hpp>

#include <sstream>
#include <string>

using namespace std;
/*
struct task {
    struct promise_type {
        promise_type() {
            DEBUGPREFIX << "1.create promie object\n";
        }
        task get_return_object() {
            DEBUGPREFIX << "2.create coroutine return object, and the coroutine is created now\n";
            return {std::coroutine_handle<task::promise_type>::from_promise(*this)};
        }
        std::suspend_never initial_suspend() {
            DEBUGPREFIX << "3.do you want to susupend the current coroutine?\n";
            DEBUGPREFIX << "4.don't suspend because return std::suspend_never, so continue to execute coroutine body\n";
            return {};
        }
        std::suspend_never final_suspend() noexcept {
            DEBUGPREFIX << "13.coroutine body finished, do you want to susupend the current coroutine?\n";
            DEBUGPREFIX << "14.don't suspend because return std::suspend_never, and the continue will be automatically destroyed, bye\n";
            return {};
        }
        void return_void() {
            DEBUGPREFIX << "12.coroutine don't return value, so return_void is called\n";
        }
        void unhandled_exception() {}
    };

    std::coroutine_handle<task::promise_type> handle_;
};

struct awaiter {
    bool await_ready() {
        DEBUGPREFIX << "6.do you want to suspend current coroutine?\n";
        DEBUGPREFIX << "7.yes, suspend becase awaiter.await_ready() return false\n";
        return false;
    }
    void await_suspend(
            std::coroutine_handle<task::promise_type> handle) {
        DEBUGPREFIX << "8.execute awaiter.await_suspend()\n";
        std::thread([handle]() mutable { handle(); }).detach();
        DEBUGPREFIX << "9.a new thread lauched, and will return back to caller\n";
    }
    void await_resume() {}
};*/
/*
task test() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string str = ss.str();
    DEBUGPREFIX << "5.begin to execute coroutine body, the thread id=" << QString::fromStdString(str) << "\n";//#1
    co_await awaiter{};
    ss << std::this_thread::get_id();
    str = ss.str();
    DEBUGPREFIX << "11.coroutine resumed, continue execcute coroutine body now, the thread id=" << QString::fromStdString(str) << "\n";//#3
}
*/


NetworkManager::NetworkManager(QObject *parent)
    : QObject{parent}
    , m_downloadTheads{Q_NULLPTR, Q_NULLPTR, Q_NULLPTR, Q_NULLPTR}
    , m_networkOpts{Q_NULLPTR, Q_NULLPTR, Q_NULLPTR, Q_NULLPTR}
{
//    asio::io_context io_context;
//    asio::ip::tcp::socket socket(io_context);

//    test();
    m_isDownloading = false;
    m_downloadState = DownloadState_DownloadFinished;

    connect(this, &NetworkManager::networkErrorSiganl, [&](int id, int error){
        DEBUGPREFIX_ERROR << id << error;
    });

    //DEBUGPREFIX << "QSslSocket=" << QSslSocket::sslLibraryBuildVersionString();
    //DEBUGPREFIX << "OpenSSL支持情况:" << QSslSocket::supportsSsl();
}

NetworkManager::~NetworkManager()
{
    for(int i = 0; i < DOWNLOAD_THREAD_COUNT; ++i)
    {
        if(Q_NULLPTR != m_downloadTheads[i])
        {
            m_downloadTheads[i]->quit();
            m_downloadTheads[i]->wait();
        }
    }
}

void NetworkManager::startDownloadFiles(QString paths)
{
    if(paths.isEmpty())
    {
        return;
    }

    m_downloadPaths = paths.split('\n').toVector();

    int pathSize = m_downloadPaths.size();
    int size = (pathSize >= DOWNLOAD_THREAD_COUNT) ? DOWNLOAD_THREAD_COUNT : pathSize;

    bool startDownload = false;

    for(int i = 0; i < size; ++i)
    {
        m_networkOpts[i] = new NetworkOperate();
        NetworkOperate* networkOpt = m_networkOpts[i];
        networkOpt->set_id(i);
        networkOpt->set_downloadDirctory(m_downloadDirctory);

        if(Q_NULLPTR == m_downloadTheads[i])
        {
            m_downloadTheads[i] = new QThread(this);
        }
        networkOpt->moveToThread(m_downloadTheads[i]);

        connect(m_downloadTheads[i], &QThread::finished, networkOpt, &QObject::deleteLater);
        connect(m_downloadTheads[i], &QThread::finished, this, &NetworkManager::checkDownloadFinishd);
        connect(networkOpt, &NetworkOperate::startDownload, networkOpt, &NetworkOperate::download);
        connect(networkOpt, &NetworkOperate::networkErrorSiganl, this, &NetworkManager::networkErrorSiganl);
        connect(networkOpt, &NetworkOperate::sendDownloadProgress, this, &NetworkManager::sendDownloadProgress);
        connect(networkOpt, &NetworkOperate::downloadFinishedSignal, this, &NetworkManager::downloadNext);
        connect(networkOpt, &NetworkOperate::exitDownload, m_downloadTheads[i], &QThread::quit);
        m_downloadTheads[i]->start();
        emit networkOpt->startDownload(m_downloadPaths.takeFirst());

        startDownload = true;
    }

    if(startDownload)
    {
        set_downloadState(DownloadState_StartDownload);
    }

}

void NetworkManager::downloadNext()
{
    NetworkOperate* networkOpt = qobject_cast<NetworkOperate*>(sender());
    if(!m_downloadPaths.isEmpty())
    {
        m_mutex.lock();
        emit networkOpt->startDownload(m_downloadPaths.takeFirst());
        m_mutex.unlock();
    }
    else
    {
        emit networkOpt->exitDownload();
    }
}

void NetworkManager::checkDownloadFinishd()
{
    bool allFinshed = true;
    for(int i = 0; i < DOWNLOAD_THREAD_COUNT; ++i)
    {
        if((Q_NULLPTR != m_downloadTheads[i]) && m_downloadTheads[i]->isRunning())
        {
            allFinshed = false;
            break;
        }
    }

    if(allFinshed)
    {
        set_downloadState(DownloadState_DownloadFinished);
    }
}


