#include "client.h"

Client::Client(ServerInterface *parent)
    : ServerInterface{parent}
{

    if(!QDir(DATA_FOLDER).exists()) {
        QDir().mkdir(DATA_FOLDER);
    }
}

const QString Client::CRED_FILE = "me.cred", Client::DATA_FOLDER = "data";

void Client::start() {
    if(loadCredentials()) {
        currentForm = new ChatForm(this);
        scheduleContactListLoad();
    }
    else {
        currentForm = new AuthenticationForm(this);

    }
    currentForm->show();

}

bool Client::loadCredentials() {
    QFile credFile(DATA_FOLDER + "/" + CRED_FILE);

    if(!credFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Cannot read credentials... need to login again!";
        return false;
    }

    if(!credFile.atEnd()) {
        username = QString(credFile.readLine()).replace("\n", "");
        qDebug() << username;
        if(!credFile.atEnd()) {
            password = QString(credFile.readLine()).replace("\n", "");
            qDebug() << password;

            if(!credFile.atEnd()) {
                token = QString(credFile.readLine()).replace("\n", "");
                qDebug() << token;
                this->setToken(token);
                this->me = new class User(this->username, this->password);
                return true;
            }
        }
    }
    return false;
}

QString Client::getChatWithContact(Contact *contact) {
    return me->getChat(contact != nullptr ? contact : target);
}

void Client::saveCredentials(){
    QFile credFile(DATA_FOLDER + "/" + CRED_FILE);
    if(!credFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        if(currentForm != nullptr) {
            currentForm->popup("Credntial Save Error", "Application cannot save your login credentials; this may cuase bugs in the performance!");
        }
        qDebug() << "Credential Save Error!";
        return;
    }
    QTextStream output(&credFile);

    output << username << "\n" << password << "\n" << token;

    credFile.close();
}

void Client::login(QString username, QString password) {
    // set query
    this->username = username;
    this->password = password;

    QUrlQuery query;
    query.addQueryItem("username", username);
    query.addQueryItem("password", password);

    QNetworkRequest request = PrepareRequest("login", query);
    QNetworkAccessManager *restfulApiClient = new QNetworkAccessManager(this);
    connect(restfulApiClient, &QNetworkAccessManager::finished,
                    this, &Client::loginCallback);

    //QNetworkReply *res = restclient->post(req, jsonPayload);
    //QNetworkReply *res = restclient->get(req);
    restfulApiClient->get(request);
}
void Client::loginCallback(QNetworkReply *response) {
    QJsonObject jsonObject = GetJsonObject(response);
    QString resultText = CheckResponse(jsonObject);
    // show message box with resultText;
    int code = jsonObject.value("code").toString().toInt();

    qDebug() << resultText;
    if(currentForm != nullptr && currentForm->type() == Authentication) {
        AuthenticationForm *authForm = (AuthenticationForm *) currentForm;

        if(code == 200) {
            QJsonValue newToken = jsonObject.value("token");

            if(!newToken.isNull()) {
                this->setToken(newToken.toString());
                this->me = new class User(this->username, this->password);
                if(!this->token.isEmpty()) {
                    authForm->popup("Login Successfull", "You\'ve logged in successfully...Now you can start chatting :)!");
                    saveCredentials();
                    currentForm = new ChatForm(this);
                    authForm->close();
                    currentForm->show();
                    scheduleContactListLoad();

                } else {
                    authForm->ui->lblResult->setText("Logged in Already: It seems that you\'ve already logged in... Please first press logout and then Log in again!");
                }
            } else {
                authForm->ui->lblResult->setText("Logged in Already: It seems that you\'ve already logged in... Please first press logout and then Log in again!");
            }
        } else {
            authForm->popup("Login Unsuccessful", resultText, MessageTypes::Error);
            authForm->ui->lblResult->setText(resultText);
            currentForm = new ChatForm(this);
        }
    }
}

