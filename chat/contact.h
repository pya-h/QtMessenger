#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>

typedef enum {
    User,
    Group,
    Channel,
    None
} ContactTypes;

class Contact : public QObject
{
    Q_OBJECT
public:
    explicit Contact(QString name, QObject *parent = nullptr);
    QString getName() {
        return name;
    }
protected:
    QString name, recentMessage;
public:
    virtual ContactTypes type() {
        return None;
    }
    virtual QString strType() {
        return "None";
    }

    void setRecentMessage(QString newMessage);
    QString getRecentMessage();
    QString getRecentMessageSigned();
    QString toString();
signals:

};

#endif // CONTACT_H
