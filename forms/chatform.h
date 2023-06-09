#ifndef CHATFORM_H
#define CHATFORM_H

#include "form.h"
#include "ui_chatform.h"

namespace Ui {
class ChatForm;
}

class ChatForm : public Form
{
    Q_OBJECT

public:
    explicit ChatForm(QObject *client, Form *parent = nullptr);
    ~ChatForm();

    Forms type() {
        return Chat;
    }
private:
    Ui::ChatForm *ui;
    //Ui::ChatForm *getUI();

};

#endif // CHATFORM_H
