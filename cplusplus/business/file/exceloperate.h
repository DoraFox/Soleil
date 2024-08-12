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

public:
    explicit ExcelOperate(QObject *parent = nullptr);

signals:

private:

};

#endif // EXCELOPERATE_H
