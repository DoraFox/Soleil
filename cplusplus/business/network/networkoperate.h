#ifndef NETWORKOPERATE_H
#define NETWORKOPERATE_H

#include <midware/define/basedefine.h>

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>
#include <QUrl>
#include <QFileInfo>
#include <QDesktopServices>

class NetworkOperate : public QObject
{
    Q_OBJECT

public:

    typedef enum enumNetworkError{
        NetworkError_filePathEmpty,
        NetworkError_urlInvalid,
        NetworkError_downloadDirectoryEmpty,
        NetworkError_newFileOpenFailed,

        NetworkError_none
    }NetworkError;
    Q_ENUM(enumNetworkError)

    explicit NetworkOperate(QObject *parent = nullptr);

signals:
    void networkErrorSiganl(int id, int error);
    void sendDownloadProgress(int id, qint64 bytesReceived, qint64 bytesTotal);

    void startDownload(const QString& filePath);

    DECLARE_PROPERTY_SIGNAL(QString, downloadDirctory)

    void downloadFinishedSignal();

    void exitDownload();

public slots:
    void download(const QString& filePath);

private slots:
    void downloadFinished();

    void readReceivedData();

    void onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    QNetworkAccessManager m_networkManager;//网络管理
    QNetworkReply* m_reply;   //网络响应
    QFile* m_downloadedFile;//下载保存的临时文件

    DECLARE_QML_PROPERTY(QString, downloadDirctory)

    DECLARE_PROPERTY(int, id);
};

#endif // NETWORKOPERATE_H
