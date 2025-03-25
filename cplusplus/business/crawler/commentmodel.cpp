#include "commentmodel.h"

#include "business/database/sqlitemanager.h"

CommentModel::CommentModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_sqlManager = ClassRegister::GetInstance()->sqliteManager();
}

int CommentModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent);
    return m_comments.size();
}

QHash<int, QByteArray> CommentModel::roleNames() const {
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
        {HadSentMessageRole, "hadSentMessage"},
        {DeletedRole, "deleted"}
    };
    return roles;
}

QVariant CommentModel::data(const QModelIndex &index, int role) const
{
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
    case DeletedRole:
        return comment.deleted;
    }
    return QVariant();
}

bool CommentModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_comments.size())
        return false;

    CommentItem &comment = m_comments[index.row()];

    switch (role) {
    case CommentIdRole:
        comment.commentId = value.toString();
        break;
    case CreateTimeRole:
        comment.createTime = value.toString();
        break;
    case IpLocationRole:
        comment.ipLocation = value.toString();
        break;
    case AwemeIdRole:
        comment.awemeId = value.toString();
        break;
    case ContentRole:
        comment.content = value.toString();
        break;
    case UserIdRole:
        comment.userId = value.toString();
        break;
    case SecUidRole:
        comment.secUid = value.toString();
        break;
    case ShortUserIdRole:
        comment.shortUserId = value.toString();
        break;
    case UserUniqueIdRole:
        comment.userUniqueId = value.toString();
        break;
    case UserSignatureRole:
        comment.userSignature = value.toString();
        break;
    case NicknameRole:
        comment.nickname = value.toString();
        break;
    case AvatarRole:
        comment.avatar = value.toString();
        break;
    case SubCommentCountRole:
        comment.subCommentCount = value.toInt();
        break;
    case LikeCountRole:
        comment.likeCount = value.toInt();
        break;
    case ParentCommentIdRole:
        comment.parentCommentId = value.toString();
        break;
    case PicturesRole:
        comment.pictures = value.toString();
        break;
    case HadSentMessageRole:
        comment.hadSentMessage = value.toBool();
        m_sqlManager->setHasSendMessage(comment.commentId, comment.hadSentMessage);
        break;
    case DeletedRole:
        comment.deleted = value.toBool();
        break;
    default:
        return false;
    }

    emit dataChanged(index, index, {role}); // 触发数据更改信号
    return true;
}

void CommentModel::append(const QJsonObject &json)
{
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
    comment.deleted = false;

    if(!m_commentsId.contains(comment.commentId))
    {
        m_sqlManager->addComment(comment);

        m_commentsId.insert(comment.commentId);
        m_awemeComments[comment.awemeId].append(comment);

        // 如果当前显示的 aweme_id 正好是这个，则更新数据
        if (comment.awemeId == m_currentAwemeId) {
            appendCurrentAwemeComment(comment);
        }
    }
}

void CommentModel::appendCurrentAwemeComment(CommentItem comment)
{
    beginInsertRows(QModelIndex(), m_comments.size(), m_comments.size());
    m_comments.append(comment);
    endInsertRows();
}

void CommentModel::setCurrentAwemeComment(QString awemeId)
{
    DEBUGPREFIX << awemeId << m_currentAwemeId;

    if (m_currentAwemeId == awemeId) return;

    beginResetModel();
    m_currentAwemeId = awemeId;
    m_comments = m_awemeComments[awemeId];
    endResetModel();
}

void CommentModel::loadCommentsFromDatabase(const QSet<QString>& awemesId)
{
    m_sqlManager->loadCommentsFromDatabase(awemesId, m_commentsId, m_awemeComments);
}

void CommentModel::deleteCurrentAwemeComment()
{
    m_awemeComments.remove(m_currentAwemeId);

    beginResetModel();
    m_comments.clear();
    endResetModel();

    m_sqlManager->softDeleteCommentByAwemeId(m_currentAwemeId);
}

void CommentModel::onDeletedAweme(QString awemeId)
{
    m_awemeComments.remove(awemeId);
}
