#ifndef GROUP_H
#define GROUP_H

#include "contact.h"
#include <QObject>

class Group : public Contact
{
    Q_OBJECT
private:
    QString title;
public:
    explicit Group(QString name, QString title = "", QObject *parent = nullptr);

    ContactTypes type() override {
        return ContactTypes::Group;
    }

    QString strType() override {
        return "group";
    }
};

#endif // GROUP_H
