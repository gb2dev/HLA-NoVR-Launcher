
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QProcess>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "downloadmanager.h"
#include "launcher.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("HLA-NoVR");
    app.setApplicationName("Launcher");

    QQmlApplicationEngine engine;
    QUrl url;

    if (app.arguments().contains("-noupdate")) {
        Launcher *launcher = new Launcher(&app);

        QQmlContext *rootContext = engine.rootContext();
        rootContext->setContextProperty("launcher", launcher);
        rootContext->setContextProperty("applicationDirPath", app.applicationDirPath());

        url = QUrl(u"qrc:/HLA-NoVR-Launcher/Main.qml"_qs);
    } else {
        QNetworkAccessManager *networkManager = new QNetworkAccessManager(&app);
        QNetworkRequest request(QUrl("https://api.github.com/repos/bfeber/HLA-NoVR-Launcher/releases/latest"));
        QNetworkReply *reply = networkManager->get(request);

        QObject::connect(reply, &QNetworkReply::finished, [=]() {
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
                    DownloadManager *downloadManager = new DownloadManager;
                    downloadManager->download(QUrl("https://github.com/bfeber/HLA-NoVR-Launcher/releases/latest/download/HLA-NoVR-Launcher.zip"), "HLA-NoVR-Launcher.zip");

                    QObject::connect(downloadManager, &DownloadManager::downloadFinished, [=]() {
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
