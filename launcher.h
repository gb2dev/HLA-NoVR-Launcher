
#ifndef LAUNCHER_H
#define LAUNCHER_H


#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QGuiApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QRegularExpression>
#include <QScreen>
#include <QSettings>
#include <QtConcurrent>
#include <QDebug>

#include "networkhandler.h"


class Launcher : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool validInstallation MEMBER m_validInstallation NOTIFY validInstallationChanged)
    Q_PROPERTY(QString customLaunchOptions MEMBER m_customLaunchOptions NOTIFY customLaunchOptionsChanged)
    Q_PROPERTY(QString modBranch MEMBER m_modBranch NOTIFY modBranchChanged)
public:
    explicit Launcher(QObject *parent = nullptr);
    QQmlApplicationEngine *engine;

public slots:
    void playGame();
    void updateMod(const QString &installLocation = "");

signals:
    void updateModInstalling();
    void errorMessage(const QString &message);
    void validInstallationChanged();
    void customLaunchOptionsChanged();
    void modBranchChanged();
    void installationSelectionNeeded();

private:
    void checkValidInstallation();
    const QString readModVersionInfo(const QString &path);

    QThread networkThread;
    NetworkHandler *networkHandler;
    QSettings settings;
    bool m_validInstallation;
    QString m_customLaunchOptions;
    QString m_modBranch;
};

#endif // LAUNCHER_H
