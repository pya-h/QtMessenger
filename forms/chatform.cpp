#include "chatform.h"


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
/*
Ui::ChatForm *ChatForm::getUI() {
    return ui;
}
*/
