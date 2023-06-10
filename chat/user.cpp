#include "user.h"

User::User(QString name, QString password, QObject *parent) : Contact(name, parent)
{
    this->password = password;
}

void User::updateChat(Contact *contact, QString chat) {

    if(getChat(contact).isEmpty()) {
        setChat(contact, chat);
        return;
    }
    chats[contact] += "\n\n" + chat;

}

Contact *User::getContact(QString name) {
    for(Contact *c: getContacts()) {
        if(c->getName() == name)
            return c;
    }
    return nullptr;
}
