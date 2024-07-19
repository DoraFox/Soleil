#ifndef EXCELOPERATE_H
#define EXCELOPERATE_H

#include <xlsxdocument.h>
#include <xlsxworkbook.h>

#include <QObject>
#include <QDebug>
#include <QRegExp>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QMetaEnum>

class ExcelOperate : public QObject
{
    Q_OBJECT

    /** code: {TranslateName : Translation} */
    using TransInfoHash = QHash<QString, QHash<int, QString>>;
    using TransInfoMap = QMap<int, QMap<int, QString>>;

public:
    explicit ExcelOperate(QObject *parent = nullptr);

    /* url: dircetory path */
    Q_INVOKABLE void convertExcelToTS(const QUrl& url);


    Q_INVOKABLE void transformCode(const QUrl& url);

    TransInfoHash getTransInfoHash(const QUrl& url) const;


signals:

private:


};

#endif // EXCELOPERATE_H
