#include "systemsettings.h"

#include <QFuture>
#include <QEventLoop>
#include <QTimer>
#include <QDate>
#include <QFile>

#ifdef Q_OS_WIN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winuser.h>
#else
#include <QProcess>
#endif

#include "cplusplus/midware/define/pathdefine.h"


SystemSettings::SystemSettings(QObject *parent)
    : QObject{parent}
    , m_settings(QSettings(CONFIG_FILE_PATH, QSettings::IniFormat))
{
    connect(this, &SystemSettings::asySignal,
            this, &SystemSettings::asyFunction,
            Qt::QueuedConnection);


    initializeSettings();
}

SystemSettings::~SystemSettings()
{
    QString filePath = MESSAGE_DATA_PATH; // 文件路径

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning() << "无法打开文件：" << filePath;
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8");  // 解决中文乱码
    for (const QString &line : m_messageDataModel) {
        out << line << "\n"; // 每行写入一条数据
    }

    file.close();
    qDebug() << "数据已成功写入：" << filePath;
}

void SystemSettings::testFunc()
{

#if 0

    Source source;
    Target target;

    source.theEvent += delegate(&target, &Target::onEvent);

    source.fireEvent(42);

    source.theEvent -= delegate(&target, &Target::onEvent);


    emit asySignal();

#endif
}

void SystemSettings::swapMouseButton(bool isLeft)
{
#ifdef Q_OS_WIN
    SwapMouseButton(!isLeft);
#else
    QProcess process;
    static QStringList leftList({"-e", "pointer = 1 2 3 4 5 6 7 8 9 10"});
    static QStringList rightList({"-e", "pointer = 3 2 1 4 5 6 7 8 9 10"});
    process.start("xmodmap", isLeft ? leftList : rightList);
    process.waitForFinished();
    //DEBUGPREFIX << process.readAllStandardOutput() << process.readAllStandardError();
    process.close();
#endif

}

bool SystemSettings::isCurrentMouseLeft()
{
#ifdef Q_OS_WIN
    bool flag = ::SwapMouseButton(false);
    if(flag)
    {
        ::SwapMouseButton(true); // restore to right button
        return false;
    }
    else
    {
        return true;
    }
#else
    QProcess process;
    process.start("xmodmap", {"-pp"});
    process.waitForFinished();
    QStringList output = QString::fromUtf8(process.readAllStandardOutput()).split('\n');
    //DEBUGPREFIX << process.readAllStandardOutput() << process.readAllStandardError();
    process.close();

    QChar leftBtnCode = output.at(4).back();
    QChar rightBtnCode = output.at(6).back();
    static QChar stdLeftBtnCode('1');
    //DEBUGPREFIX << leftBtnCode << rightBtnCode;
    static QChar stdRightBtnCode('3');
    if(stdLeftBtnCode == leftBtnCode && stdRightBtnCode == rightBtnCode)
    {
        return true;
    }
    else if(stdRightBtnCode == leftBtnCode && stdLeftBtnCode == rightBtnCode)
    {
        return false;
    }
    else
    {
        return true;
    }
#endif
}

void SystemSettings::initializeSettings(bool reset) {
    auto getOrSetDefault = [this, reset](const QString &key, const QVariant &defaultValue) {
        if (reset || !m_settings.contains(key) || m_settings.value(key).toString().isEmpty()) {
            m_settings.setValue(key, defaultValue);
        }
        return m_settings.value(key);
    };

    // 设置默认路径
    m_browserPath = getOrSetDefault("Browser/browserPath", "C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe").toString();
    m_port = getOrSetDefault("Browser/port", 1234).toInt();
    m_userDataPath = getOrSetDefault("Paths/userDataPath", APP_DIR_PTH + "/python/user_data").toString();

    // 读取视频相关配置
    m_videoCount = getOrSetDefault("Vedio/videoCount", 100).toInt();
    m_getSecondLevelComments = getOrSetDefault("Vedio/getSecondLevelComments", true).toBool();
    m_firstLevelCommentsCount = getOrSetDefault("Vedio/firstLevelCommentsCount", 100).toInt();
    m_secondLevelCommentsCount = getOrSetDefault("Vedio/secondLevelCommentsCount", 20).toInt();

    // 读取并确保只存储年月日
    QString defaultStartDate = QDate::currentDate().addDays(-7).toString("yyyy-MM-dd");
    QString defaultEndDate = QDate::currentDate().toString("yyyy-MM-dd");

    m_commentStartTime = getOrSetDefault("Comment/commentStartTime", defaultStartDate).toString();
    m_commentEndTime = getOrSetDefault("Comment/commentEndTime", defaultEndDate).toString();

    loadMessageDataFile();
}

