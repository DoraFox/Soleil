#ifndef TRANSLATIONMANAGER_H
#define TRANSLATIONMANAGER_H

#include <QObject>
#include <QHash>
#include <QUrl>

#include <qcorotask.h>

class QTranslator;

class TranslationManager : public QObject
{
    Q_OBJECT

    using TransInfoHash = QHash<QString, QHash<int, QString>>;

public:
    typedef enum enumTranslateTitle: quint8{
        ColumnBegin,

        Translate_Code,
        Translate_Chinese,
        Translate_English,
        Translate_ChineseTw,
        Translate_French,
        Translate_Japanese,

        Translate_Auto,

        ColumnEnd,

        RowBegin = 2,

        Unknow
    }TranslateTitle;
    Q_ENUM(enumTranslateTitle)

    explicit TranslationManager(QObject *parent = nullptr);

    Q_INVOKABLE void selectLanguage(int type);

    /* 
        translate the word in m_transInfoHash from language "from" to "to".
        see more detail in url: https://ai.youdao.com/console/#/service-singleton/text-translation ,
                                https://fanyi.youdao.com/openapi/
     */
    QCoro::Task<bool> translateText(TranslateTitle from, TranslateTitle to);

    /* url: file path */
    Q_INVOKABLE void translateExcel(const QUrl& url);

    QStringList extractSourceContent(const QString &fileName);

    QStringList extractSourceContent(const QString &fileName, const QString& filter);

private:
    void initLanguage();

    void initTranslationTools();

    QString generateSign(const QString &q, const QString &salt, const QString &curtime);

    bool getTransInfoHash(const QString &excelFilePath);

    /* populate the Code Column in execl by codes */
    void writeCodes2Execl(const QStringList& codes, const QString &excelFilePath);

    /* populate the All translation Column in execl by m_transInfoHash */
    void writeTrans2Execl(const QString &excelFilePath);

    void updateTsFileByExecl(const QString& excelFilePath, const QString& tsFilePath, TranslateTitle language);

    /* access the url and get the translated word (the return value QString) */
    QCoro::Task<QString> getTranslationByUrl(QUrl url);

private slots:

private:
    /* the app key in youdao text translation service API */
    static const QString TRANSLATE_APP_KEY;
    /* the app secret in youdao text translation service API */
    static const QString TRANSLATE_APP_SECRET;

    QTranslator* m_translator;

    TransInfoHash m_transInfoHash;


signals:

};

#endif // TRANSLATIONMANAGER_H
