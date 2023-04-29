
#ifndef LAUNCHER_H
#define LAUNCHER_H


#include <QDesktopServices>
#include <QDir>
#include <QObject>
#include <QProcess>
#include <QSettings>
#include <QDebug>


class Launcher : public QObject
{
    Q_OBJECT
public:
    explicit Launcher(QObject *parent = nullptr);

public slots:
    void playGame();
    void updateMod(const QString &installLocation);

signals:
    void updateModInstalling();
    void updateModFinished();

private:
    QSettings settings;

};

#endif // LAUNCHER_H
