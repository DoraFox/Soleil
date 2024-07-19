#ifndef MAINWINDOWCONTROL_H
#define MAINWINDOWCONTROL_H

#include <QObject>

#include <QQuickWindow>

#include <midware/define/basedefine.h>

#include <QDebug>

class MainWindowControl : public QObject
{
    Q_OBJECT
public:
    explicit MainWindowControl(QObject *parent);


    Q_INVOKABLE void setFlags(Qt::WindowFlags flags);

    Q_INVOKABLE void setFlag(Qt::WindowType flag, bool on = true);

    Q_INVOKABLE bool getFlagSatus(Qt::WindowType flag)
    {
        Q_CHECK_PTR(m_mainWindow);
        return flag & m_mainWindow->flags();
    }

signals:

private:

    DECLARE_PROPERTY(QQuickWindow*, mainWindow)

};

#endif // MAINWINDOWCONTROL_H
