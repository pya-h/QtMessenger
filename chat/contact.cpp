#include "contact.h"

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
