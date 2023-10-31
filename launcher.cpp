
#include "downloadmanager.h"
#include "launcher.h"

Launcher::Launcher(QObject *parent)
    : QObject{parent}
{
    checkValidInstallation();
}

void Launcher::editKeyBinds() // Opens the bindings.lua file
{
    QDesktopServices::openUrl(QUrl("file:///" + settings.value("installLocation").toString() + "/game/hlvr/scripts/vscripts/bindings.lua"));
}

void Launcher::playGame() // Launches the game with the arguments
{
    QDesktopServices::openUrl(QUrl("steam://run/546560// -novr +vr_enable_fake_vr 1 " + m_customLaunchOptions));
}

void Launcher::updateMod(const QString &installLocation) // Takes the install location, downloads the new file, extracts everything so it is installed correctly
{
    settings.setValue("installLocation", QUrl(installLocation).toLocalFile());
    checkValidInstallation();

    if (m_validInstallation) {
        DownloadManager *manager = new DownloadManager(this);
        manager->download(QUrl("https://github.com/bfeber/HLA-NoVR/archive/refs/heads/main.zip"), "main.zip");

        connect(manager, &DownloadManager::downloadFinished, this, [=]() {
            emit updateModInstalling();
            QProcess *unzip = new QProcess(this);
            unzip->setProgram("7za");
            unzip->setArguments({"x", "main.zip", "-y"});
            unzip->start();

            connect(unzip, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                QProcess *move = new QProcess(this);
                move->setProgram("robocopy");
                move->setArguments({"HLA-NoVR-main", QUrl(installLocation).toLocalFile(), "/S", "/IS"});
                move->start();

                connect(move, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                    QDir("HLA-NoVR-main").removeRecursively();
                    QFile("main.zip").remove();
                    emit updateModFinished();
                });
            });
        });
    }
}

void Launcher::checkValidInstallation() // Checks for a valid installation by checking if the hlvr.exe exists and if the install location is set correctly
{
    m_validInstallation = QFile(settings.value("installLocation").toString() + "/game/bin/win64/hlvr.exe").exists();
    emit validInstallationChanged();
}
