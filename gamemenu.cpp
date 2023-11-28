
#include "gamemenu.h"

#include <QGuiApplication>

#include <Windows.h>

GameMenu::GameMenu(QObject *parent)
    : QObject{parent}
{
    mainMenuExecFile.setFileName(settings.value("installLocation").toString() + "/game/hlvr/scripts/vscripts/main_menu_exec.lua");
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this](){
        stopRead = true;
        mainMenuExecFile.close();
    });
}

void GameMenu::gameStarted(QQuickWindow *w)
{
    readConvarsFile();

    window = w;

    hWnd = (HWND)window->winId();

    QFuture<void> future = QtConcurrent::run([this]{
        bool skipBuffered = true;
        bool listingAddons = false;
        QFile file(settings.value("installLocation").toString() + "/game/hlvr/console.log");
        file.resize(0);
        file.open(QIODevice::ReadOnly);
        QTextStream in(&file);
        while (!stopRead) {
            while (!in.atEnd()) {
                QString resultString = in.readLine();
                if (!skipBuffered) {
                    if (listingAddons) {
                        if (resultString.contains("default_enabled_addons_list")) {
                            listingAddons = false;
                        } else {
                            resultString = resultString.sliced(15);
                            QStringList localAddons = resultString.split("local: ");
                            QStringList subscribedAddons = resultString.split("subscribed: ");

                            // Addon info
                            resultString = localAddons.at(0);
                            resultString.remove("\t");
                            static QRegularExpression subscribedRegEx("subscribed.*$");
                            resultString.remove(subscribedRegEx);
                            QStringList info = resultString.split(" =");

                            if (info.at(0) == "mounted") {
                                addons.last().mounted = info.at(1).startsWith(" YES");
                            } else if (info.at(0) == "enabled") {
                                addons.last().enabled = info.at(1).startsWith(" YES");
                            } else if (info.at(0) == "maps") {
                                resultString = info.at(1);
                                resultString.remove(" ");
                                info = resultString.split(",");
                                addons.last().maps = info;
                            }

                            if (localAddons.size() == 2) {
                                // New local addon
                                resultString = localAddons.at(1);
                                Addon addon;
                                addon.fileName = resultString;
                                addons.append(addon);
                            } else if (subscribedAddons.size() == 2) {
                                // New subscribed addon
                                resultString = subscribedAddons.at(1);
                                Addon addon;
                                addon.fileName = resultString;
                                addons.append(addon);
                            }
                        }
                    }
                    if (resultString.contains("CHostStateMgr::QueueNewRequest( Loading (") || resultString.contains("CHostStateMgr::QueueNewRequest( Restoring Save (")) {
                        loadingMode = true;
                        gamePaused = false;
                        emit visibilityStateChanged(VisibilityState::Hidden);
                    } else {
                        // Check difficulty
                        static QRegularExpression skillRegEx("skill_hlvr(\\d).cfg");
                        QRegularExpressionMatch match = skillRegEx.match(resultString);
                        if (match.hasMatch()) {
                            emit convarLoaded("skill", match.captured(1));
                        } else {
                            QStringList result = resultString.split("[MainMenu] ");
                            if (result.size() > 1) {
                                if (result.at(1) == "main_menu_mode") {
                                    pauseMenuMode = false;
                                    loadingMode = false;
                                    window->setFlag(Qt::WindowTransparentForInput, false);
                                    emit visibilityStateChanged(VisibilityState::MainMenu);

                                    // Check if there are no save files
                                    QDir savesDirectory(settings.value("installLocation").toString() + "/game/hlvr/SAVE", "*.sav", QDir::Time, QDir::Files);
                                    if (savesDirectory.entryList().isEmpty()) {
                                        emit noSaveFilesDetected();
                                    }

                                    // Get list of addon
                                    addons.clear();
                                    runGameScript("print(\"[MainMenu] addon_list\");SendToConsole(\"addon_list\")");
                                } else if (result.at(1) == "pause_menu_mode") {
                                    pauseMenuMode = true;
                                    loadingMode = false;
                                    window->setFlag(Qt::WindowTransparentForInput, true);
                                    emit visibilityStateChanged(VisibilityState::HUD);
                                } else if (result.at(1) == "addon_list") {
                                    listingAddons = true;
                                } else {
                                    result = result.at(1).split(" ");
                                    if (result.at(0) == "player_health") {
                                        emit hudHealthChanged(hudHealth = result.at(1).toInt());
                                    }
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
            window->setFlag(Qt::WindowDoesNotAcceptFocus, true);
            SetForegroundWindow(targetWindow);
        }
    }
}

void GameMenu::runGameScript(const QString &script)
{
    QFile file(settings.value("installLocation").toString() + "/game/hlvr/scripts/vscripts/main_menu_exec.lua");
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

void GameMenu::writeToBindingsFile(const QString &key, const QVariant &value)
{
    const QString path = settings.value("installLocation").toString() + "/game/hlvr/scripts/vscripts/bindings.lua";
    QFile bindings(path);
    bindings.open(QIODevice::Text | QIODevice::ReadWrite);
    QTextStream in(&bindings);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.startsWith(key)) {
            QString valueString = "= ";

            switch (value.userType()) {
                case QMetaType::Bool:
                    valueString += value.toString();
                    break;

                case QMetaType::Int:
                    valueString += QString::number(value.toInt());
                    break;

                case QMetaType::Float:
                    valueString += QString::number(value.toFloat());
                    break;

                case QMetaType::QString:
                    valueString += "\"" + value.toString() + "\"";
                    break;
            }

            static QRegularExpression valueRegEx("=.*$");
            line.replace(valueRegEx, valueString);
        }
        lines.append(line);
    }
    bindings.resize(0);
    QTextStream out(&bindings);
    for (const QString &line : lines) {
        out << line << "\n";
    }
    bindings.close();
}

void GameMenu::readBindingsFile()
{
    const QString path = settings.value("installLocation").toString() + "/game/hlvr/scripts/vscripts/bindings.lua";
    QFile bindings(path);
    bindings.open(QIODevice::Text | QIODevice::ReadOnly);
    QTextStream in(&bindings);
    while (!in.atEnd()) {
        QString line = in.readLine().remove(" ").remove("\"");
        QStringList keyValue = line.split("=");
        emit bindingChanged(keyValue.at(0), keyValue.at(1));
    }
    bindings.close();
}

void GameMenu::readConvarsFile()
{
    const QString path = settings.value("installLocation").toString() + "/game/hlvr/cfg/machine_convars.vcfg";
    QFile convars(path);
    convars.open(QIODevice::Text | QIODevice::ReadOnly);
    QTextStream in(&convars);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        static QRegularExpression convarRegEx("\"(.*)\"[ \t]*\"(.*)\"");
        QRegularExpressionMatch match = convarRegEx.match(line);
        if (match.hasMatch()) {
            emit convarLoaded(match.captured(1), match.captured(2));
        }
    }
    convars.close();
}

void GameMenu::writeToSaveCfg(const QString &key, const QString &value)
{
    const QString path = settings.value("installLocation").toString() + "/game/hlvr/SAVE/personal.cfg";
    QFile saveCfg(path);
    saveCfg.open(QIODevice::Text | QIODevice::ReadWrite);
    QTextStream in(&saveCfg);
    QStringList lines;
    while (!in.atEnd()) {
        QString line = in.readLine();
        static QRegularExpression convarRegEx("\"(.*)\"[ \t]*\"(.*)\"");
        QRegularExpressionMatch match = convarRegEx.match(line);
        if (match.captured(1) == key) {
            line.replace(match.captured(2), value);
        }
        lines.append(line);
    }
    saveCfg.resize(0);
    QTextStream out(&saveCfg);
    for (const QString &line : lines) {
        out << line << "\n";
    }
    saveCfg.close();
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
            QDateTime dateTime = saveInfo.lastModified();
            emit saveAdded(name, dateTime.toString(), fileName);
        }
    }
}

void GameMenu::buttonNewGameClicked()
{
    if (!pauseMenuMode) {
        for (const Addon &addon : addons) {
            emit addonMapsAdded(addon.maps, addon.enabled);
        }
    }
    emit newGameSelected();
}

void GameMenu::buttonOptionsClicked()
{
    readBindingsFile();
}

void GameMenu::buttonMainMenuClicked()
{
    runGameCommand("map startup");
}

void GameMenu::buttonAddonsClicked()
{
    for (const Addon &addon : addons) {
        QString name = addon.fileName + (addon.enabled ? " (Enabled)" : " (Disabled)");
        emit addonAdded(name, addon.fileName);
    }
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

void GameMenu::newGame(const QString &mapName, bool addonMap)
{
    if (mapName == "cancel") {
        emit visibilityStateChanged(pauseMenuMode ? VisibilityState::PauseMenu : VisibilityState::MainMenu);
    } else {
        if (addonMap) {
            runGameCommand("addon_play " + mapName);
        } else {
            runGameCommand("map " + mapName);
        }
    }
}

void GameMenu::toggleAddon(const QString &fileName)
{
    if (fileName == "cancel") {
        emit visibilityStateChanged(VisibilityState::MainMenu);
    } else if (fileName == "workshop") {
        QDesktopServices::openUrl(QUrl("steam://url/SteamWorkshopPage/546560"));
    } else {
        emit addonToggled();
        for (Addon &addon : addons) {
            if (addon.fileName == fileName) {
                if (addon.enabled) {
                    runGameCommand("addon_disable " + fileName);
                } else {
                    runGameCommand("addon_enable " + fileName);
                }
                addon.enabled = !addon.enabled;
            }
            QString name = addon.fileName + (addon.enabled ? " (Enabled)" : " (Disabled)");
            emit addonAdded(name, addon.fileName);
        }
    }
}

void GameMenu::recordInput(const QString &inputName)
{
    recordInputName = inputName;
    window->installEventFilter(this);
    window->setFlag(Qt::WindowDoesNotAcceptFocus, false);
    SetFocus(hWnd);
}

void GameMenu::changeOptions(const QStringList &options)
{
    if (!options.isEmpty()) {
        QString commands;
        for (const QString &command : options) {
            commands += command + ";";

            if (command.startsWith("mouse_pitchyaw_sensitivity")) {
                writeToBindingsFile("MOUSE_SENSITIVITY", command.split(" ").at(1).toFloat());
            } else if (command.startsWith("mouse_invert_y")) {
                writeToBindingsFile("INVERT_MOUSE_Y", command.split(" ").at(1) == "true");
            } else if (command.startsWith("fov_desired")) {
                writeToBindingsFile("FOV", command.split(" ").at(1).toFloat());
            } else if (command.startsWith("skill")) {
                writeToSaveCfg("setting.skill", command.split(" ").at(1));
            }
        }

        runGameCommand(commands);
    }
}

bool GameMenu::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        QString key = keyEvent->text();
        int keyNumber = keyEvent->key();
        // TODO: Add more keys
        if (keyNumber == Qt::Key_Escape) {
            key = "ESCAPE";
        } else if (keyNumber == Qt::Key_Tab) {
            key = "TAB";
        } else if (keyNumber == Qt::Key_CapsLock) {
            key = "CAPSLOCK";
        } else if (keyNumber == Qt::Key_Shift) {
            key = "SHIFT";
        } else if (keyNumber == Qt::Key_Control) {
            key = "CTRL";
        } else if (keyNumber == Qt::Key_Meta) {
            key = "LWIN";
        } else if (keyNumber == Qt::Key_Alt) {
            key = "ALT";
        } else if (keyNumber == Qt::Key_Space) {
            key = "SPACE";
        } else if (keyNumber == Qt::Key_Menu) {
            key = "APPS";
        } else if (keyNumber == Qt::Key_Return) {
            key = "ENTER";
        } else if (keyNumber == Qt::Key_Enter) {
            key = "KP_ENTER";
        } else if (keyNumber == Qt::Key_Backspace) {
            key = "BACKSPACE";
        } else if (keyNumber == Qt::Key_Up) {
            key = "UPARROW";
        } else if (keyNumber == Qt::Key_Down) {
            key = "DOWNARROW";
        } else if (keyNumber == Qt::Key_Left) {
            key = "LEFTARROW";
        } else if (keyNumber == Qt::Key_Right) {
            key = "RIGHTARROW";
        } else if (keyNumber == Qt::Key_Delete) {
            key = "DEL";
        } else if (keyNumber >= Qt::Key_F1 && keyNumber <= Qt::Key_F23) {
            key = "F" + QString::number(keyNumber - 0x0100002F);
        } else {
            if (key.isEmpty()) {
                key = "UNSUPPORTED";
            }
        }
        key = key.toUpper();
        emit bindingChanged(recordInputName, key);
        writeToBindingsFile(recordInputName, key);

        window->removeEventFilter(this);
        window->setFlag(Qt::WindowDoesNotAcceptFocus, true);
        SetFocus(targetWindow);
    } else if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        int button = mouseEvent->button();
        if (button == Qt::MiddleButton) {
            button = 3;
        } else if (button == Qt::BackButton) {
            button = 4;
        } else if (button == Qt::ForwardButton) {
            button = 5;
        }
        QString buttonString = "MOUSE" + QString::number(button);
        emit bindingChanged(recordInputName, buttonString);
        writeToBindingsFile(recordInputName, buttonString);

        window->removeEventFilter(this);
        window->setFlag(Qt::WindowDoesNotAcceptFocus, true);
        SetFocus(targetWindow);
    }
    return false;
}
