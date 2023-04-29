
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <launcher.h>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    app.setOrganizationName("HLA-NoVR");
    app.setApplicationName("Launcher");

    Launcher launcher;

    QQmlApplicationEngine engine;

    QQmlContext *rootContext = engine.rootContext();
    rootContext->setContextProperty("launcher", &launcher);
    rootContext->setContextProperty("applicationDirPath", app.applicationDirPath());

    const QUrl url(u"qrc:/HLA-NoVR-Launcher/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreationFailed,
        &app, []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
