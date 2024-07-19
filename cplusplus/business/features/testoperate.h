#ifndef TESTOPERATE_H
#define TESTOPERATE_H

#include <QObject>

#include <midware/define/basedefine.h>

class TestOperate : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE set_enabled NOTIFY enabledChanged)
public:
    explicit TestOperate(QObject *parent = nullptr);

    Q_INVOKABLE void qmlTestFunction();

    void definedFunc();

signals:
    DECLARE_PROPERTY_SIGNAL(bool, enabled)

private:
    DECLARE_QML_PROPERTY(bool, enabled)

};

#endif // TESTOPERATE_H
