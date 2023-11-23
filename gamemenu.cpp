
#include "gamemenu.h"

#include <Windows.h>

GameMenu::GameMenu(QObject *parent)
    : QObject{parent}
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this](){
        stopRead = true;
    });
}

void GameMenu::gameStarted(QQuickWindow *w)
{
    window = w;
    w->installEventFilter(this);

    // Add WS_EX_NOACTIVATE to the default extended style
    hWnd = (HWND)window->winId();
    LONG_PTR exStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    SetWindowLongPtr(hWnd, GWL_EXSTYLE, exStyle | WS_EX_NOACTIVATE);

    targetWindow = FindWindowA("SDL_app", "Half-Life: Alyx");
    if (!targetWindow) {
        qDebug() << "window not found";
        qApp->quit();
    }

    SetForegroundWindow(targetWindow);

    QFuture<void> future = QtConcurrent::run([this]{
        bool skipBuffered = true;
        QFile file("C:/Program Files (x86)/Steam/steamapps/common/Half-Life Alyx/game/hlvr/console.log");
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        while (!stopRead) {
            while (!in.atEnd()) {
                if (skipBuffered) {
                    in.readLine();
                } else {
                    QString resultString = in.readLine();
                    if (resultString.contains("CHostStateMgr::QueueNewRequest( Loading (") || resultString.contains("CHostStateMgr::QueueNewRequest( Restoring Save (")) {
                        window->setVisible(false);
                        loadingMode = true;
                    } else {
                        QStringList result = resultString.split("[MainMenu] ");
                        if (result.size() > 1) {
                            if (result[1] == "main_menu_mode") {
                                window->setVisible(true);
                                m_pauseMenuMode = false;
                                emit pauseMenuModeChanged();
                                loadingMode = false;
                            } else if (result[1] == "pause_menu_mode") {
                                m_pauseMenuMode = true;
                                emit pauseMenuModeChanged();
                                loadingMode = false;
                            }
                        }
                    }
                }
            }
            skipBuffered = false;
        }
    });

    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &GameMenu::update);
    timer->start();

    window->show();
}

void GameMenu::update()
{
    SetWindowPos(hWnd, GetNextWindow(targetWindow, GW_HWNDPREV), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    RECT rect;
    if (GetClientRect(targetWindow, &rect)) {
        ClientToScreen(targetWindow, reinterpret_cast<POINT*>(&rect.left));
        ClientToScreen(targetWindow, reinterpret_cast<POINT*>(&rect.right));
        window->setGeometry(rect.left, rect.top, rect.right - rect.left,
                            rect.bottom - rect.top);
    } else {
        qDebug() << "GetClientRect failed";
        qApp->quit();
    }

    bool escape_current = GetKeyState(VK_ESCAPE) < 0;
    if (escape_current && !escPrevious) {
        if (m_pauseMenuMode && !loadingMode) {
            if (window->isVisible()) {
                runGameCommand("gameui_allowescape;gameui_preventescapetoshow;gameui_hide;r_drawvgui 1;unpause");
            } else {
                runGameCommand("gameui_preventescape;gameui_allowescapetoshow;gameui_activate;r_drawvgui 0;pause");
            }
            window->setVisible(!window->isVisible());
        }
    }
    escPrevious = escape_current;
}

void GameMenu::runGameScript(const QString &script)
{
    QFile file("C:/Program Files (x86)/Steam/steamapps/common/Half-Life Alyx/game/hlvr/scripts/vscripts/main_menu_exec.lua");
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << script;
    file.close();
    SendMessage(targetWindow, WM_KEYDOWN, VK_F24, 0);
    SendMessage(targetWindow, WM_KEYUP, VK_F24, 0);
}

void GameMenu::runGameCommand(const QString &command)
{
    runGameScript("SendToConsole(\"" + command + "\")");
}

void GameMenu::buttonPlayClicked()
{
    if (m_pauseMenuMode) {
        runGameCommand("gameui_allowescape;gameui_preventescapetoshow;gameui_hide;r_drawvgui 1;unpause");
        window->setVisible(false);
    } else {
        // TODO: Implement loading most recent save
        runGameCommand("load quick");
    }
}

void GameMenu::buttonOptionsClicked()
{

}

void GameMenu::buttonMainMenuClicked()
{
    runGameCommand("map startup");
}

void GameMenu::buttonQuitClicked()
{
    runGameCommand("quit");
}

bool GameMenu::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        SetForegroundWindow(targetWindow);
    }
    return false;
}
