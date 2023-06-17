#ifndef CONTACT_H
#define CONTACT_H

#include <QObject>
#include <QDate>

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
    QString getName() const {
        return name;
    }
protected:
    QString name, recentMessage;
    QString lastModified;
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

    bool operator== (const Contact &other) const {
        return this->name == other.getName();
    }

    QString getFileName();

    const static QString USERS_FOLDER, GROUPS_FOLDER, CHANNELS_FOLDER;

    void updateLastModifiedDate(QDateTime date) {
        this->lastModified = date.toString("yyyyMMddHHmmss");
    }

    QString getLastModifiedDate() {
        return this->lastModified;
    }
signals:

};

#endif // CONTACT_H
