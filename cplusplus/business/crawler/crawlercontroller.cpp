#include "crawlercontroller.h"

#include <midware/define/basedefine.h>
#include "register/classregister.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QCoro/websockets/qcorowebsockets.h>


CrawlerController::CrawlerController(QObject *parent)
    : QObject(parent)
    , pythonProcess(nullptr)
    , webSocket(nullptr)
    , coroWebSocket(nullptr)
    , awemeModel(nullptr)
    , commentModel(nullptr)
{
    pythonProcess = new QProcess(this);
    connect(pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CrawlerController::onPythonFinished);

    webSocket = new QWebSocket();
    coroWebSocket = qCoro(webSocket);

    connect(webSocket, &QWebSocket::connected, this, &CrawlerController::onConnected);
    connect(webSocket, &QWebSocket::textMessageReceived, this, &CrawlerController::onMessageReceived);
    connect(webSocket, &QWebSocket::disconnected, this, &CrawlerController::onDisconnected);
    connect(webSocket, &QWebSocket::stateChanged, this, &CrawlerController::onStateChanged);

    awemeModel = new AwemeModel(this);
    commentModel = new CommentModel(this);

    QQmlContext* ptrContext = ClassRegister::GetInstance()->engine()->rootContext();
    ptrContext->setContextProperty("commentModel", commentModel);
    ptrContext->setContextProperty("awemeModel", awemeModel);
}

QCoro::Task<bool> CrawlerController::startPython()
{
    if (pythonProcess->state() == QProcess::NotRunning) {
        QString pythonPath = "C:/Users/BoolPing/AppData/Local/Programs/Python/Python39/python.exe";  // 确保 Python 在 PATH 中
        QString scriptPath = "D:/QtCreatorProjects/pyqt/main.py";  // Python 爬虫路径

        pythonProcess->start(pythonPath, QStringList() << scriptPath);
        if (!pythonProcess->waitForStarted()) {
            qDebug() << "爬虫启动失败" << pythonProcess->error();
            co_return false;
        }
        qDebug() << "Python 爬虫已启动";
    }

    co_return true;
}

bool CrawlerController::launchEdge()
{
    static auto innerFunc = [this]()->QCoro::Task<bool>{
        // bool result = co_await startPython();
        // if(!result)
        // {
        //     DEBUGPREFIX << "start pthon successful";
        //     co_return false;
        // }
        // else
        // {
        //     DEBUGPREFIX << "start pthon failed";
        // }

        bool connected = co_await connectPthon();
        DEBUGPREFIX << "connected:" << connected;

        QJsonObject command;
        command["command"] = "launch_edge";

        sendTextMessage(command);

        co_return true;
    };
    innerFunc();
    return true;
}

QCoro::Task<bool> CrawlerController::connectPthon()
{
    if (webSocket->state() != QAbstractSocket::ConnectedState)
    {
        co_return co_await coroWebSocket.open(QUrl("ws://localhost:8765"), std::chrono::milliseconds(10000));
    }
    else
    {
        co_return true;
    }
}

void CrawlerController::startCrawler(QString keyword)
{
    QJsonObject command;
    command["command"] = "start";
    command["keyword"] = keyword;

    sendTextMessage(command);
}

void CrawlerController::stopCrawler()
{
    if (webSocket->state() != QAbstractSocket::ConnectedState) {
        DEBUGPREFIX << "WebSocket already is unconnected";
        return;
    }

    QJsonObject command;
    command["command"] = "stop";
    sendTextMessage(command);

    // if (pythonProcess->state() != QProcess::NotRunning) {
    //     pythonProcess->terminate();
    // }

    DEBUGPREFIX << "WebSocket disconnected";
}

void CrawlerController::sendPrivateMessage(QString msg, QString sec_uid)
{
    QJsonObject command;
    command["command"] = "send_message";
    command["message"] = msg;
    command["sec_uid"] = sec_uid;

    sendTextMessage(command);
}

void CrawlerController::onConnected()
{
    qDebug() << "WebSocket connected";
}

void CrawlerController::onDisconnected()
{
    qDebug() << "WebSocket disconnected";
}

void CrawlerController::onMessageReceived(QString message)
{
    DEBUGPREFIX << message;

    QJsonDocument doc = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject obj = doc.object();

    if (obj.contains("msg_type"))
    {
        if(obj["msg_type"].toString() == "aweme")
        {
            awemeModel->append(obj);
        }
        else if(obj["msg_type"].toString() == "comment")
        {
            commentModel->append(obj);
        }


    } else if (obj.contains("status")) {
        QString status = obj["status"].toString();
        DEBUGPREFIX << "STATUS：" << status;
    }
}

void CrawlerController::onPythonFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "Python exit，code:" << exitCode << exitStatus;
}

void CrawlerController::onStateChanged(QAbstractSocket::SocketState state)
{
    DEBUGPREFIX << state;
}

void CrawlerController::sendTextMessage(const QJsonObject &command)
{
    qint64 bytes = webSocket->sendTextMessage(QJsonDocument(command).toJson(QJsonDocument::Compact));

    DEBUGPREFIX << "sendTextMessage" << bytes << command;
}
