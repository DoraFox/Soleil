#include "crawlercontroller.h"

#include <midware/define/basedefine.h>
#include <midware/define/pathdefine.h>
#include "register/classregister.h"
#include <midware/tools/dandelion.h>

#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QCoro/websockets/qcorowebsockets.h>


CrawlerController::CrawlerController(QObject *parent)
    : QObject(parent)
    , m_pythonProcess(nullptr)
    , m_webSocket(nullptr)
    , m_coroWebSocket(nullptr)
    , m_awemeModel(nullptr)
    , m_commentModel(nullptr)
{
    m_pythonProcess = new QProcess(this);

    connect(m_pythonProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, &CrawlerController::onPythonFinished);

    // 连接信号到槽函数，以便捕获输出
    //connect(m_pythonProcess, &QProcess::readyReadStandardOutput, this, &CrawlerController::readStandardOutput);
    //connect(m_pythonProcess, &QProcess::readyReadStandardError, this, &CrawlerController::readStandardError);

    set_pythonState(m_pythonProcess->state());
    connect(m_pythonProcess, &QProcess::stateChanged, this, [this](QProcess::ProcessState state){
        DEBUGPREFIX << "set_pythonState:" << Dandelion::enumToString(state);
        set_pythonState(state);
    });

    m_webSocket = new QWebSocket();
    m_coroWebSocket = qCoro(m_webSocket);

    set_socketState(m_webSocket->state());

    connect(m_webSocket, &QWebSocket::connected, this, &CrawlerController::onConnected);
    connect(m_webSocket, &QWebSocket::textMessageReceived, this, &CrawlerController::onMessageReceived);
    connect(m_webSocket, &QWebSocket::disconnected, this, &CrawlerController::onDisconnected);
    connect(m_webSocket, &QWebSocket::stateChanged, this, [this](QAbstractSocket::SocketState state){
        DEBUGPREFIX << "set_socketState:" << Dandelion::enumToString(state);
        set_socketState(state);
    });

    m_awemeModel = new AwemeModel(this);
    m_commentModel = new CommentModel(this);

    m_awemeModel->init();
    m_commentModel->loadCommentsFromDatabase(m_awemeModel->getAwemesId());

    connect(m_awemeModel, &AwemeModel::deletedAweme, m_commentModel, &CommentModel::onDeletedAweme);

    QQmlContext* ptrContext = ClassRegister::GetInstance()->engine()->rootContext();
    ptrContext->setContextProperty("awemeModel", m_awemeModel);
    ptrContext->setContextProperty("commentModel", m_commentModel);

    RegisterQmlType(CommentModel, 1, 0);
}

CrawlerController::~CrawlerController()
{
    DEBUGPREFIX << "EXIT";

    m_webSocket->close();
    m_webSocket->deleteLater();
    m_webSocket = nullptr;

    m_pythonProcess->deleteLater();
    m_pythonProcess = nullptr;

    m_awemeModel->deleteLater();
    m_awemeModel = nullptr;
    m_commentModel->deleteLater();
    m_commentModel = nullptr;
}

QCoro::Task<bool> CrawlerController::startPython()
{
#ifdef QT_DEBUG
    if (m_pythonProcess->state() == QProcess::NotRunning) {
        QString pythonPath = "D:/QtCreatorProjects/pyqt/.venv/Scripts/python.exe";  // 确保 Python 在 PATH 中
        QString scriptPath = "D:/QtCreatorProjects/pyqt/main.py";  // Python 爬虫路径

        DEBUGPREFIX << pythonPath << scriptPath;

        m_pythonProcess->start(pythonPath, QStringList() << scriptPath);
        if (!m_pythonProcess->waitForStarted()) {
            qDebug() << "爬虫启动失败" << m_pythonProcess->error();
            co_return false;
        }
        qDebug() << "Python 爬虫已启动";
    }

    co_return true;

#else
    if (m_pythonProcess->state() == QProcess::NotRunning)
    {
        QString programPath = APP_DIR_PTH + "/python/crawler.exe";  // 确保 Python 在 PATH 中

        DEBUGPREFIX << "programPath:" << programPath;

        // 设置要运行的程序
        m_pythonProcess->setProgram(programPath);

        // 启动程序
        m_pythonProcess->start();

        // 检查是否启动成功
        if (m_pythonProcess->waitForStarted())
        {
            DEBUGPREFIX << "Program started successfully.";
            co_return true;
        }
        else
        {
            DEBUGPREFIX << "Failed to start the program.";
            co_return false;
        }
    }

    co_return true;
#endif
}

