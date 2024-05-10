
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#ifdef Q_OS_WIN
#include <QAudioDevice>
#include <QMediaDevices>
#endif
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

#include "gamemenu.h"
#include "launcher.h"

bool noUpdate = true;

void customMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    QFile outFile(noUpdate ? "log.txt" : "log_update.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);

    QTextStream textStream(&outFile);
    textStream << msg << Qt::endl;
}

int main(int argc, char *argv[])
{
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");

    QQuickStyle::setStyle("Fusion");

    QGuiApplication app(argc, argv);
    app.setOrganizationName("HLA-NoVR");
    app.setApplicationName("Launcher");
    app.setFont(QFont("Dinish"));
    app.setApplicationVersion(VERSION);

    QFontDatabase::addApplicationFont("Dinish-Regular.otf");
    QFontDatabase::addApplicationFont("Dinish-Bold.otf");
    QFontDatabase::addApplicationFont("Dinish-Italic.otf");

    QQmlApplicationEngine engine;
    QUrl url;

    noUpdate = app.arguments().contains("-noupdate");

    QFile outFile(noUpdate ? "log.txt" : "log_update.txt");
    outFile.resize(0);
    qInstallMessageHandler(customMessageHandler);

    if (noUpdate) {
        QFile("HLA-NoVR-Launcher-Installer.exe").remove();

        QQmlContext *rootContext = engine.rootContext();
        rootContext->setContextProperty("applicationDirPath", app.applicationDirPath());

        Launcher *launcher = new Launcher(&app);
        launcher->engine = &engine;
        rootContext->setContextProperty("launcher", launcher);

#ifdef Q_OS_WIN
        QAudioDevice info(QMediaDevices::defaultAudioOutput());
        rootContext->setContextProperty("audioWarning", info.maximumChannelCount() > 2);
#else
        rootContext->setContextProperty("audioWarning", false);
#endif

        GameMenu *gameMenu = new GameMenu(&app);
        rootContext->setContextProperty("gameMenu", gameMenu);

        url = QUrl(u"qrc:/HLA-NoVR-Launcher/Launcher.qml"_qs);
    } else {
        QNetworkAccessManager *networkManager = new QNetworkAccessManager(&app);
        QNetworkRequest versionInfoRequest(QUrl("https://api.github.com/repos/bfeber/HLA-NoVR-Launcher/releases/latest"));
        QNetworkReply *versionInfoReply = networkManager->get(versionInfoRequest);

        QObject::connect(versionInfoReply, &QNetworkReply::finished, qApp, [versionInfoReply, networkManager]() {
            versionInfoReply->deleteLater();

            if (versionInfoReply->error()) {
                qDebug() << "Can't find newest version";
#ifdef Q_OS_WIN
                qApp->quit();
                QProcess::startDetached(qApp->arguments()[0], qApp->arguments() << "-noupdate");
#else
                QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                qApp->quit();
                return;
            }

            QJsonDocument doc = QJsonDocument::fromJson(versionInfoReply->readAll());
            if (doc.isNull()) {
#ifdef Q_OS_WIN
                qDebug() << "Invalid newest version";
                qApp->quit();
                QProcess::startDetached(qApp->arguments()[0], qApp->arguments() << "-noupdate");
#else
                QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                qApp->quit();
                return;
            } else {
                if (VERSION != doc.object().value("tag_name").toString()) {
                    QNetworkRequest launcherRequest(QUrl("https://github.com/bfeber/HLA-NoVR-Launcher/releases/latest/download/HLA-NoVR-Launcher-Installer.exe"));
                    QNetworkReply *launcherReply = networkManager->get(launcherRequest);

                    QObject::connect(launcherReply, &QNetworkReply::finished, launcherReply, [launcherReply]() {
                        QFile file("HLA-NoVR-Launcher-Installer.exe");

                        if (launcherReply->error() || !file.open(QIODevice::WriteOnly)) {
                            qDebug() << "Invalid update file";
                            launcherReply->deleteLater();
                            QFile("HLA-NoVR-Launcher-Installer.exe").remove();
#ifdef Q_OS_WIN
                            qApp->quit();
                            QProcess::startDetached(qApp->arguments()[0], qApp->arguments() << "-noupdate");
#else
                            QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                            qApp->quit();
                            return;
                        }

                        file.write(launcherReply->readAll());
                        file.close();
                        launcherReply->deleteLater();

                        QProcess *install = new QProcess;
#ifdef Q_OS_WIN
                        qDebug() << "Installing update";
                        install->setProgram("HLA-NoVR-Launcher-Installer.exe");
                        install->setArguments({"/silent", "/norestartapplications"});
#else
                        install->setProgram("unzip");
                        install->setArguments({"-o", "HLA-NoVR-Launcher-Linux.zip", "-d", "Update"});
#endif
                        install->start();

                        QObject::connect(install, &QProcess::finished, [=](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                            qDebug() << "Update installed: " + QString::number(exitCode);
#ifdef Q_OS_WIN
#else
                            QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                            qApp->quit();
                            return;
                        });
                    });
                } else {
#ifdef Q_OS_WIN
                    qApp->quit();
                    QProcess::startDetached(qApp->arguments()[0], qApp->arguments() << "-noupdate");
#else
                    QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                    qApp->quit();
                    return;
                }
            }
        });

        url = QUrl(u"qrc:/HLA-NoVR-Launcher/Updating.qml"_qs);
    }

    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
