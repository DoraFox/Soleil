#include "layoutmanager.h"


#define MAKE_UI(ui, layout, screen) m_uiArray[ui] = MakeUi(layout, screen);


LayoutManager::LayoutManager(QQmlApplicationEngine *ptrEngine, QObject *parent)
    : QObject{parent}
    , m_engine(ptrEngine)
    , m_mainWindow(nullptr)
    , m_layoutArray{}
    , m_uiArray{}
    , m_mainURL(qEnvironmentVariable("MAIN_QML", GET_QML_PATH("main.qml")))
{
    DEBUGPREFIX << m_mainURL << qEnvironmentVariable("MAIN_QML");
}

QQuickItem *LayoutManager::getLayout(int layoutLevel)
{
    if(layoutLevel > LayoutCout - 1 || layoutLevel < 0)
    {
        return nullptr;
    }

    return m_layoutArray[layoutLevel];
}

void LayoutManager::loadMainQML()
{
    QObject::connect(m_engine, &QQmlApplicationEngine::objectCreated,
                     this, &LayoutManager::onQMLObjectCreated,
                     Qt::QueuedConnection);

    m_engine->load(m_mainURL);
}

QQuickWindow *LayoutManager::getMainWindow() const
{
    return m_mainWindow;
}

void LayoutManager::initLayout()
{
    createUI<UI::MainWindow>();
    //createUI<UI::LoginWindow>();
    createUI<UI::System>();
    createUI<UI::InfoMessageBox>();

    emit finishInit();
}

void LayoutManager::deleteAllLayout()
{
    deleteLayout<MainWindow>();
    deleteLayout<System>();
    //deleteLayout<Task>();
}

void LayoutManager::onQMLObjectCreated(QObject *obj, const QUrl &objUrl)
{
    if (!obj && m_mainURL == objUrl)
    {
        QCoreApplication::exit(-1);
    }
    else
    {
        QList<QObject*> rootObjects = m_engine->rootObjects();
        QObject* mainObject = rootObjects.first();
        m_mainWindow = qobject_cast<QQuickWindow*>(mainObject);
        emit qmlMainWindowCreated();
    }
}

QQuickItem *LayoutManager::createComponent(const QString &componentUrl)
{
    QQuickItem* item = nullptr;

    do{
        bool flag = false;

        QQmlComponent component(m_engine, QUrl(componentUrl));

        flag = component.isReady();

        if(!flag)
        {
            DEBUGPREFIX << component.errorString() << '\n' << componentUrl << '\n' << component.status();
            break;
        }

        QObject* btnObj = component.create();

        if(!btnObj) { break; }

        item = qobject_cast<QQuickItem*>(btnObj);

    }while(0);

    return item;
}

QQuickItem *LayoutManager::createBaseLayout(const QString &componentUrl)
{
    QQuickItem* layoutItem = createComponent(componentUrl);

    if(!m_mainWindow) { return nullptr; }

    QQuickItem* mainItem = m_mainWindow->contentItem();

    layoutItem->setParent(mainItem);

    layoutItem->setParentItem(mainItem);

    DEBUGPREFIX //<< layoutItem->dynamicPropertyNames() << '\n'
                //<< layoutItem->property("testInt") << '\n'
                //<< layoutItem->width() << layoutItem->height() << '\n'
                //<< layoutItem->parentItem() << '\n'
                //<< layoutItem->parent() << '\n'
                //<< layoutItem << '\n'
                << componentUrl
                ;

    return layoutItem;
}

QQuickItem *LayoutManager::createBaseScreen(const UI name, const QString &componentUrl)
{
    QQuickItem* screenItem = nullptr;

    do{
        if(nullptr != m_uiArray[name]){
            DEBUGPREFIX << "base screen exist:" << QMetaEnum::fromType<UI>().valueToKey(name);
            break;
        }

        screenItem = createComponent(componentUrl);

        if(!screenItem){
            DEBUGPREFIX_ERROR << "fail to create:" << QMetaEnum::fromType<UI>().valueToKey(name);
            break;
        }

        int screenLevel = name % static_cast<int>(ScreenCount);
        screenItem->setZ(screenLevel);

        m_uiArray[name] = screenItem;
        DEBUGPREFIX << "success:" << QMetaEnum::fromType<UI>().valueToKey(name);
    }while(0);

    return screenItem;
}


/*
void LayoutManager::initUIArray()
{
    MAKE_UI(UI::MainWindow,   FirstLayout,  FirstScreen);
    MAKE_UI(UI::LoginWindow,  FirstLayout,  SecondScreen);
    MAKE_UI(UI::LeftPane,     FirstLayout,  ThirdScreen);

    MAKE_UI(UI::Task, SecondLayout, FirstScreen);

    MAKE_UI(UI::ToolBar, ThirdLayout , FirstScreen);

    MAKE_UI(UI::Dialog, FourthLayout, FirstScreen);

    MAKE_UI(UI::MessageBoxBase, FifthLayout, FirstScreen);
    MAKE_UI(UI::SuccessMessageBox , FifthLayout, SecondScreen);
    MAKE_UI(UI::InfoMessageBox , FifthLayout, ThirdScreen);
    MAKE_UI(UI::WarningMessageBox , FifthLayout, FourthScreen);
    MAKE_UI(UI::ErrorMessageBox , FifthLayout, TopScreen);

    MAKE_UI(UI::System, TopLayout, FirstScreen);
    MAKE_UI(UI::Emergency, TopLayout, SecondScreen);
}
*/
