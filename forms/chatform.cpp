#include "chatform.h"
#include "../client.h"

ChatForm::ChatForm(QObject *client, Form *parent) :
    Form(client, parent),
    ui(new Ui::ChatForm)
{
    ui->setupUi(this);

}

ChatForm::~ChatForm()
{
    delete ui;
}

void ChatForm::on_btnSend_clicked()
{
    // for now:
    if(!ui->txtCurrentContact->text().isEmpty() && !ui->txtNewMessage->text().isEmpty()) {
        Client *client = (Client *) objClient;
        Contact *newTarget = client->getContactByName(ui->txtCurrentContact->text());
        if(newTarget == nullptr) {
            newTarget = new class User(ui->txtCurrentContact->text());
            client->bindLoaderOnContact(newTarget);
        }
        client->setTarget(newTarget);
        client->sendMessage(ui->txtNewMessage->text());
        ui->txtNewMessage->setText("");
    }
}

void ChatForm::addNewContact(Contact *contact) {

   QListWidgetItem *item = new QListWidgetItem(contact->getName());
   ui->lstContacts->addItem(item);
}


void ChatForm::on_lstContacts_itemSelectionChanged()
{
 //  qDebug() << ui->lstContacts->SelectedClicked();
   QList<QListWidgetItem*> selections(ui->lstContacts->selectedItems());
   qDebug() << selections;
   if(selections.size() > 0) {
       auto item = selections.at(0);

       Client *client = (Client *) objClient;
       Contact *contact = client->getContactByName(item->text());

       if(contact != nullptr) {
            ui->txtCurrentContact->setText(contact->getName());
            client->setTarget(contact);
            updateChatList(client->getChatWithContact());
       }

   }
}

void ChatForm::scrollChatToRecentMessage() {
    QScrollBar *v = ui->txtChatList->verticalScrollBar();
    v->setValue(v->maximum());
}

void ChatForm::updateChatList(QString chat) {
    ui->txtChatList->setPlainText(chat);
    scrollChatToRecentMessage();
}

