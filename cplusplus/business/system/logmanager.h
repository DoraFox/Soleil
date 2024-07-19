#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>

class LogManager : public QObject
{
    Q_OBJECT
public:
    explicit LogManager(QObject *parent = nullptr);

signals:

};

#endif // LOGMANAGER_H
