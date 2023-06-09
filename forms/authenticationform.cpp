#include "authenticationform.h"

#include "../client.h"

AuthenticationForm::AuthenticationForm(QObject *client, Form *parent) :
    Form(client, parent),
    ui(new Ui::AuthenticationForm)
{
    ui->setupUi(this);
}

AuthenticationForm::~AuthenticationForm()
{
    delete ui;
}

void AuthenticationForm::on_btnSignup_clicked()
{
    Client *client = (Client *) objClient;
    if(!ui->txtLoginUsername->text().isEmpty() && !ui->txtLoginPassword->text().isEmpty())
        client->signup(ui->txtSignupUsername->text(), ui->txtSignupPassword->text(), ui->txtSignupFirstname->text(), ui->txtSignupLastname->text());
    else
        popup("Incompleted Credentials", "Please fill in the required fields.", MessageTypes::Warning);

}

void AuthenticationForm::on_btnLogin_clicked()
{
    Client *client = (Client *) objClient;
    if(!ui->txtLoginUsername->text().isEmpty() && !ui->txtLoginPassword->text().isEmpty())
        client->login(ui->txtLoginUsername->text(), ui->txtLoginPassword->text());
    else
        popup("Incompleted Credentials", "Please fill in the required fields.", MessageTypes::Warning);
}

void AuthenticationForm::on_btnLogout_clicked()
{
    Client *client = (Client *) objClient;
    client->logout();
}
