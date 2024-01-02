
#ifndef GAMEMENU_H
#define GAMEMENU_H


#include <QDesktopServices>
#include <QtConcurrent>
#include <QObject>
#include <QQuickWindow>
#include <QSettings>
#include <QDebug>

#ifdef Q_OS_UNIX
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#undef Bool
#endif

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
    void buttonSaveGameClicked();
    void buttonNewGameClicked();
    void buttonOptionsClicked();
    void buttonMainMenuClicked();
    void buttonAddonsClicked();
    void buttonQuitClicked();
    void loadSave(const QString &fileName);
    void newGame(const QString &mapName, bool addonMap);
    void toggleAddon(const QString &fileName);
    void recordInput(const QString &input);
    void changeOptions(const QStringList &options);
    void hackFailed();
    void hackSuccess();

signals:
    void pauseMenuModeChanged(bool pauseMenuMode);
    void hudHealthChanged(int hudHealth);
    void visibilityStateChanged(GameMenu::VisibilityState v);
    void saveAdded(const QString &name, const QString &dateTime, const QString &fileName);
    void newGameSelected();
    void noSaveFilesDetected();
    void addonAdded(const QString &name, const QString &fileName);
    void addonMapsAdded(const QStringList &maps, bool addonEnabled);
    void addonToggled();
    void bindingChanged(const QString &name, const QString &bind);
    void convarLoaded(const QString &convar, const QString &value);
    void hackingPuzzleStarted(const QString &type);

private:
    bool eventFilter(QObject *object, QEvent *event) override;
    void runGameScript(const QString &script, bool focusLauncher = true);
    void runGameCommand(const QString &command, bool focusLauncher = true);
    void writeToBindingsFile(const QString &key, const QVariant &value);
    void readBindingsFile();
    void readConvarsFile();
    void writeToSaveCfg(const QString &key, const QString &value);

    QFile mainMenuExecFile;
    QSettings settings;
    QQuickWindow *window;
#ifdef Q_OS_WIN
    HWND thisWindow;
    HWND targetWindow;
#else
    Display *display;
    Window window_from_name_search(Window current, char const *needle);
    Window window_from_name(char const *name);
    Window thisWindow;
    Window targetWindow;
#endif
    bool escPrevious = false;
    QFuture<void> future;
    bool stopRead = false;
    bool stopSearchingTargetWindow = false;
    int hudHealth = 100;
    bool pauseMenuMode = false;
    bool loadingMode = false;
    bool gamePaused = false;
    QList<Addon> addons;
    QString recordInputName = "";
    bool recordingInput = false;
    int saveSlot = -1;
};

#endif // GAMEMENU_H
