#ifndef LAYOUTMANAGER_H
#define LAYOUTMANAGER_H

#include <QObject>
#include <midware/define/pathdefine.h>

#include "cplusplus/midware/define/pathdefine.h"

#include <QDebug>
#include <QPair>
#include <QUrl>
#include <QMetaEnum>

#include <QQmlApplicationEngine>
#include <QQmlComponent>
#include <QQuickItem>
#include <QQuickWindow>

namespace LayoutSpace {
    template<typename T>
    struct identity{
        typedef T type;
    };

} // namespace LayoutSpace

class LayoutManager : public QObject
{
    Q_OBJECT

public:
    typedef enum enumLayoutLevel{
        FirstLayout,
        SecondLayout,
        ThirdLayout,
        FourthLayout,
        FifthLayout,
        TopLayout,

        LayoutCout,
    }LayoutLevel;

    typedef enum enumScreenLevel{
        FirstScreen,
        SecondScreen,
        ThirdScreen,
        FourthScreen,
        FifthScreen,
        TopScreen,

        ScreenCount,
    }ScreenLevel;

    typedef enum enumUI{
        MainWindow = ((int)FirstLayout * (int)ScreenCount),
        LoginWindow, LeftPane,

        Task = ((int)SecondLayout * (int)ScreenCount),

        ToolBar = ((int)ThirdLayout * (int)ScreenCount),

        Dialog = ((int)FourthScreen * (int)ScreenCount),

        MessageBoxBase = ((int)FifthLayout * (int)ScreenCount),
        SuccessMessageBox, InfoMessageBox, WarningMessageBox, ErrorMessageBox,

        System = ((int)TopLayout * (int)ScreenCount),
        SystemSetting, Emergency,

        UICount = ((int)LayoutCout * (int)ScreenCount),
        Undefined = 0xFF
    }UI;
    Q_ENUM(enumUI)


    explicit LayoutManager(QQmlApplicationEngine* engine, QObject *parent = nullptr);

    template<UI enumKey>
    QQuickItem* createUI()
    {
        QQuickItem* layoutItem = createLayout<enumKey>();

        if(nullptr == layoutItem)
        {
            DEBUGPREFIX << "fail to create layout:" << QMetaEnum::fromType<UI>().valueToKey(enumKey);
            return nullptr;
        }

        QQuickItem* uiItem = createScreen<enumKey>(layoutItem);
        if(uiItem)
        {
            DEBUGPREFIX << "createScreen:" << QMetaEnum::fromType<UI>().valueToKey(enumKey);
        }
        else
        {
            DEBUGPREFIX_ERROR << "fail to createScreen:" << QMetaEnum::fromType<UI>().valueToKey(enumKey);
        }

        return uiItem;
    }

    inline bool deleteUI(int index)
    {
        QQuickItem* uiItem = m_uiArray[index];
        if(nullptr == uiItem)
        {
            return false;
        }
        DEBUGPREFIX << "delete uiItem:" << index;
        m_uiArray[index] = nullptr;
        uiItem->deleteLater();
        return true;
    }

    template<UI enumKey>
    bool deleteLayout()
    {
        constexpr int layoutLevel = enumKey / static_cast<int>(ScreenCount);

        QQuickItem* layoutItem = m_layoutArray[layoutLevel];

        if(nullptr == layoutItem)
        {
            DEBUGPREFIX << "layoutItem is nullptr:" << layoutLevel;
            return false;
        }
        DEBUGPREFIX << "delete layoutItem:" << layoutLevel;
        constexpr int uiBegin = layoutLevel * ScreenCount;
        constexpr int uiEnd = uiBegin + ScreenCount;
        for(int i = uiBegin; i < uiEnd; ++i)
        {
            deleteUI(i);
        }
        m_layoutArray[layoutLevel] = nullptr;
        layoutItem->deleteLater();
        return true;
    }

    QQuickItem* getLayout(int layoutLevel);

    QQuickWindow* getMainWindow() const;

public slots:
    void loadMainQML();

    void initLayout();

    void deleteAllLayout();


private slots:
    void onQMLObjectCreated(QObject *obj, const QUrl &objUrl);

