#include "client.h"

Client::Client(ServerInterface *parent)
    : ServerInterface{parent}
{
    // prepare offline folders
    const QString folders[] = {DATA_FOLDER, DATA_FOLDER + "/" + Contact::USERS_FOLDER, DATA_FOLDER + "/" + Contact::GROUPS_FOLDER, DATA_FOLDER + "/" + Contact::CHANNELS_FOLDER};

    for(int i = 0; i < folders->length(); i++) {
        if(!QDir(folders[i]).exists())
            QDir().mkdir(folders[i]);

    }

}

const QString Client::CRED_FILE = "me.cred", Client::DATA_FOLDER = "data";

Client::~Client() {
    if(me != nullptr)
        me->saveAllChats();
}

void Client::start() {
    if(loadCredentials()) {
        currentForm = new ChatForm(this);
        currentForm->show();

        loadOfflineData();
        scheduleContactListLoad();
    }
    else {
        currentForm = new AuthenticationForm(this);
        currentForm->show();

    }
}

QString Client::getChatWithContact(Contact *contact) {
    return me->getChat(contact != nullptr ? contact : target);
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
                this->me = new class User(this->username, this->password, DATA_FOLDER);
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
                 // THEN DELETE OFFLINE DATA
                QDir dataFolder(DATA_FOLDER);
                if(dataFolder.exists())
                    dataFolder.removeRecursively();

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

            if(me->contactIsNew(target)) {
                me->addNewContact(target);
                chatForm->addNewContact(target);
            }
            else {
                me->updateChat(target, me->getRecentMessageSigned());
                me->saveChatWith(target);
            }
            chatForm->ui->txtChatList->setPlainText(me->getChat(target));
            chatForm->scrollChatToRecentMessage();

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
    connect(rest, &QNetworkAccessManager::finished, this, &Client::loadUsersCallback);
    rest->get(request);

    request = PrepareRequest("getgrouplist", query);
    rest = new QNetworkAccessManager(this);
    connect(rest, &QNetworkAccessManager::finished, this, &Client::loadGroupsCallback);
    rest->get(request);

    request = PrepareRequest("getchannellist", query);
    rest = new QNetworkAccessManager(this);
    connect(rest, &QNetworkAccessManager::finished, this, &Client::loadChannelsCallback);
    rest->get(request);
}

void Client::loadUsersCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    const QString resultText = Client::CheckResponse(jsonObject),
        strCode = jsonObject.value("code").toString();

    if(!strCode.isEmpty()) {
        const int code = strCode.toInt();
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

            currentForm->popup("Error", resultText, MessageTypes::Error);
        }
    }
    else {
        // probably connections is lost
        qDebug() << resultText;
    }
}

