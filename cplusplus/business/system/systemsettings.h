#ifndef SYSTEMSETTINGS_H
#define SYSTEMSETTINGS_H

#include <iostream>

#include <QObject>
#include <QDebug>
#include <QVariant>

using std::cout;
using std::endl;

class SystemSettings : public QObject
{
    Q_OBJECT
public:
    explicit SystemSettings(QObject *parent = nullptr);

    void testFunc();

    /* 交换鼠标左右键 */
    Q_INVOKABLE void swapMouseButton(bool isLeft);

    Q_INVOKABLE bool isCurrentMouseLeft();

signals:
    void asySignal();

public slots:

    void asyFunction();

};

#endif // SYSTEMSETTINGS_H
