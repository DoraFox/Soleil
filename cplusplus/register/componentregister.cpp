#include <QQmlContext>
#include <midware/define/basedefine.h>

#include "component/torus.h"
#include "component/arc.h"

#include "componentregister.h"

ComponentRegister::ComponentRegister(QObject *parent) : QObject(parent)
{

}

void ComponentRegister::RegistQMLComponent(QQmlContext *ptrContext)
{
    //Q_CHECK_PTR(ptrContext);
    Q_UNUSED(ptrContext)

    RegisterQmlComponentType(Torus, 1, 0);
    RegisterQmlComponentType(Arc, 1, 0);
}
