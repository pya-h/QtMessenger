#include "form.h"
#include "ui_form.h"

Form::Form(QObject *client, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);
    objClient = client;
}

Form::~Form()
{
    delete ui;
}

bool Form::popup(QString title, QString text, MessageTypes messageType) {
    if(messageType != Question) {
        QMessageBox msgbox;
        msgbox.setWindowTitle(title);
        msgbox.setText(text);
        switch(messageType) {
            case Information:
                msgbox.setIcon(QMessageBox::Icon::Information);
                break;

            case Warning:
                msgbox.setIcon(QMessageBox::Icon::Warning);
                break;
            case Error:
                msgbox.setIcon(QMessageBox::Icon::Critical);
                break;
            default:
                break;
        }
        msgbox.setButtonText(QMessageBox::Button::Ok, "OK!");
        msgbox.exec();

    }
    else {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, title, text, QMessageBox::Yes | QMessageBox::No);
        return reply == QMessageBox::Yes;
    }

    return true;
}
