
#include "gamemenu.h"

#include <QGuiApplication>

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

    hWnd = (HWND)window->winId();

    QFuture<void> future = QtConcurrent::run([this]{
        bool skipBuffered = true;
        QFile file("C:/Program Files (x86)/Steam/steamapps/common/Half-Life Alyx/game/hlvr/console.log");
        file.resize(0);
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        while (!stopRead) {
            while (!in.atEnd()) {
                if (skipBuffered) {
                    qDebug() << in.readLine();
                } else {
                    QString resultString = in.readLine();
                    if (resultString.contains("CHostStateMgr::QueueNewRequest( Loading (") || resultString.contains("CHostStateMgr::QueueNewRequest( Restoring Save (")) {
                        loadingMode = true;
                        gamePaused = false;
                        emit visibilityStateChanged(VisibilityState::Hidden);
                    } else {
                        QStringList result = resultString.split("[MainMenu] ");
                        if (result.size() > 1) {
                            if (result[1] == "main_menu_mode") {
                                pauseMenuMode = false;
                                loadingMode = false;
                                window->setFlag(Qt::WindowTransparentForInput, false);
                                emit visibilityStateChanged(VisibilityState::MainMenu);
                            } else if (result[1] == "pause_menu_mode") {
                                pauseMenuMode = true;
                                loadingMode = false;
                                window->setFlag(Qt::WindowTransparentForInput, true);
                                emit visibilityStateChanged(VisibilityState::HUD);
                            } else {
                                result = result[1].split(" ");
                                if (result[0] == "player_health") {
                                    emit hudHealthChanged(hudHealth = result[1].toInt());
                                }
                            }
                        }
                    }
                }
            }
            skipBuffered = false;
        }
    });

    QTimer* timerUpdate = new QTimer(this);
    connect(timerUpdate, &QTimer::timeout, this, &GameMenu::update);
    timerUpdate->start();

    QTimer* timerTargetWindow = new QTimer(this);
    connect(timerTargetWindow, &QTimer::timeout, this, [this]() {
        stopSearchingTargetWindow = true;
    });
    timerTargetWindow->start(60000);
}

void GameMenu::update()
{
    if (stopSearchingTargetWindow) {
        if (targetWindow) {
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
            if (escape_current && !escPrevious && GetForegroundWindow() == targetWindow) {
                if (pauseMenuMode && !loadingMode) {
                    if (gamePaused) {
                        gamePaused = false;
                        window->setFlag(Qt::WindowTransparentForInput, true);
                        runGameCommand("gameui_allowescape;gameui_preventescapetoshow;gameui_hide;r_drawvgui 1;unpause");
                        emit visibilityStateChanged(VisibilityState::HUD);
                    } else {
                        gamePaused = true;
                        window->setFlag(Qt::WindowTransparentForInput, false);
                        runGameCommand("gameui_preventescape;gameui_allowescapetoshow;gameui_activate;r_drawvgui 0;pause");
                        emit visibilityStateChanged(VisibilityState::PauseMenu);
                    }
                }
            }
            escPrevious = escape_current;
        } else {
            qApp->quit();
        }
    } else {
        targetWindow = FindWindowA("SDL_app", "Half-Life: Alyx");
        if (targetWindow) {
            stopSearchingTargetWindow = true;

            SetWindowLongPtr(hWnd, GWLP_HWNDPARENT, (LONG_PTR)targetWindow);
            window->show();
        }
    }
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
    if (pauseMenuMode) {
        if (GetForegroundWindow() == targetWindow) {
            gamePaused = false;
            window->setFlag(Qt::WindowTransparentForInput, true);
            runGameCommand("gameui_allowescape;gameui_preventescapetoshow;gameui_hide;r_drawvgui 1;unpause");
            emit visibilityStateChanged(VisibilityState::HUD);
        }
    } else {
        QDir savesDirectory(settings.value("installLocation").toString() + "/game/hlvr/SAVE", "*.sav", QDir::Time, QDir::Files);
        QString name = savesDirectory.entryList().at(0);
        name.remove(".sav");
        runGameCommand("load " + name);
    }
}

void GameMenu::buttonLoadGameClicked()
{
    QDir savesDirectory(settings.value("installLocation").toString() + "/game/hlvr/SAVE", "*.sav", QDir::Time, QDir::Files);
    QFileInfoList saveInfos = savesDirectory.entryInfoList();
    for (const QFileInfo &saveInfo : saveInfos) {
        const QString fileName = saveInfo.fileName().remove(".sav");
        QString name = fileName;
        if (!name.contains("autosavedangerous")) {
            name.replace("autosave", "Autosave");
            name.replace("quick", "Quicksave");
            name.replace("01", "");
            name.replace("02", "");
            QDateTime timeDate = saveInfo.lastModified();
            emit saveAdded(name, timeDate.toString(), fileName);
        }
    }
}

void GameMenu::buttonNewGameClicked()
{
    emit newGameSelected();
}

void GameMenu::buttonOptionsClicked()
{
    QDesktopServices::openUrl(QUrl("file:///" + settings.value("installLocation").toString() + "/game/hlvr/scripts/vscripts/bindings.lua"));
}

void GameMenu::buttonMainMenuClicked()
{
    runGameCommand("map startup");
}

void GameMenu::buttonQuitClicked()
{
    runGameCommand("quit");
}

void GameMenu::loadSave(const QString &fileName)
{
    if (fileName == "cancel") {
        emit visibilityStateChanged(pauseMenuMode ? VisibilityState::PauseMenu : VisibilityState::MainMenu);
    } else {
        runGameCommand("load " + fileName);
    }
}

void GameMenu::newGame(const QString &mapName)
{
    if (mapName == "cancel") {
        emit visibilityStateChanged(pauseMenuMode ? VisibilityState::PauseMenu : VisibilityState::MainMenu);
    } else {
        runGameCommand("map " + mapName);
    }
}
