#ifndef CRAWLERCONTROLLER_H
#define CRAWLERCONTROLLER_H

#include <QObject>
#include <QProcess>
#include <QWebSocket>

#include <QCoro/websockets/qcorowebsockets.h>

#include "awememodel.h"
#include "commentmodel.h"

using QCoroWebSocket = QCoro::detail::QCoroWebSocket;

class CrawlerController : public QObject {
    Q_OBJECT
public:
    explicit CrawlerController(QObject *parent = nullptr);

    Q_INVOKABLE QCoro::Task<bool> startPython();

    Q_INVOKABLE bool launchEdge();
    Q_INVOKABLE QCoro::Task<bool> connectPthon();

    Q_INVOKABLE void startCrawler(QString keyword);
    Q_INVOKABLE void stopCrawler();

    Q_INVOKABLE void sendPrivateMessage(QString msg, QString sec_uid);

signals:
    void dataReceived(QString keyword, int index, QString data);

private slots:
    void onConnected();
    void onMessageReceived(QString message);
    void onDisconnected();
    void onPythonFinished(int exitCode, QProcess::ExitStatus exitStatus);

    void onStateChanged(QAbstractSocket::SocketState state);

private:
    void sendTextMessage(const QJsonObject& command);

private:
    QProcess* pythonProcess;
    QWebSocket* webSocket;
    QCoroWebSocket coroWebSocket;

    AwemeModel* awemeModel;
    CommentModel* commentModel;
};

#endif // CRAWLERCONTROLLER_H
