#include "user.h"

User::User(QString name, QString password, QString parentFolder, QObject *parent) : Contact(name, parent)
{
    this->password = password;
    this->parentFolder = parentFolder;
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

void User::saveChatWith(Contact *contact) {
    const QString error = "Chat Save Error: Error while trying to save chat offline data for " + contact->strType() + ": " + contact->getName() + "!";
    if(contact != nullptr && !parentFolder.isEmpty()) {
        QString chat = getChat(contact), fileName = contact->getFileName();
        if(!fileName.isEmpty()) {
            QFile chatFile(parentFolder + "/" + fileName);
            if(!chatFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                qDebug() << error;
            }
            QTextStream output(&chatFile);
            output << chat;
            chatFile.close();
        }
        else {
            qDebug() << error;
        }
    }
    else {
        qDebug() << "Request for saving chat with a nullptr contact!";
    }
}

void User::saveAllChats() {
    for(auto chat = chats.cbegin(), end = chats.cend(); chat != end; chat++) {
        saveChatWith(chat.key());
    }
}
