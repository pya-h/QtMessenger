#ifndef USER_H
#define USER_H

#include <QObject>
#include "contact.h"

class User : public Contact
{
    Q_OBJECT
private:
    QString password;
public:
    explicit User(QString name, QString password = "", QObject *parent = nullptr);

    ContactTypes type() override {
        return ContactTypes::User;
    }

    QString strType() override {
        return "user";
    }

};

#endif // USER_H
