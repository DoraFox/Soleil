#ifndef COMPONENTREGISTER_H
#define COMPONENTREGISTER_H

#include <QObject>

class QQmlContext;

class ComponentRegister : public QObject
{
    Q_OBJECT
public:
    Q_DISABLE_COPY(ComponentRegister)

    explicit ComponentRegister(QObject *parent = nullptr);

    static void RegistQMLComponent(QQmlContext* ptrContext = nullptr);

signals:

};

#endif // COMPONENTREGISTER_H
