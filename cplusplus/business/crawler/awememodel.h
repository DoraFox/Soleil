#ifndef AWEMEMODEL_H
#define AWEMEMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QJsonObject>

#include <QAbstractListModel>
#include <QVector>

#include "register/classregister.h"


#include <midware/tools/dandelion.h>

class SQLiteManager;

class AwemeModel : public QAbstractListModel {
    Q_OBJECT
public:

    struct AwemeItem {
        QString aweme_id;
        int aweme_type;
        QString title;
        QString create_time;
        QString user_id;
        QString sec_uid;
        QString short_user_id;
        QString user_unique_id;
        QString user_signature;
        QString nickname;
        QString avatar;
        int liked_count;
        int collected_count;
        int comment_count;
        int share_count;
        QString ip_location;
        QString aweme_url;
        bool deleted;
    };

    enum AwemeRoles {
        AwemeIdRole = Qt::UserRole + 1,
        AwemeTypeRole,
        TitleRole,
        CreateTimeRole,
        UserIdRole,
        SecUidRole,
        ShortUserIdRole,
        UserUniqueIdRole,
        UserSignatureRole,
        NicknameRole,
        AvatarRole,
        LikedCountRole,
        CollectedCountRole,
        CommentCountRole,
        ShareCountRole,
        IpLocationRole,
        AwemeUrlRole,
        DeletedRole
    };

    explicit AwemeModel(QObject *parent = nullptr);

    void init();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QHash<int, QByteArray> roleNames() const override;

    QVariant data(const QModelIndex &index, int role) const override;

    void append(const QJsonObject &json);

    const QSet<QString>& getAwemesId() const
    {
        return m_awemesId;
    }

    Q_INVOKABLE bool removeRow(int row);


signals:
    void deletedAweme(QString awemeId);

private:
    QSet<QString> m_awemesId;
    QVector<AwemeItem> m_awemes;  // 存储 AwemeItem 数据

    SQLiteManager* m_sqlManager;
};



#endif // AWEMEMODEL_H
