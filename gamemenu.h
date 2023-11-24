
#ifndef GAMEMENU_H
#define GAMEMENU_H


#include <QtConcurrent>
#include <QObject>
#include <QQuickWindow>
#include <QDebug>


class GameMenu : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool pauseMenuMode MEMBER m_pauseMenuMode NOTIFY pauseMenuModeChanged)
    Q_PROPERTY(int health MEMBER m_health NOTIFY healthChanged)
public:
    explicit GameMenu(QObject *parent = nullptr);

public slots:
    void gameStarted(QQuickWindow *window);
    void update();
    void buttonPlayClicked();
    void buttonOptionsClicked();
    void buttonMainMenuClicked();
    void buttonQuitClicked();

signals:
    void pauseMenuModeChanged();
    void healthChanged();

private:
    QQuickWindow *window;
    QObject *menu;
    QObject *hud;
    HWND hWnd;
    HWND targetWindow;
    bool escPrevious = false;
    QFuture<void> future;
    bool stopRead = false;
    void runGameScript(const QString &script);
    void runGameCommand(const QString &command);
    bool loadingMode = false;
    bool eventFilter(QObject *object, QEvent *event);
    bool m_pauseMenuMode = false;
    int m_health;
};

#endif // LAUNCHER_H
