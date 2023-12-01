
#include <QFont>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QAudioDevice>
#include <QMediaDevices>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "gamemenu.h"
#include "launcher.h"

int main(int argc, char *argv[])
{
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

        QAudioDevice info(QMediaDevices::defaultAudioOutput());
        rootContext->setContextProperty("audioWarning", info.maximumChannelCount() > 2);

        GameMenu *gameMenu = new GameMenu(&app);
        rootContext->setContextProperty("gameMenu", gameMenu);

        url = QUrl(u"qrc:/HLA-NoVR-Launcher/Launcher.qml"_qs);
    } else {
        QNetworkAccessManager *networkManager = new QNetworkAccessManager(&app);
        QNetworkRequest request(QUrl("https://api.github.com/repos/bfeber/HLA-NoVR-Launcher/releases/latest"));
        QNetworkReply *reply = networkManager->get(request);

        QObject::connect(reply, &QNetworkReply::finished, qApp, [reply]() {
            if (reply->error()) {
                QDesktopServices::openUrl(QUrl("update.bat"));
                qApp->quit();
                return;
            }

            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            if (doc.isNull()) {
                QDesktopServices::openUrl(QUrl("update.bat"));
                qApp->quit();
                return;
            } else {
                if (VERSION != doc.object().value("tag_name").toString()) {
                    QThread networkThread;
                    NetworkHandler *networkHandler = new NetworkHandler;
                    networkHandler->moveToThread(&networkThread);
                    QObject::connect(qApp, &QCoreApplication::aboutToQuit, qApp, [networkHandler](){
                        networkHandler->deleteLater();
                    });
                    networkThread.start();

                    QObject::connect(networkHandler, &NetworkHandler::returnNetworkReply, networkHandler, [](QNetworkReply *reply) {
                        QObject::connect(reply, &QNetworkReply::finished, reply, [reply]() {
                            if (reply->error()) {
                                reply->deleteLater();
                                return;
                            }

                            QFile file("HLA-NoVR-Launcher.zip");
                            if (!file.open(QIODevice::WriteOnly)) {
                                reply->deleteLater();
                                return;
                            }

                            file.write(reply->readAll());
                            file.close();
                            reply->deleteLater();

                            QProcess *unzip = new QProcess;
                            unzip->setProgram("7za");
                            unzip->setArguments({"x", "HLA-NoVR-Launcher.zip", "-aoa", "-oUpdate"});
                            unzip->start();

                            QObject::connect(unzip, &QProcess::finished, [=](int exitCode, QProcess::ExitStatus exitStatus = QProcess::NormalExit) {
                                QFile("HLA-NoVR-Launcher.zip").remove();
                                QDesktopServices::openUrl(QUrl("update.bat"));
                                qApp->quit();
                                return;
                            });
                        });
                    }, Qt::SingleShotConnection);
                    QMetaObject::invokeMethod(networkHandler, [networkHandler]() {
                        const QUrl url("https://github.com/bfeber/HLA-NoVR-Launcher/releases/latest/download/HLA-NoVR-Launcher.zip");
                        networkHandler->createNetworkReply(url);
                    });
                } else {
                    QDesktopServices::openUrl(QUrl("update.bat"));
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
