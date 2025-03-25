#ifndef COMMENTMODEL_H
#define COMMENTMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QJsonObject>
#include <QSet>
#include <QHash>

#include "register/classregister.h"
#include <midware/tools/dandelion.h>

class SQLiteManager;

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
        bool deleted;
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
        HadSentMessageRole,
        DeletedRole
    };

    Q_ENUM(CommentRoles)

    explicit CommentModel(QObject *parent = nullptr);

    // 获取数据行数
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    // 角色名称映射
    QHash<int, QByteArray> roleNames() const override;

    // 获取数据
    QVariant data(const QModelIndex &index, int role) const override;

    // 设置数据
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // 添加评论数据
    void append(const QJsonObject &json);

    void appendCurrentAwemeComment(CommentItem comment);

    Q_INVOKABLE void setCurrentAwemeComment(QString awemeId);

    void loadCommentsFromDatabase(const QSet<QString> &awemesId);

    Q_INVOKABLE void deleteCurrentAwemeComment();

public slots:
    void onDeletedAweme(QString awemeId);

private:
    QVector<CommentItem> m_comments;
    QSet<QString> m_commentsId;
    QHash<QString, QVector<CommentItem>> m_awemeComments;
    QString m_currentAwemeId;

    SQLiteManager* m_sqlManager;
};

#endif // COMMENTMODEL_H
