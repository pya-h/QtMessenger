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
        class User *newTarget = new class User(ui->txtCurrentContact->text());
        client->setTarget(newTarget);
        client->sendMessage(ui->txtNewMessage->text());
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
       int index = 0;
       for(index = 0; index < ui->lstContacts->count() && ui->lstContacts->item(index) != item; index++);

       qDebug() << index;

       if(index < ui->lstContacts->count()) {
           Client *client = (Client *) objClient;
           client->setTarget(index);
           client->loadChat();
       }

   }
}

