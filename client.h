#ifndef CLIENT_H
#define CLIENT_H
#include <QtCore/QtCore>
#include "serverinterface.h"
#include "forms/authenticationform.h"
#include "forms/chatform.h"
#include "chat/user.h"
#include "chat/group.h"
#include "chat/channel.h"
#include "loaderthread.h"
#include <QFile>


class Client : public ServerInterface
{
    Q_OBJECT
private:

    Form *currentForm = nullptr;
    class User *me = nullptr;
    Contact *target = nullptr;

    const static QString CRED_FILE, DATA_FOLDER;
public:
    explicit Client(ServerInterface *parent = nullptr);
    ~Client();
    void start();
    void saveCredentials();
    bool loadCredentials();

    void loadOfflineData();
signals:
public slots:

    void login(QString username, QString password);
    void loginCallback(QNetworkReply *response);

    void logout();
    void logoutCallback(QNetworkReply *response);

    Contact *getTarget() { return this->target; }

    QString getChatWithContact(Contact *contact = nullptr);

    void setTarget(Contact *nextTarget);
    void setTarget(unsigned int contactIndex);

    void signup(QString username, QString password, QString firstname = nullptr, QString lastname = nullptr);
    void signupCallback(QNetworkReply *response);

    void sendMessage(QString message);
    void sendMessageCallback(QNetworkReply *response);

    void loadContacts();
    void loadUsersCallback(QNetworkReply *response);
    void loadGroupsCallback(QNetworkReply *response);
    void loadChannelsCallback(QNetworkReply *response);

    void loadChat(Contact *contact = nullptr);
    void loadUserChatCallback(QNetworkReply *response);
    void loadGroupOrChannelChatCallback(QNetworkReply *response);

    Contact *getContactByName(QString name) {
        return me->getContact(name);
    }

    void bindChatLoaderOnContact(Contact *contact);
    void scheduleContactListLoad();

    void createGroupOrChannel(const QString typeName, const QString &name, const QString &title);
    void handleCreationOrJoining(QNetworkReply *response);
    void joinGroupOrChannel(const QString typeName, const QString &name);

};

#endif // CLIENT_H
