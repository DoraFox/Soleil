#include "networkoperate.h"
#include <QThread>


NetworkOperate::NetworkOperate(QObject *parent)
    : QObject{parent}
    , m_networkManager(new QNetworkAccessManager(this))
    , m_reply(Q_NULLPTR)
    , m_downloadedFile(Q_NULLPTR)
    , m_id(-1)
{

}

void NetworkOperate::download(const QString &filePath)
{
    bool success = false;
    do{
        DEBUGPREFIX << m_id << ":" << QThread::currentThreadId();
        if (filePath.isEmpty())
        {
            break;
        }

        QUrl url = QUrl::fromUserInput(filePath);//URL地址
        DEBUGPREFIX << url.path() << filePath;
        if (!url.isValid())
        {
            emit networkErrorSiganl(m_id, NetworkError_urlInvalid);
            break;
        }

        if (m_downloadDirctory.isEmpty())
        {
            emit networkErrorSiganl(m_id, NetworkError_downloadDirectoryEmpty);
            break;
        }

        QString fullFileName = m_downloadDirctory + url.fileName(); //文件名

        if (QFile::exists(fullFileName))
        {
            QFile::remove(fullFileName);
        }

        m_downloadedFile = new QFile(fullFileName);//创建临时文件
        if (!m_downloadedFile->open(QIODevice::WriteOnly))
        {
            emit networkErrorSiganl(m_id, NetworkError_newFileOpenFailed);
            break;
        }

        //发送网络请求，创建网络响应
        m_reply = m_networkManager.get(QNetworkRequest(url));

        connect(m_reply, &QNetworkReply::finished, this, &NetworkOperate::downloadFinished);
        connect(m_reply, &QNetworkReply::readyRead, this, &NetworkOperate::readReceivedData);
        connect(m_reply, &QNetworkReply::downloadProgress, this, &NetworkOperate::onDownloadProgress);

        success = true;
    }while(0);

    if(!success)
    {
        emit downloadFinishedSignal();
    }
}

void NetworkOperate::downloadFinished()
{
    QFileInfo fileInfo;
    fileInfo.setFile(m_downloadedFile->fileName());
    DEBUGPREFIX << m_downloadedFile->fileName() << ',' << fileInfo.filePath() << ',' << fileInfo.fileName();

    m_downloadedFile->close();
    delete m_downloadedFile;
    m_downloadedFile = Q_NULLPTR;

    m_reply->deleteLater();
    m_reply = Q_NULLPTR;

    emit downloadFinishedSignal();

    //if (ui->checkOpen->isChecked())//打开下载的文件
    //QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absoluteFilePath()));
}

void NetworkOperate::readReceivedData(){
    m_downloadedFile->write(m_reply->readAll());
}

void NetworkOperate::onDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    emit sendDownloadProgress(m_id, bytesReceived, bytesTotal);
}
