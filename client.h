#ifndef FCLIENT_H
#define FCLIENT_H
#include <QtCore/QtCore>
#include "serverinterface.h"
#include "forms/authenticationform.h"
#include "forms/chatform.h"

class Client : public ServerInterface
{
    Q_OBJECT
private:
    Form *currentForm = nullptr;
public:
    explicit Client(ServerInterface *parent = nullptr);

    void start();
signals:
public slots:

    void login(QString username, QString password);
    void loginCallback(QNetworkReply *response);

    void logout();
    void logoutCallback(QNetworkReply *response);


    void signup(QString username, QString password, QString firstname = nullptr, QString lastname = nullptr);
    void signupCallback(QNetworkReply *response);

};

#endif // FCLIENT_H