void Client::logout() {
    QUrlQuery query;
    query.addQueryItem("username", username);
    query.addQueryItem("password", password);

    QNetworkRequest request = PrepareRequest("logout", query);
    QNetworkAccessManager *restclient = new QNetworkAccessManager(this);
    connect(restclient, &QNetworkAccessManager::finished,
                    this, &Client::logoutCallback);

    restclient->get(request);
}

void Client::logoutCallback(QNetworkReply *response) {
    QJsonObject jsonObject = GetJsonObject(response);
    QString resultText = CheckResponse(jsonObject);
    // show message box with resultText;
    QJsonValue code = jsonObject.value("code");

    if(code.toString().toInt() == 200) {
        this->setToken("");
        // EXIT CHAT FORM

        this->username = "";
        this->password = "";
        if(currentForm != nullptr) {
             if(currentForm->type() == Forms::Authentication) {
                 AuthenticationForm *authForm = (AuthenticationForm *) currentForm;
                 authForm->ui->lblResult->setText(resultText);
             } else if(currentForm->type() == Forms::Chat) {
                QFile credFile(DATA_FOLDER + "/" + CRED_FILE);
                if(credFile.exists()) {
                    credFile.remove();
                }
                // THEN DELETE OFFLINE DATA
                if(currentForm != nullptr) {
                    currentForm->close();
                    exit(0);
                }

             }
         }
    }

}

void Client::signup(QString username, QString password, QString firstname, QString lastname) {
    // set query
    QUrlQuery query;
    query.addQueryItem("username", username);
    query.addQueryItem("password", password);
    if(firstname != nullptr && !firstname.isEmpty())
        query.addQueryItem("firstname", firstname);
    if(lastname != nullptr && !lastname.isEmpty())
        query.addQueryItem("lastname", lastname);
    QNetworkRequest request = PrepareRequest("signup", query);
    QNetworkAccessManager *restclient = new QNetworkAccessManager(this);
    connect(restclient, &QNetworkAccessManager::finished,
                    this, &Client::signupCallback);

    restclient->get(request);
}

void Client::signupCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    QString resultText = Client::CheckResponse(jsonObject);

    int code = jsonObject.value("code").toString().toInt();

    if(code == 200) {
        resultText += "\nNow you can log into this acount...";
    }

   if(currentForm != nullptr && currentForm->type() == Authentication) {
        AuthenticationForm *authForm = (AuthenticationForm *) currentForm;
        authForm->ui->lblResult->setText(resultText);
    }
}

void Client::setTarget(Contact *nextTarget) {
    this->target = nextTarget;
}

void Client::setTarget(unsigned int contactIndex) {
    this->target = me->getContacts().at(contactIndex);
    if(currentForm != nullptr && currentForm->type() == Forms::Chat) {
        ChatForm *chatForm = (ChatForm *) currentForm;
        chatForm->ui->txtCurrentContact->setText(this->target->getName());
    }
}

void Client::sendMessage(QString message) {
    // set query
    QUrlQuery query = tokenBasedQuery();
    me->setRecentMessage(message);
    query.addQueryItem("dst", target->getName());
    query.addQueryItem("body", message);

    QNetworkRequest request = PrepareRequest("sendmessage" + target->strType(), query);
    QNetworkAccessManager *restclient = new QNetworkAccessManager(this);
    connect(restclient, &QNetworkAccessManager::finished,
                    this, &Client::sendMessageCallback);

    restclient->get(request);
}

void Client::sendMessageCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    QString resultText = Client::CheckResponse(jsonObject);
    try {
        int code = jsonObject.value("code").toString().toInt();
        if(code == 200) {
            ChatForm *chatForm = (ChatForm *) currentForm;
            QString chatList = chatForm->ui->txtChatList->toPlainText();
            chatForm->ui->txtChatList->setPlainText(chatList + "\n\n" + me->getRecentMessageSigned());
            if(me->contactIsNew(target)) {
                me->addNewContact(target);
                chatForm->addNewContact(target);
            }
        } else {
            currentForm->popup("Message Send Failure", resultText, MessageTypes::Error);
        }
    }
    catch (QException ex) {
        qDebug() << ex.what();
    }
}

