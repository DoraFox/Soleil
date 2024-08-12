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

#define TRANSLATE(code) {code, QObject::tr(_TRANSLATE(_T ## code))}
#define _TRANSLATE(code) #code

class TranslationManager : public QObject
{
    Q_OBJECT

    using TransInfoHash = QHash<QString, QHash<int, QString>>;


public:
    typedef enum enumTranslateTitle: quint8{
        Code = 1,
        Translate_Chinese,
        Translate_ChineseTw,
        Translate_English,
        Translate_French,
        Translate_Japanese,

        Translate_Auto,

        ColumnBegin = Code + 1,
        ColumnEnd = Translate_Auto + 1,

        Unknow
    }TranslateTitle;
    Q_ENUM(enumTranslateTitle)

    explicit TranslationManager(QObject *parent = nullptr);

    Q_INVOKABLE void selectLanguage(int type);

    /* unused */
    Q_INVOKABLE void translateProject(const QUrl& url);

    Q_INVOKABLE bool translateProFile(QString command);

    QCoro::Task<void> translateText(TransInfoHash& hash, TranslateTitle from, TranslateTitle to);

    /* url: file path */
    Q_INVOKABLE void translateExcel(const QUrl& url);

    QStringList extractSourceContentFromFile(const QString &fileName);

private:
    void initLanguage();

    void initTranslationTools();

    QString generateSign(const QString &q, const QString &salt, const QString &curtime);

    bool getTransInfoHash(const QString &docPath);

    void writeTrans2Execl(const QUrl &url, TransInfoHash& hash);

    QCoro::Task<> translateExcel_inter(QUrl url);

    void convertExcelToTS(const QString &execlFileName, const QString &tsFileName, TranslateTitle type);

    void writeToTranslationExcel(const QStringList &contents, const QString &fileName);

private slots:

private:
    static const QString TRANSLATE_APP_KEY;
    static const QString TRANSLATE_APP_SECRET;

    QTranslator* m_translator;

    TransInfoHash m_transInfoHash;


signals:

};

#endif // TRANSLATIONMANAGER_H
