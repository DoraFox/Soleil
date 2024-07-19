#include "enhancedqmlapplicationengine.h"

#include <midware/define/basedefine.h>

EnhancedQmlApplicationEngine::EnhancedQmlApplicationEngine(QObject *parent)
    : QQmlApplicationEngine{parent}
{
    this->addImportPath(QStringLiteral(_STR(QML_PATH)));


}

void EnhancedQmlApplicationEngine::clearCache()
{
    this->clearComponentCache();
}
