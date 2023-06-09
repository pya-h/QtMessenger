#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QMessageBox>

namespace Ui {
class Form;
}

typedef enum {
    Authentication,
    Chat,
    Base
} Forms;

typedef enum {
    Information,
    Warning,
    Error,
    Question
} MessageTypes;

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QObject *client, QWidget *parent = nullptr);
    ~Form();

    virtual Forms type() { return Base; }

    bool popup(QString title, QString text, MessageTypes messageType = Information);
   // void* getUI();
protected:
    Ui::Form *ui;
    QObject *objClient;
};

#endif // FORM_H
