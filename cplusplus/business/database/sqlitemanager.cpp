#include "SQLiteManager.h"
#include <midware/define/pathdefine.h>

// 构造函数
SQLiteManager::SQLiteManager(QObject *parent) : QObject(parent) {

    QCoreApplication::addLibraryPath(SQL_DRIVER_DIR);

    openDatabase(SQLITE_DB_PATH);

}

// 析构函数
SQLiteManager::~SQLiteManager() {
    closeDatabase();
}

// 打开数据库
bool SQLiteManager::openDatabase(const QString &dbName)
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dbName);

    if (!db.open()) {
        qDebug() << "Failed to open database:" << db.lastError().text();
        return false;
    }
    query = QSqlQuery(db);
    return createTables();
}

// 关闭数据库
void SQLiteManager::closeDatabase()
{
    if (db.isOpen()) {
        db.close();
    }
}

// 创建表
bool SQLiteManager::createTables()
{
    // 视频表
    QString createVideosTable =
        "CREATE TABLE IF NOT EXISTS videos ("
        "aweme_id TEXT PRIMARY KEY, "
        "aweme_type INTEGER, "
        "title TEXT, "
        "create_time TEXT, "
        "user_id TEXT, "
        "sec_uid TEXT, "
        "short_user_id TEXT, "
        "user_unique_id TEXT, "
        "user_signature TEXT, "
        "nickname TEXT, "
        "avatar TEXT, "
        "liked_count INTEGER, "
        "collected_count INTEGER, "
        "comment_count INTEGER, "
        "share_count INTEGER, "
        "ip_location TEXT, "
        "aweme_url TEXT, "
        "deleted INTEGER DEFAULT 0"
        ");";

    if (!query.exec(createVideosTable)) {
        qDebug() << "Failed to create videos table:" << query.lastError().text();
        return false;
    }

    // 评论表
    QString createCommentsTable =
        "CREATE TABLE IF NOT EXISTS comments ("
        "commentId TEXT PRIMARY KEY, "
        "awemeId TEXT, "
        "createTime TEXT, "
        "ipLocation TEXT, "
        "content TEXT, "
        "userId TEXT, "
        "secUid TEXT, "
        "shortUserId TEXT, "
        "userUniqueId TEXT, "
        "userSignature TEXT, "
        "nickname TEXT, "
        "avatar TEXT, "
        "subCommentCount INTEGER, "
        "likeCount INTEGER, "
        "parentCommentId TEXT, "
        "pictures TEXT, "
        "hadSentMessage INTEGER, "
        "deleted INTEGER DEFAULT 0, "
        "FOREIGN KEY(awemeId) REFERENCES videos(aweme_id)"
        ");";

    if (!query.exec(createCommentsTable)) {
        qDebug() << "Failed to create comments table:" << query.lastError().text();
        return false;
    }

    return true;
}

bool SQLiteManager::addVideo(const AwemeModel::AwemeItem &video)
{
    QSqlQuery query;
    query.prepare("INSERT INTO videos (aweme_id, aweme_type, title, create_time, user_id, sec_uid, "
                  "short_user_id, user_unique_id, user_signature, nickname, avatar, liked_count, collected_count, "
                  "comment_count, share_count, ip_location, aweme_url, deleted) "
                  "VALUES (:aweme_id, :aweme_type, :title, :create_time, :user_id, :sec_uid, "
                  ":short_user_id, :user_unique_id, :user_signature, :nickname, :avatar, :liked_count, "
                  ":collected_count, :comment_count, :share_count, :ip_location, :aweme_url, :deleted)");

    query.bindValue(":aweme_id", video.aweme_id);
    query.bindValue(":aweme_type", video.aweme_type);
    query.bindValue(":title", video.title);
    query.bindValue(":create_time", video.create_time);
    query.bindValue(":user_id", video.user_id);
    query.bindValue(":sec_uid", video.sec_uid);
    query.bindValue(":short_user_id", video.short_user_id);
    query.bindValue(":user_unique_id", video.user_unique_id);
    query.bindValue(":user_signature", video.user_signature);
    query.bindValue(":nickname", video.nickname);
    query.bindValue(":avatar", video.avatar);
    query.bindValue(":liked_count", video.liked_count);
    query.bindValue(":collected_count", video.collected_count);
    query.bindValue(":comment_count", video.comment_count);
    query.bindValue(":share_count", video.share_count);
    query.bindValue(":ip_location", video.ip_location);
    query.bindValue(":aweme_url", video.aweme_url);
    query.bindValue(":deleted", video.deleted);

    if (!query.exec()) {
        DEBUGPREFIX << "添加视频失败：" << query.lastError().text();
        return false;
    }

    return true;
}

