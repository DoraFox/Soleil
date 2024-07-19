#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <midware/define/basedefine.h>

#include "business/network/networkoperate.h"

#include <QObject>
#include <QThread>
#include <QMutex>

#define DOWNLOAD_THREAD_COUNT 4

class NetworkManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString downloadDirctory MEMBER m_downloadDirctory NOTIFY downloadDirctoryChanged)
    Q_PROPERTY(int downloadState MEMBER m_downloadState NOTIFY downloadStateChanged)

public:
    typedef enum enumDownloadState{
        DownloadState_StartDownload,
        DownloadState_DownloadFinished,
    }enumDownloadState;
    Q_ENUM(enumDownloadState)

    explicit NetworkManager(QObject *parent = nullptr);

    ~NetworkManager();

    Q_INVOKABLE void startDownloadFiles(QString paths);

signals:

    DECLARE_PROPERTY_SIGNAL(QString, downloadDirctory)
    DECLARE_PROPERTY_SIGNAL(int, isDownloading)
    DECLARE_PROPERTY_SIGNAL(int, downloadState)

    void sendDownloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal);

    void networkErrorSiganl(int id, int error);

private slots:
    void downloadNext();

    void checkDownloadFinishd();

private:

    NetworkOperate* m_networkOpts[DOWNLOAD_THREAD_COUNT];
    QThread* m_downloadTheads[DOWNLOAD_THREAD_COUNT];

    DECLARE_QML_PROPERTY(QString, downloadDirctory)

    DECLARE_QML_PROPERTY(int, isDownloading)

    DECLARE_QML_PROPERTY(int, downloadState)

    QVector<QString> m_downloadPaths;

    QMutex m_mutex;

    QStringList dirList;
};

#endif // NETWORKMANAGER_H
