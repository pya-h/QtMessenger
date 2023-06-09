#include "user.h"

User::User(QString name, QString password, QObject *parent) : Contact(name, parent)
{
    this->password = password;
}
