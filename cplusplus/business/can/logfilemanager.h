#ifndef LOGFILEMANAGER_H
#define LOGFILEMANAGER_H

#include <QObject>

#include <midware/define/basedefine.h>

#include "logfileopt.h"

class logFileManager : public QObject
{
    Q_OBJECT

public:
    explicit logFileManager(QObject *parent = nullptr);


signals:


private:
    LogFileOpt* m_logFileOpt;

};

#endif // LOGFILEMANAGER_H
