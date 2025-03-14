#ifndef COMMENTMODEL_H
#define COMMENTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QJsonObject>

#include "register/classregister.h"
#include <midware/tools/dandelion.h>

class CommentModel : public QAbstractListModel {
    Q_OBJECT
public:
    struct CommentItem {
        QString commentId;
        QString createTime;
        QString ipLocation;
        QString awemeId;
        QString content;
        QString userId;
        QString secUid;
        QString shortUserId;
        QString userUniqueId;
        QString userSignature;
        QString nickname;
        QString avatar;
        int subCommentCount;
        int likeCount;
        QString parentCommentId;
        QString pictures;
        bool hadSentMessage;
    };


    enum CommentRoles {
        CommentIdRole = Qt::UserRole + 1,
        CreateTimeRole,
        IpLocationRole,
        AwemeIdRole,
        ContentRole,
        UserIdRole,
        SecUidRole,
        ShortUserIdRole,
        UserUniqueIdRole,
        UserSignatureRole,
        NicknameRole,
        AvatarRole,
        SubCommentCountRole,
        LikeCountRole,
        ParentCommentIdRole,
        PicturesRole,
        HadSentMessageRole
    };

    explicit CommentModel(QObject *parent = nullptr) : QAbstractListModel(parent) {}

    // 获取数据行数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        Q_UNUSED(parent);
        return m_comments.size();
    }

    // 角色名称映射
    QHash<int, QByteArray> roleNames() const override {
        static const QHash<int, QByteArray> roles = {
            {CommentIdRole, "comment_id"},
            {CreateTimeRole, "create_time"},
            {IpLocationRole, "ip_location"},
            {AwemeIdRole, "aweme_id"},
            {ContentRole, "content"},
            {UserIdRole, "user_id"},
            {SecUidRole, "sec_uid"},
            {ShortUserIdRole, "short_user_id"},
            {UserUniqueIdRole, "user_unique_id"},
            {UserSignatureRole, "user_signature"},
            {NicknameRole, "nickname"},
            {AvatarRole, "avatar"},
            {SubCommentCountRole, "sub_comment_count"},
            {LikeCountRole, "like_count"},
            {ParentCommentIdRole, "parent_comment_id"},
            {PicturesRole, "pictures"},
            {HadSentMessageRole,"hadSentMessage"}
        };
        return roles;
    }

    // 获取数据
    QVariant data(const QModelIndex &index, int role) const override {
        if (!index.isValid() || index.row() >= m_comments.size())
            return QVariant();

        const CommentItem &comment = m_comments.at(index.row());

        switch (role) {
        case CommentIdRole:
            return comment.commentId;
        case CreateTimeRole:
            return comment.createTime;
        case IpLocationRole:
            return comment.ipLocation;
        case AwemeIdRole:
            return comment.awemeId;
        case ContentRole:
            return comment.content;
        case UserIdRole:
            return comment.userId;
        case SecUidRole:
            return comment.secUid;
        case ShortUserIdRole:
            return comment.shortUserId;
        case UserUniqueIdRole:
            return comment.userUniqueId;
        case UserSignatureRole:
            return comment.userSignature;
        case NicknameRole:
            return comment.nickname;
        case AvatarRole:
            return comment.avatar;
        case SubCommentCountRole:
            return comment.subCommentCount;
        case LikeCountRole:
            return comment.likeCount;
        case ParentCommentIdRole:
            return comment.parentCommentId;
        case PicturesRole:
            return comment.pictures;
        case HadSentMessageRole:
            return comment.hadSentMessage;
        }
        return QVariant();
    }

    // 添加评论数据
    void append(const QJsonObject &json) {
        static Dandelion* tool = ClassRegister::GetInstance()->dandelion();

        CommentItem comment;
        comment.commentId = json.value("comment_id").toString();
        comment.createTime =  tool->timestampToDateTime(json["create_time"].toDouble());
        comment.ipLocation = json.value("ip_location").toString();
        comment.awemeId = json.value("aweme_id").toString();
        comment.content = json.value("content").toString();
        comment.userId = json.value("user_id").toString();
        comment.secUid = json.value("sec_uid").toString();
        comment.shortUserId = json.value("short_user_id").toString();
        comment.userUniqueId = json.value("user_unique_id").toString();
        comment.userSignature = json.value("user_signature").toString();
        comment.nickname = json.value("nickname").toString();
        comment.avatar = json.value("avatar").toString();
        comment.subCommentCount = json.value("sub_comment_count").toInt();
        comment.likeCount = json.value("like_count").toInt();
        comment.parentCommentId = json.value("parent_comment_id").toString();
        comment.hadSentMessage = false;

        m_awemeComments[comment.awemeId].append(comment);

        // 如果当前显示的 aweme_id 正好是这个，则更新数据
        if (comment.awemeId == m_currentAwemeId) {
            appendCurrentAwemeComment(std::move(comment));
        }
    }

    void appendCurrentAwemeComment(CommentItem&& comment)
    {
        beginInsertRows(QModelIndex(), m_comments.size(), m_comments.size());
        m_comments.append(std::move(comment));
        endInsertRows();
    }

    Q_INVOKABLE void setCurrentAwemeComment(QString awemeId)
    {
        DEBUGPREFIX << awemeId << m_currentAwemeId;

        if (m_currentAwemeId == awemeId) return;

        beginResetModel();
        m_currentAwemeId = awemeId;
        m_comments = m_awemeComments[awemeId];
        endResetModel();
    }

private:
    QVector<CommentItem> m_comments;

    QHash<QString, QVector<CommentItem>> m_awemeComments;

    QString m_currentAwemeId;
};


#endif // COMMENTMODEL_H
