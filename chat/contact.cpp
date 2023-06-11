#include "contact.h"

const QString Contact::USERS_FOLDER = "Users", Contact::GROUPS_FOLDER = "Groups", Contact::CHANNELS_FOLDER = "Channels";


Contact::Contact(QString name, QObject *parent) : QObject(parent)
{
    this->name = name;

}

void Contact::setRecentMessage(QString newMessage) {
    if(!newMessage.isEmpty())
        this->recentMessage = newMessage;
}
QString Contact::getRecentMessage() {
    return this->recentMessage;
}

QString Contact::getRecentMessageSigned() {
    return this->name + ": " + this->recentMessage;
}

QString Contact::toString() {
    return getName();
}

QString Contact::getFileName() {
    switch (type()) {
        case User:
            return USERS_FOLDER + "/" + name;
        case Group:
            return GROUPS_FOLDER + "/" + name;
        case Channel:
            return CHANNELS_FOLDER + "/" + name;
        case None:
        default:
            return "";
    }
}
