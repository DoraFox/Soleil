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

#include "register/classregister.h"
#include "business/file/exceloperate.h"

#define TRANSLATE(code) {code, QObject::tr(_TRANSLATE(_T ## code))}
#define _TRANSLATE(code) #code

class TranslationManager : public QObject
{
    Q_OBJECT
public:
    typedef enum enumTranslateTitle{
        Type = 1,
        Code,
        Translate_ZH,
        Translate_EN,

        ColumnBegin = Code,
        ColumnEnd = Translate_EN + 1,

        Unknow
    }TranslateTitle;
    Q_ENUM(enumTranslateTitle)

    explicit TranslationManager(QObject *parent = nullptr);

    Q_INVOKABLE void selectLanguage(int type);

    Q_INVOKABLE QString getTranslation(int code);

    /* unused */
    Q_INVOKABLE void translateProject(const QUrl& url);

    Q_INVOKABLE bool translateProFile(QString command);

private:
    void initLanguage();

    void initialErrorCode();


private:

    QHash<int, QString> m_errorCodeHash;

    QTranslator* m_translator;

signals:

};

#endif // TRANSLATIONMANAGER_H
