#ifndef USER_H
#define USER_H

#include <QObject>
#include "contact.h"
#include <QMap>

class User : public Contact
{
    Q_OBJECT
private:
    QString password;
    //QList<Contact *> contacts;
    QMap<Contact *, QString> chats;

   // QList<QString> chats;
public:
    explicit User(QString name, QString password = "", QObject *parent = nullptr);

    QString getChat(Contact *contact) {
        return chats.value(contact, "");
    }

    void updateChat(Contact *contact, QString chat);

    void setChat(Contact *contact, QString chat) {
        chats.insert(contact, chat);
    }

    QList<Contact *> getContacts() {
        return chats.keys();
    }

    Contact *getContact(QString name);
    void addNewContact(Contact *c) {
        chats.insert(c, "");
    }

    bool contactIsNew(Contact *c) {
        return !chats.contains(c);
    };

    ContactTypes type() override {
        return ContactTypes::User;
    }

    QString strType() override {
        return "user";
    }


};

#endif // USER_H