void SystemSettings::setBrowserPath(const QString &path) {
    if (m_browserPath != path) {
        m_browserPath = path;
        m_settings.setValue("Browser/browserPath", path);
        emit browserPathChanged(m_browserPath);
    }
}

void SystemSettings::setPort(int newPort) {
    if (m_port != newPort) {
        m_port = newPort;
        m_settings.setValue("Browser/port", newPort);
        emit portChanged(m_port);
    }
}

void SystemSettings::setUserDataPath(const QString &path) {
    if (m_userDataPath != path) {
        m_userDataPath = path;
        m_settings.setValue("Paths/userDataPath", path);
        emit userDataPathChanged(m_userDataPath);
    }
}

void SystemSettings::setVideoCount(int count) {
    if (m_videoCount != count) {
        m_videoCount = count;
        m_settings.setValue("Vedio/videoCount", count);
        emit videoCountChanged(count);
    }
}

void SystemSettings::setGetSecondLevelComments(bool enabled) {
    if (m_getSecondLevelComments != enabled) {
        m_getSecondLevelComments = enabled;
        m_settings.setValue("Vedio/getSecondLevelComments", enabled);
        emit getSecondLevelCommentsChanged(enabled);
    }
}

void SystemSettings::setFirstLevelCommentsCount(int count) {
    if (m_firstLevelCommentsCount != count) {
        m_firstLevelCommentsCount = count;
        m_settings.setValue("Vedio/firstLevelCommentsCount", count);
        emit firstLevelCommentsCountChanged(count);
    }
}

void SystemSettings::setSecondLevelCommentsCount(int count) {
    if (m_secondLevelCommentsCount != count) {
        m_secondLevelCommentsCount = count;
        m_settings.setValue("Vedio/secondLevelCommentsCount", count);
        emit secondLevelCommentsCountChanged(count);
    }
}

void SystemSettings::setCommentStartTime(const QString& date)
{
    if (m_commentStartTime != date) {
        m_commentStartTime = date;
        m_settings.setValue("Comment/commentStartTime", date);
        emit commentStartTimeChanged(m_commentStartTime);
    }
}

void SystemSettings::setCommentEndTime(const QString &date)
{
    if (m_commentEndTime != date) {
        m_commentEndTime = date;
        m_settings.setValue("Comment/commentEndTime", date);
        emit commentStartTimeChanged(m_commentEndTime);
    }
}

void SystemSettings::setMessageDataModel(int index, const QString &data)
{
    if(index < 0 || index >= m_messageDataModel.size())
    {
        return;
    }

    m_messageDataModel[index] = data;

    emit messageDataModelChanged(m_messageDataModel);
}


void SystemSettings::asyFunction()
{
    /*
    QFuture<void> asyFunction = QtConcurrent::run([](){
        QTimer timer;
        QEventLoop loop;
        timer.start(5 * 1000);

        timer.callOnTimeout([&]{
            timer.stop();
            loop.exit(1);
        });

        int ret = loop.exec();

        DEBUGPREFIX << ret;

    });
    */

    DEBUGPREFIX;

    //asyFunction.waitForFinished();

    DEBUGPREFIX;

}

void SystemSettings::loadMessageDataFile() {
    QString filePath = MESSAGE_DATA_PATH;
    QFile file(filePath);

    // 如果文件不存在，则创建新文件并写入默认值
    if (!file.exists()) {
        qDebug() << "文件不存在，创建新文件：" << filePath;

        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return;
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");  // 解决中文乱码

        //out.setCodec(QTextCodec::codecForName("utf-8"));
        for (QString msg : DEFAULT_MESSAGE_DATA) {
            out << msg << "\n";
        }

        file.close();

        qDebug() << "默认文件已创建";
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning("无法打开文件");
        return;
    }

    m_messageDataModel.clear();
    QTextStream in(&file);
    in.setCodec("UTF-8");  // 解决中文乱码
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            m_messageDataModel.append(line);
        }
    }
    file.close();

    emit messageDataModelChanged(m_messageDataModel);
}


