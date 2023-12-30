
#include "launcher.h"

Launcher::Launcher(QObject *parent)
    : QObject{parent}
{
    networkHandler = new NetworkHandler;
    networkHandler->moveToThread(&networkThread);
    connect(qApp, &QCoreApplication::aboutToQuit, this, [this](){
        networkHandler->deleteLater();
    });
    networkThread.start();

    QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
    int screenWidth = screenGeometry.width();
    int screenHeight = screenGeometry.height();
    if (QString(qgetenv("XDG_CURRENT_DESKTOP")) == "gamescope") {
        screenWidth--;
    }

    m_customLaunchOptions = settings.value("customLaunchOptions", "-console -vconsole -vsync -w " + QString::number(screenWidth) + " -h " + QString::number(screenHeight)).toString();
    connect(this, &Launcher::customLaunchOptionsChanged, this, [this] {
        settings.setValue("customLaunchOptions", m_customLaunchOptions);
    });

    m_modBranch = settings.value("modBranch", "main").toString();
    connect(this, &Launcher::modBranchChanged, this, [this] {
        settings.setValue("modBranch", m_modBranch);
    });
}

void Launcher::playGame() // Launches the game with the arguments
{
    m_customLaunchOptions.remove("-fullscreen");
    QDesktopServices::openUrl(QUrl("steam://run/546560// -novr +vr_enable_fake_vr 1 -condebug +hlvr_main_menu_delay 999999 +hlvr_main_menu_delay_with_intro 999999 +hlvr_main_menu_delay_with_intro_and_saves 999999 " + m_customLaunchOptions + " -window"));
    engine->load(QUrl(u"qrc:/HLA-NoVR-Launcher/GameMenu.qml"_qs));
    networkHandler->deleteLater();
    engine->rootObjects().at(0)->deleteLater();
    deleteLater();
}

void Launcher::updateMod(const QString &installLocation) // Takes the install location, downloads the new file, extracts everything so it is installed correctly
{
    if (!installLocation.isEmpty()) {
        settings.setValue("installLocation", QUrl(installLocation).toLocalFile());
    }

    checkValidInstallation();

    if (m_validInstallation) {
        if (qApp->arguments().contains("-nomodupdate")) {
            playGame();
            return;
        }

        // Read installed mod version
        const QString installedModVersionInfoPath = settings.value("installLocation").toString() + "/game/hlvr/scripts/vscripts/version.lua";
        QFile installedModVersionInfoFile(installedModVersionInfoPath);
        installedModVersionInfoFile.open(QIODevice::Text | QIODevice::ReadWrite);
        QTextStream in(&installedModVersionInfoFile);
        const QString installedModVersionInfo = readModVersionInfo(in.readAll());
        installedModVersionInfoFile.close();

        // Read newest mod version info
        connect(networkHandler, &NetworkHandler::returnNetworkReply, networkHandler, [this, installedModVersionInfo](QNetworkReply *versionInfoReply) {
            connect(versionInfoReply, &QNetworkReply::finished, versionInfoReply, [this, versionInfoReply, installedModVersionInfo]() {
                versionInfoReply->deleteLater();

                QString newestModVersion = readModVersionInfo(versionInfoReply->readAll());
                if (versionInfoReply->error() || newestModVersion == installedModVersionInfo) {
                    QMetaObject::invokeMethod(this, [this]() {
                        playGame();
                    });
                } else {
                    // Download newest mod
                    connect(networkHandler, &NetworkHandler::returnNetworkReply, networkHandler, [this](QNetworkReply *modReply) {
                        connect(modReply, &QNetworkReply::finished, modReply, [this, modReply]() {
                            if (modReply->error()) {
                                emit errorMessage("Download error: " + modReply->errorString());
                                modReply->deleteLater();
                                return;
                            }

                            QFile file(m_modBranch + ".zip");
                            if (!file.open(QIODevice::WriteOnly)) {
                                emit errorMessage("Failed to open file for writing");
                                modReply->deleteLater();
                                return;
                            }

                            file.write(modReply->readAll());
                            file.close();
                            modReply->deleteLater();

                            // Install newest mod
                            QMetaObject::invokeMethod(this, [this]() {
                                emit updateModInstalling();
                                QProcess *unzip = new QProcess(this);
#ifdef Q_OS_WIN
                                unzip->setProgram("7za");
                                unzip->setArguments({"x", m_modBranch + ".zip", "-y"});
#else
                                unzip->setProgram("unzip");
                                unzip->setArguments({"-o", m_modBranch + ".zip"});
#endif

                                connect(unzip, &QProcess::finished, unzip, [this](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                                    QProcess *move = new QProcess(this);
#ifdef Q_OS_WIN
                                    move->setProgram("robocopy");
                                    move->setArguments({"HLA-NoVR-" + m_modBranch, settings.value("installLocation").toString(), "/S", "/IS"});
#else
                                    move->setProgram("rsync");
                                    move->setArguments({"-a", "HLA-NoVR-" + m_modBranch + "/game", settings.value("installLocation").toString()});
#endif
                                    move->start();
                                    connect(move, &QProcess::finished, move, [this](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                                        QDir("HLA-NoVR-" + m_modBranch).removeRecursively();
                                        QFile(m_modBranch + ".zip").remove();

                                        playGame();
                                    });
                                });
                                connect(unzip, &QProcess::errorOccurred, unzip, [this](QProcess::ProcessError processError) {
                                    emit errorMessage("An error occured while unpacking (" + QString::number(processError) + ")");
                                    return;
                                });
                                unzip->start();
                            });
                        });
                    }, Qt::SingleShotConnection);
                    QMetaObject::invokeMethod(networkHandler, [this]() {
                        const QUrl newestModUrl("https://github.com/bfeber/HLA-NoVR/archive/refs/heads/" + m_modBranch +".zip");
                        networkHandler->createNetworkReply(newestModUrl);
                    });
                }
            });
        }, Qt::SingleShotConnection);
        QMetaObject::invokeMethod(networkHandler, [this]() {
            const QUrl newestModVersionInfoUrl("https://raw.githubusercontent.com/bfeber/HLA-NoVR/" + m_modBranch + "/game/hlvr/scripts/vscripts/version.lua");
            networkHandler->createNetworkReply(newestModVersionInfoUrl);
        });
    } else if (installLocation.isEmpty()) {
        emit installationSelectionNeeded();
    }
}

void Launcher::checkValidInstallation() // Checks for a valid installation by checking if the hlvr.exe exists and if the install location is set correctly
{
    // Set default install location
    if (settings.value("installLocation").userType() != QMetaType::QString) {
        settings.setValue("installLocation", "C:/Program Files (x86)/Steam/steamapps/common/Half-Life Alyx");
    }

    m_validInstallation = QFile(settings.value("installLocation").toString() + "/game/bin/win64/hlvr.exe").exists();

    // Reset install location if invalid
    if (!m_validInstallation) {
        settings.remove("installLocation");
    }

    emit validInstallationChanged();
}

const QString Launcher::readModVersionInfo(const QString &content)
{
    static QRegularExpression dateTimeRegex("[a-zA-Z]{3} \\d{2} \\d{2}:\\d{2}");
    QRegularExpressionMatch match = dateTimeRegex.match(content);
    if (match.hasMatch()) {
        return match.captured(0);
    }
    return QString();
}