void Client::loadGroupsCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    const QString resultText = Client::CheckResponse(jsonObject),
        strCode = jsonObject.value("code").toString();

    if(!strCode.isEmpty()) {
        const int code = strCode.toInt();
        if(code == 200) {
            if(currentForm != nullptr && currentForm->type() == Forms::Chat) {
                 ChatForm *chatForm = (ChatForm *) currentForm;
                 QJsonValue value = jsonObject.value("block 0");
                 qDebug() << value.toObject();
                 for(int i = 1; !value.isNull() && value.isObject(); i++) {

                    QString name = value.toObject().value("group_name").toString();
                    if(name.isEmpty())
                        break;
                    if(me->getContact(name) == nullptr) {
                        Contact *c = new class Group(name);
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

            currentForm->popup("Error", resultText, MessageTypes::Error);
        }
    }
    else {
        // probably connections is lost
        qDebug() << resultText;
    }
}

void Client::loadChannelsCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    const QString resultText = Client::CheckResponse(jsonObject),
        strCode = jsonObject.value("code").toString();

    if(!strCode.isEmpty()) {
        const int code = strCode.toInt();
        if(code == 200) {
            if(currentForm != nullptr && currentForm->type() == Forms::Chat) {
                 ChatForm *chatForm = (ChatForm *) currentForm;
                 QJsonValue value = jsonObject.value("block 0");
                 qDebug() << value.toObject();
                 for(int i = 1; !value.isNull() && value.isObject(); i++) {

                    QString name = value.toObject().value("channel_name").toString();
                    if(name.isEmpty())
                        break;
                    if(me->getContact(name) == nullptr) {
                        Contact *c = new class Channel(name);
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

            currentForm->popup("Error", resultText, MessageTypes::Error);
        }
    }
    else {
        // probably connections is lost
        qDebug() << resultText;
    }
}
void Client::loadChat(Contact *contact) {

    if(contact == nullptr)
        contact = target;
    if(contact != nullptr) {
        QUrlQuery query = tokenBasedQuery();
        query.addQueryItem("dst", contact->getName());
        if(!contact->getLastModifiedDate().isEmpty())
            query.addQueryItem("date", contact->getLastModifiedDate());
        QNetworkRequest request = PrepareRequest("get" + contact->strType() + "chats", query);
        QNetworkAccessManager *rest = new QNetworkAccessManager(this);
        connect(rest, &QNetworkAccessManager::finished, this, contact->type() == ContactTypes::User ? &Client::loadUserChatCallback : &Client::loadGroupOrChannelChatCallback);
        rest->get(request);
    }
}

void Client::loadUserChatCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    const QString resultText = Client::CheckResponse(jsonObject),
        strCode = jsonObject.value("code").toString();

    if(!strCode.isEmpty()) {
        const int code = strCode.toInt();
        if(code == 200) {
            Contact *contact;
            //load "block #x" from json
            QString newChat;
            if(currentForm != nullptr && currentForm->type() == Forms::Chat) {
                ChatForm *chatForm = (ChatForm *) currentForm;
                QJsonValue value = jsonObject.value("block 0");
                if(!value.isNull() && value.isObject()) {
                    QString src = value.toObject().value("src").toString(),
                            dst = value.toObject().value("dst").toString();
                    contact = me->getContact(me->getName() == src ? dst : src);

                    for(int i = 1; !value.isNull() && value.isObject(); i++) {
                        QJsonObject jMessage = value.toObject();

                        QString name = jMessage.value("src").toString(),
                             message = jMessage.value("body").toString();
                        if(name.isEmpty())
                            break;
                        if(!message.isEmpty())
                            newChat += "\n\n" + name + ": " + message;
                        //next step:
                        QString blockId = "block " + QString::number(i);
                        value = jsonObject.value(blockId);
                    }
                    me->updateChat(contact, newChat);
                    me->saveChatWith(contact);
                    if(contact == target)
                        chatForm->updateChatList(me->getChat(contact));
                    // & save into file
                    // else => just save in file
                }
             }
        } else {
            currentForm->popup("Error", resultText, MessageTypes::Error);
        }
    }
    else {
        // probably connections is lost
        qDebug() << resultText;
    }
}

void Client::loadGroupOrChannelChatCallback(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    const QString resultText = Client::CheckResponse(jsonObject),
        strCode = jsonObject.value("code").toString();

    if(!strCode.isEmpty()) {
        const int code = strCode.toInt();
        if(code == 200) {
            Contact *contact;
            //load "block #x" from json
            QString newChat;
            if(currentForm != nullptr && currentForm->type() == Forms::Chat) {
                ChatForm *chatForm = (ChatForm *) currentForm;
                QJsonValue value = jsonObject.value("block 0");
                if(!value.isNull() && value.isObject()) {
                    QString src = value.toObject().value("src").toString(),
                            dst = value.toObject().value("dst").toString();
                    contact = me->getContact(src);
                    if(contact == nullptr || contact->type() == ContactTypes::User) {
                        contact = me->getContact(dst);
                        if(contact == nullptr || contact->type() == ContactTypes::User)
                            return; // something wrong
                    }
                    for(int i = 1; !value.isNull() && value.isObject(); i++) {
                        QJsonObject jMessage = value.toObject();

                        QString name = jMessage.value("src").toString(),
                             message = jMessage.value("body").toString();
                        if(name.isEmpty())
                         break;
                        if(!message.isEmpty())
                            newChat += "\n\n" + name + ": " + message;
                        //next step:
                        QString blockId = "block " + QString::number(i);
                        value = jsonObject.value(blockId);
                    }
                    me->updateChat(contact, newChat);
                    me->saveChatWith(contact);
                    if(contact == target)
                        chatForm->updateChatList(me->getChat(contact));
                    // & save into file
                    // else => just save in file
                }
             }
        } else {
            currentForm->popup("Error", resultText, MessageTypes::Error);
        }
    }
    else {
        // probably connections is lost
        qDebug() << resultText;
    }
}


void Client::createGroupOrChannel(const QString typeName, const QString &name, const QString &title) {
    QUrlQuery query = tokenBasedQuery();
    query.addQueryItem(typeName + "_name", name);
    if(!title.isEmpty())
        query.addQueryItem(typeName + "_title", title);
    QNetworkRequest request = PrepareRequest("create" + typeName, query);
    QNetworkAccessManager *rest = new QNetworkAccessManager(this);
    connect(rest, &QNetworkAccessManager::finished, this, &Client::handleCreationOrJoining);
    rest->get(request);
}

void Client::handleCreationOrJoining(QNetworkReply *response) {
    QJsonObject jsonObject = Client::GetJsonObject(response);
    const QString resultText = Client::CheckResponse(jsonObject);
    int code = jsonObject.value("code").toString().toInt();

    if(code == 200) {
        currentForm->popup("Successful", "Operation was successful!");
    }
    else {
        currentForm->popup("Error", resultText);
    }
}


void Client::joinGroupOrChannel(const QString typeName, const QString &name) {
    QUrlQuery query = tokenBasedQuery();
    query.addQueryItem(typeName + "_name", name);

    QNetworkRequest request = PrepareRequest("join" + typeName, query);
    QNetworkAccessManager *rest = new QNetworkAccessManager(this);
    connect(rest, &QNetworkAccessManager::finished, this, &Client::handleCreationOrJoining);
    rest->get(request);
}

// ------------ OFFLINE SECTION --------

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
                this->me = new class User(this->username, this->password, DATA_FOLDER);
                return true;
            }
        }
    }
    return false;
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

void Client::loadOfflineData() {
    if(currentForm != nullptr && currentForm->type() == Forms::Chat) {
        ChatForm *chatForm = (ChatForm *) currentForm;
        const QString dirs[] = {Contact::USERS_FOLDER, Contact::GROUPS_FOLDER, Contact::CHANNELS_FOLDER};
        for(QString dirname: dirs) {
            QDir dir(DATA_FOLDER + "/" + dirname);
            for(const QFileInfo &file: dir.entryInfoList(QDir::Files)) {
                QFile chatFileReader(file.filePath());
                if(chatFileReader.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    const QString chat = chatFileReader.readAll();

                    Contact *c = nullptr;
                    if(dirname == Contact::USERS_FOLDER)
                        c = new class User(file.fileName());
                    else if(dirname == Contact::GROUPS_FOLDER)
                        c = new class Group(file.fileName());
                    else
                        c = new class Channel(file.fileName());
                    c->updateLastModifiedDate(file.lastModified());
                    me->setChat(c, chat);
                    chatForm->addNewContact(c);
                    bindChatLoaderOnContact(c);
                    chatFileReader.close();
                } else {
                    qDebug() << "Error reading offline chats with: " << file.filePath();
                }
            }
        }
    }
}
