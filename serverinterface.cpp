#include "serverinterface.h"


QString ServerInterface::SCHEME = "http";
int ServerInterface::PORT = 8080;
QString ServerInterface::HOST = "api.barafardayebehtar.ml";

ServerInterface::ServerInterface(QObject *parent) : QObject(parent)
{

}

void ServerInterface::setToken(QString token) {
    this->token = token;
}

QString ServerInterface::getToken() {
    return this->token;
}


QNetworkRequest ServerInterface::PrepareRequest(QString path, QUrlQuery query) {
 // request body payload
/*
    QVariantMap payload;
    payload.insert("username", "test");
    payload.insert("password", "1234");
    QByteArray jsonPayload = QJsonDocument::fromVariant(payload).toJson();
*/
    // prepare url
    QUrl url;
    url.setScheme(ServerInterface::SCHEME);
    url.setHost(ServerInterface::HOST);
    url.setPort(ServerInterface::PORT);
    url.setPath("/" + path);

    // set query
    url.setQuery(query);

    qDebug() << "Request full path is:" << url.toString();
    // send request preperations
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    // send post
    return req;
}

QJsonObject ServerInterface::GetJsonObject(QNetworkReply *response) {
   // qDebug() << "Raw JSON returned from the server is: " << strRawJSON;
    QJsonObject object;
    QString strRawJSON = response->readAll();
    QByteArray jsonData = strRawJSON.toUtf8();
    if(!jsonData.isEmpty()) {
        QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
        if(jsonDocument.isObject()) {
            object = jsonDocument.object();

        }
        else {
            qDebug() << "Json document obtained from the json response is not JSON object!";
        }
    }
    else {
        qDebug() << "Json returned from the server is empty string!";
    }
    response->deleteLater();
    return object;
}

QString ServerInterface::CheckResponse(QJsonObject responseObject) {
    // check if response code is valid
    // if not return Error message
    QString result;
    QString code = responseObject.value("code").toString(),
               message = responseObject.value("message").toString();
    qDebug() << code << ": " << message;
    switch(code.toInt()) {
        case 200:
            result = "Successful: " + message;
            break;
        case 204:
            result = "Existing Data Error: " + message;
            break;
        case 401:
            result = "Wrong Credentials Error: " + message;
            break;
        case 404:
            result = "Not Found Error: " + message;
            break;
        case 500:
            result = "Bad Request: Internal Server Side Error. Please try again later.";
            break;
        default:
            result = "Unrecognized Response Code. Please try again later.";
            break;
    }
    return result;
}

QUrlQuery ServerInterface::tokenBasedQuery() {
    QUrlQuery query;
    query.addQueryItem("token", this->token);
    return query;
}
