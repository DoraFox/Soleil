#include "mainwindowcontrol.h"

MainWindowControl::MainWindowControl(QObject *parent)
    : QObject{parent}
    , m_mainWindow(nullptr)
{
}

void MainWindowControl::setFlags(Qt::WindowFlags flags)
{
    Q_CHECK_PTR(m_mainWindow);

    m_mainWindow->setFlags(flags);
}

void MainWindowControl::setFlag(Qt::WindowType flag, bool on)
{
    Q_CHECK_PTR(m_mainWindow);

    m_mainWindow->setFlag(flag, on);

}
