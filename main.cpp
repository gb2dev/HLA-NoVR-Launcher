
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

int main(int argc, char *argv[])
{
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");

    QQuickStyle::setStyle("Fusion");

    QGuiApplication app(argc, argv);
    app.setOrganizationName("HLA-NoVR");
    app.setApplicationName("Launcher");
    app.setFont(QFont("Dinish"));

    QFontDatabase::addApplicationFont("Dinish-Regular.otf");
    QFontDatabase::addApplicationFont("Dinish-Bold.otf");
    QFontDatabase::addApplicationFont("Dinish-Italic.otf");

    QQmlApplicationEngine engine;
    QUrl url;

    if (app.arguments().contains("-noupdate")) {
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
#ifdef Q_OS_WIN
                QDesktopServices::openUrl(QUrl("update.bat"));
#else
                QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                qApp->quit();
                return;
            }

            QJsonDocument doc = QJsonDocument::fromJson(versionInfoReply->readAll());
            if (doc.isNull()) {
#ifdef Q_OS_WIN
                QDesktopServices::openUrl(QUrl("update.bat"));
#else
                QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                qApp->quit();
                return;
            } else {
                if (VERSION != doc.object().value("tag_name").toString()) {
                    QNetworkRequest launcherRequest(QUrl("https://github.com/bfeber/HLA-NoVR-Launcher/releases/latest/download/HLA-NoVR-Launcher.zip"));
                    QNetworkReply *launcherReply = networkManager->get(launcherRequest);

                    QObject::connect(launcherReply, &QNetworkReply::finished, launcherReply, [launcherReply]() {
                        QFile file("HLA-NoVR-Launcher.zip");

                        if (launcherReply->error() || !file.open(QIODevice::WriteOnly)) {
                            launcherReply->deleteLater();
                            QFile("HLA-NoVR-Launcher.zip").remove();
#ifdef Q_OS_WIN
                            QDesktopServices::openUrl(QUrl("update.bat"));
#else
                            QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                            qApp->quit();
                            return;
                        }

                        file.write(launcherReply->readAll());
                        file.close();
                        launcherReply->deleteLater();

                        QProcess *unzip = new QProcess;
#ifdef Q_OS_WIN
                        unzip->setProgram("7za");
                        unzip->setArguments({"x", "HLA-NoVR-Launcher.zip", "-aoa", "-oUpdate"});
#else
                        unzip->setProgram("unzip");
                        unzip->setArguments({"-o", "HLA-NoVR-Launcher-Linux.zip", "-d", "Update"});
#endif
                        unzip->start();

                        QObject::connect(unzip, &QProcess::finished, [=](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                            QFile("HLA-NoVR-Launcher.zip").remove();
#ifdef Q_OS_WIN
                            QDesktopServices::openUrl(QUrl("update.bat"));
#else
                            QProcess::startDetached("/bin/bash", {"update.sh"});
#endif
                            qApp->quit();
                            return;
                        });
                    });
                } else {
#ifdef Q_OS_WIN
                    QDesktopServices::openUrl(QUrl("update.bat"));
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
