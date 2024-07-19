#ifndef FILEOPERATE_H
#define FILEOPERATE_H

#include <QObject>
#include <QUrl>
#include <QDir>
#include <QDebug>
#include <QFileInfo>
#include <QList>

class FileOperate : public QObject
{
    Q_OBJECT
public:
    explicit FileOperate(QObject *parent = nullptr);

    /* url: dircetory path */
    Q_INVOKABLE void transformQMLConnection(const QUrl& url, bool recursive = true);

    static void getAllFiles(QStringList& pathList, const QUrl& url, const QStringList& nameFilters, bool recursive);

signals:

};

#endif // FILEOPERATE_H
