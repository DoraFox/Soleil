#ifndef CRAWLERCONTROLLER_H
#define CRAWLERCONTROLLER_H

#include <QObject>
#include <QProcess>
#include <QWebSocket>

#include <QCoro/websockets/qcorowebsockets.h>
#include <midware/define/basedefine.h>

#include "awememodel.h"
#include "commentmodel.h"

using QCoroWebSocket = QCoro::detail::QCoroWebSocket;

class CrawlerController : public QObject {
    Q_OBJECT

    Q_PROPERTY(QAbstractSocket::SocketState socketState MEMBER m_socketState NOTIFY socketStateChanged)
    Q_PROPERTY(QProcess::ProcessState pythonState MEMBER m_pythonState NOTIFY pythonStateChanged)

public:

    explicit CrawlerController(QObject *parent = nullptr);
    ~CrawlerController();

    Q_INVOKABLE QCoro::Task<bool> startPython();

    Q_INVOKABLE bool launchEdge();
    Q_INVOKABLE QCoro::Task<bool> connectPthon();

    Q_INVOKABLE void startCrawler(QString keyword);
    Q_INVOKABLE void stopCrawler();

    Q_INVOKABLE void sendPrivateMessage(QString msg, QString sec_uid);

    //Q_INVOKABLE void setConfig();

signals:
    DECLARE_PROPERTY_SIGNAL(QAbstractSocket::SocketState, socketState)
    DECLARE_PROPERTY_SIGNAL(QProcess::ProcessState, pythonState)

private slots:
    void onConnected();
    void onMessageReceived(QString message);
    void onDisconnected();
    void onPythonFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void readStandardOutput();

    void readStandardError();

private:
    void sendTextMessage(const QJsonObject& command);
    bool checkWebSocketConnected();

private:
    QProcess* m_pythonProcess;
    QWebSocket* m_webSocket;
    QCoroWebSocket m_coroWebSocket;

    AwemeModel* m_awemeModel;
    CommentModel* m_commentModel;

    DECLARE_QML_PROPERTY(QAbstractSocket::SocketState, socketState)
    DECLARE_QML_PROPERTY(QProcess::ProcessState, pythonState)
};

#endif // CRAWLERCONTROLLER_H
