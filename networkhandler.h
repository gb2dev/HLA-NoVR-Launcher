
#ifndef NETWORKHANDLER_H
#define NETWORKHANDLER_H


#include <QNetworkAccessManager>
#include <QObject>
#include <QStandardPaths>
#include <QThread>

class NetworkHandler : public QObject
{
    Q_OBJECT
public:
    NetworkHandler() {
        nam->setTransferTimeout(20000);
        nam->setStrictTransportSecurityEnabled(true);
        nam->enableStrictTransportSecurityStore(true, QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + QLatin1String("/hsts/"));
    }
    ~NetworkHandler() {
        QThread::currentThread()->quit();
    }

public slots:
    void createNetworkReply(const QUrl &url)
    {
        QNetworkRequest request(url);
        emit returnNetworkReply(nam->get(request));
    }

signals:
    void returnNetworkReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *nam = new QNetworkAccessManager(this);
};

#endif // NETWORKHANDLER_H
