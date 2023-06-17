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
            client->bindChatLoaderOnContact(newTarget);
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


void ChatForm::on_actionExit_triggered()
{
    this->close();
    exit(0);
}

void ChatForm::on_actionLogout_triggered()
{
    if(popup("Sure?", "Are you sure you want to logout?", MessageTypes::Question)) {
        Client *client = (Client *) objClient;
        client->logout();
    }
}

void ChatForm::on_actionCreateGroup_triggered()
{
    bool okPressed;
    const QString name = QInputDialog::getText(this, "Group Name", "Enter the name of your group:", QLineEdit::Normal, "", &okPressed);

    if (okPressed && !name.isEmpty()) {
        QString title = QInputDialog::getText(this, "Group Title", "Enter the title of your group (Optional): ", QLineEdit::Normal, "", &okPressed);
        if(!okPressed)
            title = "";
        Client *client = (Client *) objClient;
        client->createGroupOrChannel("group", name, title);
    }
}

void ChatForm::on_actionCreateChannel_triggered()
{
    bool okPressed;
    const QString name = QInputDialog::getText(this, "Channel Name", "Enter the name of your channel:", QLineEdit::Normal, "", &okPressed);

    if (okPressed && !name.isEmpty()) {
        QString title = QInputDialog::getText(this, "Channel Title", "Enter the title of your channel (Optional): ", QLineEdit::Normal, "", &okPressed);
        if(!okPressed)
            title = "";
        Client *client = (Client *) objClient;
        client->createGroupOrChannel("channel", name, title);
    }
}
void ChatForm::on_actionJoinGroup_triggered()
{
    bool okPressed;
    const QString name = QInputDialog::getText(this, "Group Name", "Enter the name of the group you want to join:", QLineEdit::Normal, "", &okPressed);

    if (okPressed && !name.isEmpty()) {
        Client *client = (Client *) objClient;
        client->joinGroupOrChannel("group", name);
    }
}

void ChatForm::on_actionJoinChannel_triggered()
{
    bool okPressed;
    const QString name = QInputDialog::getText(this, "Channel Name", "Enter the name of the channel you want to join:", QLineEdit::Normal, "", &okPressed);

    if (okPressed && !name.isEmpty()) {
        Client *client = (Client *) objClient;
        client->joinGroupOrChannel("channel", name);
    }
}
