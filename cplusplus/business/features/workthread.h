#ifndef WORKTHREAD_H
#define WORKTHREAD_H

#include <QThread>

class WorkThread : public QThread
{
    Q_OBJECT
public:
    explicit WorkThread(QObject *parent = nullptr);
    void run() override;
signals:
    void resultReady(const QString &s);
};

#endif // WORKTHREAD_H
