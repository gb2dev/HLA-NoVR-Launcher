
#ifndef LAUNCHER_H
#define LAUNCHER_H


#include <QDesktopServices>
#include <QDir>
#include <QObject>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QSettings>
#include <QDebug>


class Launcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool validInstallation MEMBER m_validInstallation NOTIFY validInstallationChanged)
    Q_PROPERTY(QString customLaunchOptions MEMBER m_customLaunchOptions NOTIFY customLaunchOptionsChanged)
public:
    explicit Launcher(QObject *parent = nullptr);
    QQmlApplicationEngine *engine;

public slots:
    void playGame();
    void updateMod(const QString &installLocation);

signals:
    void updateModInstalling();
    void updateModFinished();
    void errorMessage(const QString &message);
    void validInstallationChanged();
    void customLaunchOptionsChanged();

private:
    void checkValidInstallation();

    QSettings settings;
    bool m_validInstallation;
    QString m_customLaunchOptions;
};

#endif // LAUNCHER_H