// 检查视频是否存在
bool SQLiteManager::videoExists(const QString &aweme_id)
{
    query.prepare("SELECT COUNT(*) FROM videos WHERE aweme_id = :aweme_id AND deleted = 0");
    query.bindValue(":aweme_id", aweme_id);
    return query.exec() && query.next() && query.value(0).toInt() > 0;
}

// 伪删除视频
bool SQLiteManager::softDeleteVideo(const QString &aweme_id)
{
    query.prepare("UPDATE videos SET deleted = 1 WHERE aweme_id = :aweme_id");
    query.bindValue(":aweme_id", aweme_id);
    return query.exec();
}

void SQLiteManager::loadVideoFromDatabase(QSet<QString> &awemesId, QVector<AwemeModel::AwemeItem> &awemes)
{
    if (!query.exec("SELECT aweme_id, aweme_type, title, create_time, user_id, sec_uid, "
                    "short_user_id, user_unique_id, user_signature, nickname, avatar, "
                    "liked_count, collected_count, comment_count, share_count, "
                    "ip_location, aweme_url FROM videos where deleted = 0")) {
        qDebug() << "查询失败：" << query.lastError().text();
        return;
    }

    while (query.next()) {
        AwemeModel::AwemeItem aweme;
        aweme.aweme_id = query.value(0).toString();
        aweme.aweme_type = query.value(1).toInt();
        aweme.title = query.value(2).toString();
        aweme.create_time = query.value(3).toString();
        aweme.user_id = query.value(4).toString();
        aweme.sec_uid = query.value(5).toString();
        aweme.short_user_id = query.value(6).toString();
        aweme.user_unique_id = query.value(7).toString();
        aweme.user_signature = query.value(8).toString();
        aweme.nickname = query.value(9).toString();
        aweme.avatar = query.value(10).toString();
        aweme.liked_count = query.value(11).toInt();
        aweme.collected_count = query.value(12).toInt();
        aweme.comment_count = query.value(13).toInt();
        aweme.share_count = query.value(14).toInt();
        aweme.ip_location = query.value(15).toString();
        aweme.aweme_url = query.value(16).toString();

        awemesId.insert(aweme.aweme_id);
        awemes.append(std::move(aweme));
    }
}

// 添加评论
bool SQLiteManager::addComment(const CommentModel::CommentItem &comment)
{
    QSqlQuery query;
    query.prepare("INSERT INTO comments (commentId, createTime, ipLocation, awemeId, content, userId, "
                  "secUid, shortUserId, userUniqueId, userSignature, nickname, avatar, subCommentCount, "
                  "likeCount, parentCommentId, pictures, hadSentMessage, deleted) "
                  "VALUES (:commentId, :createTime, :ipLocation, :awemeId, :content, :userId, "
                  ":secUid, :shortUserId, :userUniqueId, :userSignature, :nickname, :avatar, :subCommentCount, "
                  ":likeCount, :parentCommentId, :pictures, :hadSentMessage, :deleted)");

    query.bindValue(":commentId", comment.commentId);
    query.bindValue(":createTime", comment.createTime);
    query.bindValue(":ipLocation", comment.ipLocation);
    query.bindValue(":awemeId", comment.awemeId);
    query.bindValue(":content", comment.content);
    query.bindValue(":userId", comment.userId);
    query.bindValue(":secUid", comment.secUid);
    query.bindValue(":shortUserId", comment.shortUserId);
    query.bindValue(":userUniqueId", comment.userUniqueId);
    query.bindValue(":userSignature", comment.userSignature);
    query.bindValue(":nickname", comment.nickname);
    query.bindValue(":avatar", comment.avatar);
    query.bindValue(":subCommentCount", comment.subCommentCount);
    query.bindValue(":likeCount", comment.likeCount);
    query.bindValue(":parentCommentId", comment.parentCommentId);
    query.bindValue(":pictures", comment.pictures);
    query.bindValue(":hadSentMessage", comment.hadSentMessage);
    query.bindValue(":deleted", comment.deleted);

    if (!query.exec()) {
        qDebug() << "添加评论失败：" << query.lastError().text();
        return false;
    }

    return true;
}


