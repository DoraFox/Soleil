#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <iostream>

#include <QObject>
#include <QDebug>
#include <QVariant>
#include <QSettings>
#include <QString>
#include <midware/define/basedefine.h>
#include <minwindef.h>

using std::cout;
using std::endl;

class SystemSettings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString browserPath READ browserPath WRITE setBrowserPath NOTIFY browserPathChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString userDataPath READ userDataPath WRITE setUserDataPath NOTIFY userDataPathChanged)

    Q_PROPERTY(int videoCount READ videoCount WRITE setVideoCount NOTIFY videoCountChanged)
    Q_PROPERTY(bool getSecondLevelComments READ getSecondLevelComments WRITE setGetSecondLevelComments NOTIFY getSecondLevelCommentsChanged)
    Q_PROPERTY(int firstLevelCommentsCount READ firstLevelCommentsCount WRITE setFirstLevelCommentsCount NOTIFY firstLevelCommentsCountChanged)
    Q_PROPERTY(int secondLevelCommentsCount READ secondLevelCommentsCount WRITE setSecondLevelCommentsCount NOTIFY secondLevelCommentsCountChanged)

    Q_PROPERTY(QString commentStartTime READ commentStartTime WRITE setCommentStartTime NOTIFY commentStartTimeChanged)
    Q_PROPERTY(QString commentEndTime READ commentEndTime WRITE setCommentEndTime NOTIFY commentEndTimeChanged)

    Q_PROPERTY(QStringList messageDataModel READ messageDataModel NOTIFY messageDataModelChanged)

public:
    explicit SystemSettings(QObject *parent = nullptr);

    ~SystemSettings();

    void testFunc();

    /* 交换鼠标左右键 */
    Q_INVOKABLE void swapMouseButton(bool isLeft);

    Q_INVOKABLE bool isCurrentMouseLeft();

    Q_INVOKABLE void initializeSettings(bool reset = false);

    // 设置值并写入 QSettings
    void setBrowserPath(const QString &path);

    void setPort(int newPort);

    void setUserDataPath(const QString &path);

    void setVideoCount(int count);

    void setGetSecondLevelComments(bool enabled);

    void setFirstLevelCommentsCount(int count);

    void setSecondLevelCommentsCount(int count);

    void setCommentStartTime(const QString &date);

    void setCommentEndTime(const QString &date);

    Q_INVOKABLE void setMessageDataModel(int index, const QString& data);

signals:
    void asySignal();

    DECLARE_PROPERTY_SIGNAL(QString, browserPath)
    DECLARE_PROPERTY_SIGNAL(int, port)
    DECLARE_PROPERTY_SIGNAL(QString, userDataPath)

    DECLARE_PROPERTY_SIGNAL(int, videoCount)
    DECLARE_PROPERTY_SIGNAL(bool, getSecondLevelComments)
    DECLARE_PROPERTY_SIGNAL(int, firstLevelCommentsCount)
    DECLARE_PROPERTY_SIGNAL(int, secondLevelCommentsCount)

    DECLARE_PROPERTY_SIGNAL(QString, commentStartTime)
    DECLARE_PROPERTY_SIGNAL(QString, commentEndTime)

    DECLARE_PROPERTY_SIGNAL(QStringList, messageDataModel)


public slots:

    void asyFunction();

private:
    Q_INVOKABLE void loadMessageDataFile();

private:
    QSettings m_settings;

    DECLARE_PROPERTY_READ_ONLY(QString, browserPath)
    DECLARE_PROPERTY_READ_ONLY(int, port)
    DECLARE_PROPERTY_READ_ONLY(QString, userDataPath)

    DECLARE_PROPERTY_READ_ONLY(int, videoCount)
    DECLARE_PROPERTY_READ_ONLY(bool, getSecondLevelComments)
    DECLARE_PROPERTY_READ_ONLY(int, firstLevelCommentsCount)
    DECLARE_PROPERTY_READ_ONLY(int, secondLevelCommentsCount)

    DECLARE_PROPERTY_READ_ONLY(QString, commentStartTime)
    DECLARE_PROPERTY_READ_ONLY(QString, commentEndTime)

    DECLARE_PROPERTY_READ_ONLY(QStringList, messageDataModel)


};

#endif // SYSTEMSETTINGS_H
