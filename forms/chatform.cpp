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
        class User newTarget(ui->txtCurrentContact->text());
        client->setTarget(&newTarget);
        client->sendMessage(ui->txtNewMessage->text());
    }
}

bool ChatForm::contactListContains(Contact *contact) {

    QAbstractItemModel* model = ui->lstContacts->model();
    QString expression = contact->toString();
    int rowCount = model->rowCount();
    int columnCount = model->columnCount();

    for(int i = 0; i < rowCount; i++)
        for(int j = 0; j < columnCount; j++)
            if(model->index(i, j).data(Qt::DisplayRole).toString().contains(expression))
                return true;

    return false;

}

void ChatForm::addNewContact(Contact *contact) {
    if(!contactListContains(contact)) {
        /*QAbstractItemModel *model = ui->lstContacts->model();
        QModelIndex index = model->index(model->rowCount()-1, 0);
        model->setData(index, contact->toString());
        ui->lstContacts->setModel(model);*/
    }
}
