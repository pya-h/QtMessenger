#ifndef SERVERINTERFACE_H
#define SERVERINTERFACE_H

#include <QObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QtNetwork/QtNetwork>

class ServerInterface : public QObject
{
    Q_OBJECT
public:
    explicit ServerInterface(QObject *parent = nullptr);
protected:
    const static QString SCHEME,HOST;
    const static int PORT;

    QString username, password;
    QString token;

    void setToken(QString token);
    QString getToken();
public:
    static QJsonObject GetJsonObject(QNetworkReply *response);
    static QString CheckResponse(QJsonObject responseObject);
    static QNetworkRequest PrepareRequest(QString path, QUrlQuery query);
    QUrlQuery tokenBasedQuery();
signals:

};

#endif // SERVERINTERFACE_H
