#ifndef AWEMEMODEL_H
#define AWEMEMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QJsonObject>

#include <QAbstractListModel>
#include <QVector>

#include "register/classregister.h"
#include <midware/tools/dandelion.h>

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
        AwemeUrlRole
    };

    explicit AwemeModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent)
        return m_awemes.count();
    }

    QHash<int, QByteArray> roleNames() const override {
        static const QHash<int, QByteArray> roles{
            {AwemeIdRole, "aweme_id"},
            {AwemeTypeRole, "aweme_type"},
            {TitleRole, "title"},
            {CreateTimeRole, "create_time"},
            {UserIdRole, "user_id"},
            {SecUidRole, "sec_uid"},
            {ShortUserIdRole, "short_user_id"},
            {UserUniqueIdRole, "user_unique_id"},
            {UserSignatureRole, "user_signature"},
            {NicknameRole, "nickname"},
            {AvatarRole, "avatar"},
            {LikedCountRole, "liked_count"},
            {CollectedCountRole, "collected_count"},
            {CommentCountRole, "comment_count"},
            {ShareCountRole, "share_count"},
            {IpLocationRole, "ip_location"},
            {AwemeUrlRole, "aweme_url"}
        };

        return roles;
    }


    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_awemes.count())
            return QVariant();

        const AwemeItem &aweme = m_awemes.at(index.row());

        switch (role) {
        case AwemeIdRole:
            return aweme.aweme_id;
        case AwemeTypeRole:
            return aweme.aweme_type;
        case TitleRole:
            return aweme.title;
        case CreateTimeRole:
            return aweme.create_time;
        case UserIdRole:
            return aweme.user_id;
        case SecUidRole:
            return aweme.sec_uid;
        case ShortUserIdRole:
            return aweme.short_user_id;
        case UserUniqueIdRole:
            return aweme.user_unique_id;
        case UserSignatureRole:
            return aweme.user_signature;
        case NicknameRole:
            return aweme.nickname;
        case AvatarRole:
            return aweme.avatar;
        case LikedCountRole:
            return aweme.liked_count;
        case CollectedCountRole:
            return aweme.collected_count;
        case CommentCountRole:
            return aweme.comment_count;
        case ShareCountRole:
            return aweme.share_count;
        case IpLocationRole:
            return aweme.ip_location;
        case AwemeUrlRole:
            return aweme.aweme_url;
        default:
            return QVariant();
        }
    }

    void append(const QJsonObject &json) {
        static Dandelion* tool = ClassRegister::GetInstance()->dandelion();

        AwemeItem aweme;
        aweme.aweme_id = json["aweme_id"].toString();
        aweme.aweme_type = json["aweme_type"].toInt();
        aweme.title = json["title"].toString();
        aweme.create_time = tool->timestampToDateTime(json["create_time"].toDouble());
        aweme.user_id = json["user_id"].toString();
        aweme.sec_uid = json["sec_uid"].toString();
        aweme.short_user_id = json["short_user_id"].toString();
        aweme.user_unique_id = json["user_unique_id"].toString();
        aweme.user_signature = json["user_signature"].toString();
        aweme.nickname = json["nickname"].toString();
        aweme.avatar = json["avatar"].toString();
        aweme.liked_count = json["liked_count"].toInt();
        aweme.collected_count = json["collected_count"].toInt();
        aweme.comment_count = json["comment_count"].toInt();
        aweme.share_count = json["share_count"].toInt();
        aweme.ip_location = json["ip_location"].toString();
        aweme.aweme_url = json["aweme_url"].toString();

        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_awemes.append(std::move(aweme));
        endInsertRows();
    }

private:
    QVector<AwemeItem> m_awemes;  // 存储 AwemeItem 数据
};



#endif // AWEMEMODEL_H
