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

    // RegisterQmlComponentType_3(Torus, 1, 0);
    // RegisterQmlComponentType_3(Arc, 1, 0);
    // 使用示例
    RegisterQmlComponentType(Torus, 1, 0);                  // 3 参数
    RegisterQmlComponentType(Circle, "Custom.Shapes", 1, 0); // 4 参数
}
