#ifndef CHANNEL_H
#define CHANNEL_H

#include "contact.h"
#include <QObject>

class Channel : public Contact
{
    Q_OBJECT
private:
    QString title;
public:
    explicit Channel(QString name, QString title = "", QObject *parent = nullptr);

    ContactTypes type() override {
        return ContactTypes::Channel;
    }

    QString strType() override {
        return "channel";
    }
};

#endif // CHANNEL_H
