
#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H


#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    DownloadManager(QObject *parent = nullptr)
        : QObject(parent)
    {
        manager = new QNetworkAccessManager(this);
    }

public slots:
    void download(const QUrl &url, const QString &destinationFile)
    {
        QNetworkRequest request(url);
        QNetworkReply *reply = manager->get(request);

        connect(reply, &QNetworkReply::finished, this, [=]() {
            if (reply->error()) {
                emit downloadError("Download error: " + reply->errorString());
                reply->deleteLater();
                return;
            }

            QFile file(destinationFile);
            if (!file.open(QIODevice::WriteOnly)) {
                emit downloadError("Failed to open file for writing: " + destinationFile);
                reply->deleteLater();
                return;
            }

            file.write(reply->readAll());
            file.close();
            reply->deleteLater();

            emit downloadFinished();
        });
    }

signals:
    void downloadFinished();
    void downloadError(const QString &message);

private:
    QNetworkAccessManager *manager;
};

#endif // DOWNLOADMANAGER_H
