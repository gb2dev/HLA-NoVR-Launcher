
#include "launcher.h"

Launcher::Launcher(QObject *parent)
    : QObject{parent}
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

        // Download newest mod version
        auto future = QtConcurrent::run([this]() {
            nam.reset(new QNetworkAccessManager);
            nam->setTransferTimeout(10000);
            nam->setStrictTransportSecurityEnabled(true);
            nam->enableStrictTransportSecurityStore(true, QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/hsts/"));

            const QUrl availableModVersionUrl("https://raw.githubusercontent.com/bfeber/HLA-NoVR/main_menu_test/game/hlvr/scripts/vscripts/version.lua");
            QNetworkRequest availableModVersionRequest(availableModVersionUrl);

            return std::shared_ptr<QNetworkReply>(nam->get(availableModVersionRequest));
        }).then([](std::shared_ptr<QNetworkReply> networkReply) {
            qDebug() << networkReply->error();
            networkReply->deleteLater();
        });

        /*QFuture<void> future = QtConcurrent::run([]() {
            QEventLoop loop;

            QNetworkAccessManager *nam = new QNetworkAccessManager;
            const QUrl availableModVersionUrl("https://raw.githubusercontent.com/bfeber/HLA-NoVR/main_menu_test/game/hlvr/scripts/vscripts/version.lua");
            QNetworkRequest availableModVersionRequest(availableModVersionUrl);
            QNetworkReply *availableModVersionReply = nam->get(availableModVersionRequest);
            /*connect(availableModVersionReply, &QNetworkReply::downloadProgress, this, [](qint64 bytesReceived, qint64 bytesTotal) {
                qDebug() << "Downloaded" + QString::number(bytesReceived) + " of " + QString::number(bytesTotal);
            });
            connect(availableModVersionReply, &QNetworkReply::finished, this, [availableModVersionReply]() {
                availableModVersionReply->deleteLater();
                qDebug() << "Download finished";
            });
            connect(availableModVersionReply, &QNetworkReply::errorOccurred, this, [](QNetworkReply::NetworkError errorCode) {
                qDebug() << errorCode;
            });
            connect(availableModVersionReply, &QNetworkReply::sslErrors, this, [](const QList<QSslError> &errors) {
            });
            QtFuture::connect(availableModVersionReply, &QNetworkReply::finished).then([]() {
                qDebug() << "yes";
            });
            loop.exec();
        });*/


        // QNetworkReply *reply = ...
        // connect(reply, &QNetworkReply::finished, this, [reply]()) {
        //     reply->deleteLater();
        //     ...
        // });

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
