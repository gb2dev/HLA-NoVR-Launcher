
#include "downloadmanager.h"
#include "launcher.h"

Launcher::Launcher(QObject *parent)
    : QObject{parent}
{
    checkValidInstallation();
}

void Launcher::playGame()
{
    QDir::setCurrent(settings.value("installLocation").toString());
    QDesktopServices::openUrl(QUrl::fromLocalFile(settings.value("installLocation").toString() + "/play.bat"));
}

void Launcher::updateMod(const QString &installLocation)
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

void Launcher::checkValidInstallation()
{
    m_validInstallation = QFile(settings.value("installLocation").toString() + "/game/bin/win64/hlvr.exe").exists();
    emit validInstallationChanged();
}
