
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

private:
    QQuickWindow *window;
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
};

#endif // LAUNCHER_H