bool CrawlerController::launchEdge()
{
    static auto innerFunc = [this]()->QCoro::Task<bool>{
        bool result = co_await startPython();
        if(result)
        {
            DEBUGPREFIX << "start pthon successful";
        }
        else
        {
            DEBUGPREFIX << "start pthon failed";
            co_return false;
        }

        bool connected = co_await connectPthon();
        DEBUGPREFIX << "connected:" << connected;

        if(checkWebSocketConnected())
        {
            QJsonObject command;
            command["command"] = "launch_edge";

            sendTextMessage(command);
            co_return true;
        }
        else
        {
            co_return false;
        }

    };

    innerFunc();
    return true;
}

QCoro::Task<bool> CrawlerController::connectPthon()
{
    if (m_webSocket->state() != QAbstractSocket::ConnectedState)
    {
        co_return co_await m_coroWebSocket.open(QUrl("ws://localhost:8765"), std::chrono::milliseconds(10000));
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
    if (m_webSocket->state() != QAbstractSocket::ConnectedState) {
        DEBUGPREFIX << "WebSocket already is unconnected";
        return;
    }

    QJsonObject command;
    command["command"] = "stop";
    sendTextMessage(command);

    // if (m_pythonProcess->state() != QProcess::NotRunning) {
    //     m_pythonProcess->terminate();
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
    DEBUGPREFIX << "WebSocket connected";
}

void CrawlerController::onDisconnected()
{
    DEBUGPREFIX << "WebSocket disconnected";
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
            m_awemeModel->append(obj);
        }
        else if(obj["msg_type"].toString() == "comment")
        {
            m_commentModel->append(obj);
        }


    } else if (obj.contains("status")) {
        QString status = obj["status"].toString();
        DEBUGPREFIX << "STATUS：" << status;
    }
}

void CrawlerController::onPythonFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    DEBUGPREFIX << "Python exit，code:" << exitCode << exitStatus;
}

void CrawlerController::readStandardOutput()
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (process) {
        QByteArray output = process->readAllStandardOutput();
        QString decodedOutput = QString::fromLocal8Bit(output); // 将字节流解码为 UTF-8 字符串
        DEBUGPREFIX << "stdout:" << decodedOutput;
    }
}

void CrawlerController::readStandardError()
{
    QProcess *process = qobject_cast<QProcess *>(sender());
    if (process) {
        QByteArray errorOutput = process->readAllStandardError();
        QString decodedOutput = QString::fromLocal8Bit(errorOutput); // 将字节流解码为 UTF-8 字符串
        DEBUGPREFIX << "stderr:" << decodedOutput;
    }
}

void CrawlerController::sendTextMessage(const QJsonObject &command)
{
    qint64 bytes = m_webSocket->sendTextMessage(QJsonDocument(command).toJson(QJsonDocument::Compact));

    DEBUGPREFIX << "sendTextMessage" << bytes << command;
}

bool CrawlerController::checkWebSocketConnected()
{
    auto state = m_webSocket->state();
    if(state != QAbstractSocket::SocketState::ConnectedState)
    {
        DEBUGPREFIX << m_webSocket->error() << m_webSocket->errorString();
        return false;
    }
    return true;

}
