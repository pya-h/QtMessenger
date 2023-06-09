#include "client.h"

Client::Client(ServerInterface *parent)
    : ServerInterface{parent}
{

}

void Client::start() {
    currentForm = new AuthenticationForm(this);
    currentForm->show();
}

void Client::login(QString username, QString password) {
    // set query
    this->username = username;
    this->password = password;

    QUrlQuery query;
    query.addQueryItem("username", username);
    query.addQueryItem("password", password);

    QNetworkRequest request = PrepareRequest("/login", query);
    QNetworkAccessManager *restclient;
    restclient = new QNetworkAccessManager(this);
    connect(restclient, &QNetworkAccessManager::finished,
                    this, &Client::loginCallback);

    //QNetworkReply *res = restclient->post(req, jsonPayload);
    //QNetworkReply *res = restclient->get(req);
    restclient->get(request);

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
                    currentForm = new ChatForm(this);
                    authForm->close();
                    currentForm->show();
                }
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

    QNetworkRequest request = PrepareRequest("/logout", query);
    QNetworkAccessManager *restclient;
    restclient = new QNetworkAccessManager(this);
    connect(restclient, &QNetworkAccessManager::finished,
                    this, &Client::logoutCallback);

    restclient->get(request);
}

void Client::logoutCallback(QNetworkReply *response) {
    QJsonObject jsonObject = GetJsonObject(response);
    QString resultText = CheckResponse(jsonObject);
    // show message box with resultText;
    QJsonValue code = jsonObject.value("code");

    if(code.toInt() == 200) {
        this->setToken(nullptr);
        // EXIT CHAT FORM

        this->username = "";
        this->password = "";
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
    QNetworkRequest request = PrepareRequest("/signup", query);
    QNetworkAccessManager *restclient;
    restclient = new QNetworkAccessManager(this);
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

void Client::sendMessage(QString message) {
    // set query
    QUrlQuery query = tokenBasedQuery();
    me->setRecentMessage(message);
    query.addQueryItem("dst", target->getName());
    query.addQueryItem("body", message);

    QNetworkRequest request = PrepareRequest("/sendmessage" + target->strType(), query);
    QNetworkAccessManager *restclient;
    restclient = new QNetworkAccessManager(this);
    connect(restclient, &QNetworkAccessManager::finished,
                    this, &Client::signupCallback);

    restclient->get(request);
}

void Client::sendMessageCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    QString resultText = Client::CheckResponse(jsonObject);

    int code = jsonObject.value("code").toString().toInt();
    if(code == 200) {
        ChatForm *chatForm = (ChatForm *) currentForm;
        QString chatList = chatForm->ui->txtChatList->toPlainText();
        chatForm->ui->txtChatList->setPlainText(chatList + "\n" + me->getRecentMessageSigned());
        chatForm->addNewContact(target);
    } else {
        currentForm->popup("Message Send Failure", resultText, MessageTypes::Error);
    }
}
