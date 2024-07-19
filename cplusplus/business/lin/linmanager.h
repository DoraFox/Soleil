#ifndef LINMANAGER_H
#define LINMANAGER_H

#include <QObject>

class LinManager : public QObject
{
    Q_OBJECT
public:
    explicit LinManager(QObject *parent = nullptr);

    Q_INVOKABLE QStringList initLinHardware();

signals:
};

#endif // LINMANAGER_H
