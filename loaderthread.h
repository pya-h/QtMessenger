#ifndef LOADERTHREAD_H
#define LOADERTHREAD_H

#include <QObject>

#include <QThread>
#include "chat/contact.h"

class LoaderThread : public QThread
{
Q_OBJECT
public:
    explicit LoaderThread(Contact* contact = nullptr, bool stop = false, QObject *parent = nullptr);
    void run();

    // if Stop = true, the thread will break
    // out of the loop, and will be disposed
    bool Stop;
    Contact *contact;
signals:
    // To communicate with Gui Thread
    // we need to emit a signal
    void timeToLoadNoParam();
    void timeToLoad(Contact *contact);

public slots:
};

#endif // LOADERTHREAD_H
