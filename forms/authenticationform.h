#ifndef AUTHENTICATIONFORM_H
#define AUTHENTICATIONFORM_H

#include "form.h"
#include "ui_authenticationform.h"

namespace Ui {
class AuthenticationForm;
}

class AuthenticationForm : public Form
{
    Q_OBJECT

public:
    explicit AuthenticationForm(QObject *client, Form *parent = nullptr);
    ~AuthenticationForm();

    Forms type() {
        return Authentication;
    }
    Ui::AuthenticationForm *ui;
    //Ui::AuthenticationForm *getUI();
private slots:
    void on_btnSignup_clicked();


    void on_btnLogin_clicked();


    void on_btnLogout_clicked();
};

#endif // AUTHENTICATIONFORM_H
