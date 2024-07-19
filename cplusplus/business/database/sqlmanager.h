#ifndef SQLMANAGER_H
#define SQLMANAGER_H

#include <QObject>

class SqlManager : public QObject
{
    Q_OBJECT
public:
    explicit SqlManager(QObject *parent = nullptr);

signals:

};

#endif // SQLMANAGER_H
