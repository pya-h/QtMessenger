#ifndef CLIENT_H
#define CLIENT_H
#include <QtCore/QtCore>
#include "serverinterface.h"
#include "forms/authenticationform.h"
#include "forms/chatform.h"
#include "chat/user.h"




class Client : public ServerInterface
{
    Q_OBJECT
private:

    Form *currentForm = nullptr;
    class User *me = nullptr;
    Contact *target = nullptr;
public:
    explicit Client(ServerInterface *parent = nullptr);

    void start();
signals:
public slots:

    void login(QString username, QString password);
    void loginCallback(QNetworkReply *response);

    void logout();
    void logoutCallback(QNetworkReply *response);
    Contact *getTarget() { return this->target; }
    void setTarget(Contact *nextTarget);
    void setTarget(unsigned int contactIndex);

    void signup(QString username, QString password, QString firstname = nullptr, QString lastname = nullptr);
    void signupCallback(QNetworkReply *response);

    void sendMessage(QString message);
    void sendMessageCallback(QNetworkReply *response);

    void loadContacts();
    void loadContactsCallback(QNetworkReply *response);

    void loadChat(Contact *contact = nullptr);
    void loadChatCallback(QNetworkReply *response);

};

#endif // CLIENT_H