void Client::scheduleContactListLoad() {
    LoaderThread *contactLoadThread = new LoaderThread();
    connect(contactLoadThread, &LoaderThread::timeToLoadNoParam, this, &Client::loadContacts);
    contactLoadThread->start();
}

void Client::bindChatLoaderOnContact(Contact *contact) {
    LoaderThread *loader = new LoaderThread(contact);
    connect(loader, &LoaderThread::timeToLoad, this, &Client::loadChat);
    loader->start();
}
void Client::loadContacts() {
    QUrlQuery query = tokenBasedQuery();
    QNetworkRequest request = PrepareRequest("getuserlist", query);
    QNetworkAccessManager *rest = new QNetworkAccessManager(this);
    connect(rest, &QNetworkAccessManager::finished, this, &Client::loadContactsCallback);
    rest->get(request);
}

void Client::loadContactsCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    QString resultText = Client::CheckResponse(jsonObject);

    int code = jsonObject.value("code").toString().toInt();

    if(code == 200) {
        if(currentForm != nullptr && currentForm->type() == Forms::Chat) {
             ChatForm *chatForm = (ChatForm *) currentForm;
             QJsonValue value = jsonObject.value("block 0");
             qDebug() << value.toObject();
             for(int i = 1; !value.isNull() && value.isObject(); i++) {

                QString name = value.toObject().value("src").toString();
                if(name.isEmpty())
                    break;
                if(me->getContact(name) == nullptr) {
                    Contact *c = new class User(name);
                    me->addNewContact(c);
                    chatForm->addNewContact(c);
                    bindChatLoaderOnContact(c);
                }
                //next step:
                QString blockId = "block " + QString::number(i);
                value = jsonObject.value(blockId);
             }

         }
    } else {
        qDebug() << resultText;
        currentForm->popup("Error", resultText, MessageTypes::Error);
    }

}

void Client::loadChat(Contact *contact) {
    if(contact == nullptr)
        contact = target;
    if(contact != nullptr) {
        QUrlQuery query = tokenBasedQuery();
        query.addQueryItem("dst", contact->getName());
        QNetworkRequest request = PrepareRequest("get" + contact->strType() + "chats", query);
        QNetworkAccessManager *rest = new QNetworkAccessManager(this);
        connect(rest, &QNetworkAccessManager::finished, this, &Client::loadChatCallback);
        rest->get(request);
    }
}

void Client::loadChatCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    QString resultText = Client::CheckResponse(jsonObject);

    int code = jsonObject.value("code").toString().toInt();
    if(code == 200) {
        Contact *contact;
        //load "block #x" from json
        QString chat;
        if(currentForm != nullptr && currentForm->type() == Forms::Chat) {
            ChatForm *chatForm = (ChatForm *) currentForm;
            QJsonValue value = jsonObject.value("block 0");
            if(!value.isNull() && value.isObject()) {
                QString src = value.toObject().value("src").toString(),
                        dst = value.toObject().value("dst").toString();
                contact = src == me->getName() ? me->getContact(dst) : me->getContact(src);

                for(int i = 1; !value.isNull() && value.isObject(); i++) {
                    QJsonObject jMessage = value.toObject();

                    QString name = jMessage.value("src").toString(),
                         message = jMessage.value("body").toString();
                    if(name.isEmpty())
                     break;
                    chat += name + ": " + message + "\n\n";
                    //next step:
                    QString blockId = "block " + QString::number(i);
                    value = jsonObject.value(blockId);
                }
                me->setChat(contact, chat);
                if(contact == target)
                    chatForm->updateChatList(me->getChat(contact));
                // & save into file
                // else => just save in file
            }
         }
    } else {
        qDebug() << resultText;
        currentForm->popup("Error", resultText, MessageTypes::Error);
    }

}
