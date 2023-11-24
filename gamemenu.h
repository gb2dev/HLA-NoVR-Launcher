
#ifndef GAMEMENU_H
#define GAMEMENU_H


#include <QDesktopServices>
#include <QtConcurrent>
#include <QObject>
#include <QQuickWindow>
#include <QSettings>
#include <QDebug>


class GameMenu : public QObject
{
    Q_OBJECT
public:
    explicit GameMenu(QObject *parent = nullptr);
    enum VisibilityState { Hidden, HUD, PauseMenu, MainMenu };

public slots:
    void gameStarted(QQuickWindow *window);
    void update();
    void buttonPlayClicked();
    void buttonOptionsClicked();
    void buttonMainMenuClicked();
    void buttonQuitClicked();

signals:
    void pauseMenuModeChanged(bool pauseMenuMode);
    void hudHealthChanged(int hudHealth);
    void visibilityStateChanged(GameMenu::VisibilityState v);

private:
    QSettings settings;
    QQuickWindow *window;
    HWND hWnd;
    HWND targetWindow;
    bool escPrevious = false;
    QFuture<void> future;
    bool stopRead = false;
    bool stopSearchingTargetWindow = false;
    void runGameScript(const QString &script);
    void runGameCommand(const QString &command);
    int hudHealth = 100;
    bool pauseMenuMode = false;
    bool loadingMode = false;
    bool gamePaused = false;
};

#endif // LAUNCHER_H
