#ifndef CHATFORM_H
#define CHATFORM_H

#include "form.h"
#include "ui_chatform.h"
#include "../chat/contact.h"
#include <QListWidget>
#include <QListWidgetItem>
#include <QScrollBar>

namespace Ui {
class ChatForm;
}

class ChatForm : public Form
{
    Q_OBJECT

public:
    explicit ChatForm(QObject *client, Form *parent = nullptr);
    ~ChatForm();

    Forms type() override {
        return Chat;
    }
    Ui::ChatForm *ui;
    bool contactListContains(Contact *contact);
    void addNewContact(Contact *contact);
    void scrollChatToRecentMessage();
    void updateChatList(QString chat);

private slots:
    void on_btnSend_clicked();

    void on_lstContacts_itemSelectionChanged();

private:

    //Ui::ChatForm *getUI();

};

#endif // CHATFORM_H
