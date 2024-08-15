#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>
#include <QString>
#include <QHash>
#include <QDebug>
#include <QCoreApplication>
#include <QTranslator>
#include <QProcess>
#include <QDir>
#include <QUrl>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>

#include <xlsxdocument.h>
#include <xlsxworkbook.h>

#include "register/classregister.h"
#include "business/file/exceloperate.h"

#include <qcorotask.h>

class TranslationManager : public QObject
{
    Q_OBJECT

    using TransInfoHash = QHash<QString, QHash<int, QString>>;


public:
    typedef enum enumTranslateTitle: quint8{
        Translate_Code = 1,
        Translate_Chinese,
        Translate_English,
        Translate_ChineseTw,
        Translate_French,
        Translate_Japanese,

        Translate_Auto,

        ColumnBegin = Translate_Code + 1,
        ColumnEnd = Translate_Auto + 1,

        Unknow
    }TranslateTitle;
    Q_ENUM(enumTranslateTitle)

    explicit TranslationManager(QObject *parent = nullptr);

    Q_INVOKABLE void selectLanguage(int type);

    QCoro::Task<void> translateText(TranslateTitle from, TranslateTitle to);

    /* url: file path */
    Q_INVOKABLE void translateExcel(const QUrl& url);

    QStringList extractSourceContent(const QString &fileName);

    QCoro::Task<void> delayOperation(int milliseconds);

private:
    void initLanguage();

    void initTranslationTools();

    QString generateSign(const QString &q, const QString &salt, const QString &curtime);

    bool getTransInfoHash(const QString &excelFilePath);

    void writeTrans2Execl(const QString &excelFilePath);

    void writeToTranslationExcel(const QStringList &contents, const QString &fileName);

    void updateTsFileByExecl(const QString& excelFilePath, const QString& tsFilePath, TranslateTitle language);

    QCoro::Task<QString> translateWord(QUrl url);

private slots:

private:
    static const QString TRANSLATE_APP_KEY;
    static const QString TRANSLATE_APP_SECRET;

    QTranslator* m_translator;

    TransInfoHash m_transInfoHash;


signals:

};

#endif // TRANSLATIONMANAGER_H
