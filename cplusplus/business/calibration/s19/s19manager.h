#ifndef S19MANAGER_H
#define S19MANAGER_H

#include <QObject>

class S19Manager : public QObject
{
    Q_OBJECT
public:
    explicit S19Manager(QObject *parent = nullptr);

    Q_INVOKABLE void startSwapNumbeUpdate();
    Q_INVOKABLE void startSwapNumbeUpdate(const QUrl &url, QVector<int> address, QVector<int> value);

signals:

private slots:
    void s19OptFinished();
};

#endif // S19MANAGER_H