    /*
    template<typename T>
    T CallMid()
    {
        int iiii = 9;
        DEBUGPREFIX << iiii;
        return TCallType(LayoutSpace::identity<T>());
    }

    template<typename T>
    T TCallType(LayoutSpace::identity<T>())
    {
        int iiii = 789;
        DEBUGPREFIX << iiii;
        T t;
        return t;
    }

    void TCallType(LayoutSpace::identity<void>())
    {

    }

    void TCallType(LayoutSpace::identity<int16_t>())
    {

    }
    */

private:

    QQuickItem* createComponent(const QString& componentUrl);

    QQuickItem* createBaseLayout(const QString& componentUrl = QString(GET_QML_PATH("Layout/LayoutBase.qml")));

    QQuickItem* createBaseScreen(const UI name, const QString& componentUrl);

    template<UI enumKey>
    QQuickItem* createLayout()
    {
        QQuickItem* layoutItem = nullptr;

        do{
            constexpr int layoutLevel = enumKey / static_cast<int>(ScreenCount);
            UI layoutUi = static_cast<UI>(layoutLevel * ScreenCount);
            if(nullptr != m_layoutArray[layoutUi])
            {
                layoutItem = m_layoutArray[layoutUi];
                break;
            }

            layoutItem = createBaseLayout();

            if(!layoutItem){ break; }

            layoutItem->setZ(layoutLevel);

            m_layoutArray[layoutLevel] = layoutItem;

            DEBUGPREFIX << "UI:" << QMetaEnum::fromType<UI>().valueToKey(enumKey) << '\n'
                        << layoutItem;

        }while(0);

        return layoutItem;
    }

    template<UI enumKey>
    QQuickItem* createScreen(QQuickItem* /*ptrLayout*/)
    {
        DEBUGPREFIX << "empty function!";
        return nullptr;
    }

/*
    void initUIArray();

    constexpr int MakeUi(int layoutLevel, int screenLevel) const noexcept
    {
        return layoutLevel * 10 + screenLevel;
    }
*/


signals:
    void qmlMainWindowCreated();

    void finishInit();

private:

    QQmlApplicationEngine* m_engine;

    QQuickWindow* m_mainWindow;

    QQuickItem* m_layoutArray[LayoutCout];

    QQuickItem* m_uiArray[UICount];

    QUrl m_mainURL;

};


template<>
inline QQuickItem* LayoutManager::createScreen<LayoutManager::MainWindow>(QQuickItem* ptrLayout)
{
    QQuickItem* screenItem = createBaseScreen(LayoutManager::MainWindow, GET_QML_PATH("Layout/MainWindow.qml"));
    if(screenItem)
    {
        screenItem->setParent(ptrLayout);
        screenItem->setParentItem(ptrLayout);
    }
    return screenItem;
}

template<>
inline QQuickItem* LayoutManager::createScreen<LayoutManager::LoginWindow>(QQuickItem* ptrLayout)
{
    QQuickItem* screenItem = createBaseScreen(LayoutManager::LoginWindow, GET_QML_PATH("Screen/LoginWindow.qml"));
    if(screenItem)
    {
        screenItem->setParent(ptrLayout);
        screenItem->setParentItem(ptrLayout);
    }
    return screenItem;
}

template<>
inline QQuickItem* LayoutManager::createScreen<LayoutManager::System>(QQuickItem* ptrLayout)
{
    QQuickItem* screenItem = createBaseScreen(LayoutManager::System, GET_QML_PATH("Layout/TopLayout.qml"));
    if(screenItem)
    {
        screenItem->setParent(ptrLayout);
        screenItem->setParentItem(ptrLayout);
    }
    return screenItem;
}

template<>
inline QQuickItem* LayoutManager::createScreen<LayoutManager::InfoMessageBox>(QQuickItem* ptrLayout)
{
    QQuickItem* screenItem = createBaseScreen(LayoutManager::InfoMessageBox, GET_QML_PATH("Layout/InfoMessageBox.qml"));
    if(screenItem)
    {
        screenItem->setVisible(false);
        screenItem->setParent(ptrLayout);
        screenItem->setParentItem(ptrLayout);
    }
    return screenItem;
}





#endif // LAYOUTMANAGER_H