bool SQLiteManager::commentExists(const QString &commentId)
{
    query.prepare("SELECT COUNT(*) FROM comments WHERE comment_id = :comment_id");
    query.bindValue(":comment_id", commentId);

    if (!query.exec()) {
        qDebug() << "查询 commentExists 失败：" << query.lastError().text();
        return false;
    }

    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

bool SQLiteManager::softDeleteComment(const QString &commentId)
{
    query.prepare("UPDATE comments SET deleted = 1 WHERE comment_id = :comment_id");
    query.bindValue(":comment_id", commentId);

    return query.exec();
}

bool SQLiteManager::softDeleteCommentByAwemeId(const QString &awemeId)
{
    query.prepare("UPDATE comments SET deleted = 1 WHERE awemeId = :awemeId");
    query.bindValue(":awemeId", awemeId);

    if (!query.exec()) {
        qDebug() << "软删除评论失败：" << query.lastError().text();
        return false;
    }

    return true;
}

QVector<CommentModel::CommentItem> SQLiteManager::getCommentsByVideo(const QString &awemeId) {
    QVector<CommentModel::CommentItem> comments;

    QSqlQuery query;
    query.prepare("SELECT commentId, createTime, ipLocation, awemeId, content, userId, secUid, shortUserId, "
                  "userUniqueId, userSignature, nickname, avatar, subCommentCount, likeCount, "
                  "parentCommentId, pictures, hadSentMessage, deleted "
                  "FROM comments WHERE awemeId = :awemeId AND deleted = 0 ORDER BY createTime DESC");
    query.bindValue(":awemeId", awemeId);

    if (!query.exec()) {
        qDebug() << "查询 getCommentsByVideo 失败：" << query.lastError().text();
        return comments;  // 返回空的 QVector
    }

    while (query.next()) {
        CommentModel::CommentItem comment;
        comment.commentId = query.value("commentId").toString();
        comment.createTime = query.value("createTime").toString();
        comment.ipLocation = query.value("ipLocation").toString();
        comment.awemeId = query.value("awemeId").toString();
        comment.content = query.value("content").toString();
        comment.userId = query.value("userId").toString();
        comment.secUid = query.value("secUid").toString();
        comment.shortUserId = query.value("shortUserId").toString();
        comment.userUniqueId = query.value("userUniqueId").toString();
        comment.userSignature = query.value("userSignature").toString();
        comment.nickname = query.value("nickname").toString();
        comment.avatar = query.value("avatar").toString();
        comment.subCommentCount = query.value("subCommentCount").toInt();
        comment.likeCount = query.value("likeCount").toInt();
        comment.parentCommentId = query.value("parentCommentId").toString();
        comment.pictures = query.value("pictures").toString();
        comment.hadSentMessage = query.value("hadSentMessage").toBool();
        comment.deleted = query.value("deleted").toBool();

        comments.append(comment);
    }

    return comments;
}


void SQLiteManager::loadCommentsFromDatabase(const QSet<QString>& awemesId,
                                             QSet<QString> &commentsId,
                                             QHash<QString, QVector<CommentModel::CommentItem> > &awemeComments)
{
    QSqlQuery query;
    query.prepare("SELECT commentId, createTime, ipLocation, awemeId, content, userId, secUid, shortUserId, "
                  "userUniqueId, userSignature, nickname, avatar, subCommentCount, likeCount, "
                  "parentCommentId, pictures, hadSentMessage, deleted "
                  "FROM comments WHERE awemeId = :awemeId AND deleted = 0 ORDER BY createTime DESC");

    for(const QString& id: awemesId)
    {
        query.bindValue(":awemeId", id);

        if (!query.exec()) {
            qDebug() << "查询 loadCommentsByVideo 失败：" << query.lastError().text();
            continue;
        }

        QVector<CommentModel::CommentItem> comments;
        while (query.next()) {
            CommentModel::CommentItem comment;
            comment.commentId = query.value("commentId").toString();
            comment.createTime = query.value("createTime").toString();
            comment.ipLocation = query.value("ipLocation").toString();
            comment.awemeId = query.value("awemeId").toString();
            comment.content = query.value("content").toString();
            comment.userId = query.value("userId").toString();
            comment.secUid = query.value("secUid").toString();
            comment.shortUserId = query.value("shortUserId").toString();
            comment.userUniqueId = query.value("userUniqueId").toString();
            comment.userSignature = query.value("userSignature").toString();
            comment.nickname = query.value("nickname").toString();
            comment.avatar = query.value("avatar").toString();
            comment.subCommentCount = query.value("subCommentCount").toInt();
            comment.likeCount = query.value("likeCount").toInt();
            comment.parentCommentId = query.value("parentCommentId").toString();
            comment.pictures = query.value("pictures").toString();
            comment.hadSentMessage = query.value("hadSentMessage").toBool();
            comment.deleted = query.value("deleted").toBool();

            // 填充 QSet，存储评论 ID
            commentsId.insert(comment.commentId);
            // 填充 QHash，按 awemeId 分类存储
            comments.append(std::move(comment));
        }

        if (!comments.isEmpty()) {
            awemeComments.insert(id, comments);
        }
    }

}

bool SQLiteManager::setHasSendMessage(const QString &commentId, bool status)
{
    query.prepare("UPDATE comments SET hadSentMessage = :status WHERE commentId = :commentId");
    query.bindValue(":status", status ? 1 : 0);  // 传入的 status 控制更新的值
    query.bindValue(":commentId", commentId);

    if (!query.exec()) {
        DEBUGPREFIX << "更新 hadSentMessage 失败:" << query.lastError().text();
        return false;
    }
    return true;
}
