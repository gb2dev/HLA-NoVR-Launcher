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

        connect(reply, &QNetworkReply::finished, [=]() {
            if (reply->error()) {
                qDebug() << "Download error: " << reply->errorString();
                reply->deleteLater();
                return;
            }

            QFile file(destinationFile);
            if (!file.open(QIODevice::WriteOnly)) {
                qDebug() << "Failed to open file for writing: " << destinationFile;
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

private:
    QNetworkAccessManager *manager;
};
