
#include "launcher.h"

Launcher::Launcher(QObject *parent)
    : QObject{parent}, nam(new QNetworkAccessManager(this))
{
    m_customLaunchOptions = settings.value("customLaunchOptions", "-console -vconsole -vsync").toString();
    connect(this, &Launcher::customLaunchOptionsChanged, this, [this] {
        settings.setValue("customLaunchOptions", m_customLaunchOptions);
    });
}

void Launcher::playGame() // Launches the game with the arguments
{
    qDebug() << "Launching game";
    //QDesktopServices::openUrl(QUrl("steam://run/546560// -novr +vr_enable_fake_vr 1 -condebug +hlvr_main_menu_delay 999999 +hlvr_main_menu_delay_with_intro 999999 +hlvr_main_menu_delay_with_intro_and_saves 999999 " + m_customLaunchOptions));
    //engine->load(QUrl(u"qrc:/HLA-NoVR-Launcher/GameMenu.qml"_qs));
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

        // Read installed version
        const QString installedModVersionPath = settings.value("installLocation").toString() + "/game/hlvr/scripts/vscripts/version.lua";
        const QString installedModVersion = readModVersion(installedModVersionPath);

        // Read available mod version
        const QUrl availableModVersionUrl("https://raw.githubusercontent.com/bfeber/HLA-NoVR/main_menu_test/game/hlvr/scripts/vscripts/version.lua");

        // Download newest mod version

        // Update finished
        //playGame();

            /*QString availableModVersion = readModVersion("version.lua");
            if (installedModVersion == availableModVersion) {
                playGame();
            } else {
                // Download newest mod version
                const QUrl("https://github.com/bfeber/HLA-NoVR/archive/refs/heads/main.zip")

                    emit updateModInstalling();
                    QProcess *unzip = new QProcess(this);
                    unzip->setProgram("7za");
                    unzip->setArguments({"x", "main.zip", "-y"});

                    connect(unzip, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                        qDebug() << "crash";
                        QProcess *move = new QProcess(this);
                        move->setProgram("robocopy");
                        move->setArguments({"HLA-NoVR-main", QUrl(installLocation).toLocalFile(), "/S", "/IS"});
                        move->start();

                        connect(move, &QProcess::finished, this, [=](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                            QDir("HLA-NoVR-main").removeRecursively();
                            QFile("main.zip").remove();
                        });
                    });

                    connect(unzip, &QProcess::errorOccurred, this, [this](QProcess::ProcessError processError) {
                        emit errorMessage("An error occured while unpacking");
                        return;
                    });

                    unzip->start();
                });

                connect(downloadManager, &DownloadManager::downloadError, this, [this](const QString &message) {
                    emit errorMessage(message);
                });
            }
        });*/

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

const QString Launcher::readModVersion(const QString &path)
{
    QFile installedVersion(path);
    installedVersion.open(QIODevice::Text | QIODevice::ReadOnly);
    QTextStream in(&installedVersion);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        static QRegularExpression dateTimeRegex("[a-zA-Z]{3} \\d{2} \\d{2}:\\d{2}");
        QRegularExpressionMatch match = dateTimeRegex.match(line);
        if (match.hasMatch()) {
            return match.captured(0);
        }
    }
    installedVersion.close();
    return QString();
}
