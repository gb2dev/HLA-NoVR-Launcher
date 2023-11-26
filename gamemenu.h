
#ifndef GAMEMENU_H
#define GAMEMENU_H


#include <QDesktopServices>
#include <QtConcurrent>
#include <QObject>
#include <QQuickWindow>
#include <QSettings>
#include <QDebug>

struct Addon {
    QString fileName;
    bool mounted;
    bool enabled;
    QStringList maps;
};

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
    void buttonLoadGameClicked();
    void buttonNewGameClicked();
    void buttonOptionsClicked();
    void buttonMainMenuClicked();
    void buttonAddonsClicked();
    void buttonQuitClicked();
    void loadSave(const QString &fileName);
    void newGame(const QString &mapName, bool addonMap);
    void toggleAddon(const QString &fileName);
    void recordInput(const QString &input);

signals:
    void pauseMenuModeChanged(bool pauseMenuMode);
    void hudHealthChanged(int hudHealth);
    void visibilityStateChanged(GameMenu::VisibilityState v);
    void saveAdded(const QString &name, const QString &timeDate, const QString &fileName);
    void newGameSelected();
    void noSaveFilesDetected();
    void addonAdded(const QString &name, const QString &fileName);
    void addonMapsAdded(const QStringList &maps, bool addonEnabled);
    void addonToggled();
    void inputRecorded(const QString &inputName, const QString &bind);

private:
    bool eventFilter(QObject *object, QEvent *event) override;
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
    QList<Addon> addons;
    QString recordInputName = "";
};

#endif // LAUNCHER_H
