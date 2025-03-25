#ifndef SQLITEMANAGER_H
#define SQLITEMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDebug>

#include "business/crawler/awememodel.h"
#include "business/crawler/commentmodel.h"


class SQLiteManager : public QObject {
    Q_OBJECT
public:
    explicit SQLiteManager(QObject *parent = nullptr);
    ~SQLiteManager();

    bool openDatabase(const QString &dbName);
    void closeDatabase();
    bool createTables();

    // 视频操作
    bool addVideo(const AwemeModel::AwemeItem &video);
    bool videoExists(const QString &aweme_id);
    bool softDeleteVideo(const QString &aweme_id);

    void loadVideoFromDatabase(QSet<QString>& awemesId, QVector<AwemeModel::AwemeItem>& awemes);

    // 评论操作
    bool addComment(const CommentModel::CommentItem &comment);
    bool commentExists(const QString &commentId);
    bool softDeleteComment(const QString &commentId);
    bool softDeleteCommentByAwemeId(const QString &awemeId);

    QVector<CommentModel::CommentItem> getCommentsByVideo(const QString &awemeId);

    void loadCommentsFromDatabase(const QSet<QString> &awemesId,
                                  QSet<QString> &commentsId,
                                  QHash<QString, QVector<CommentModel::CommentItem>> &awemeComments);

    bool setHasSendMessage(const QString& commentId, bool status);
private:
    QSqlDatabase db;
    QSqlQuery query;
};

#endif // SQLITEMANAGER_H
