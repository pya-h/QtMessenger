#include "loaderthread.h"


LoaderThread::LoaderThread (Contact* contact, bool stop, QObject *parent) :
    QThread(parent), Stop(stop), contact(contact)
{
}

// run() will be called when this thread start
void LoaderThread ::run()
{
    //to prevent consuming memory every time, I defined the variables at once
    while(1)
    {
        emit contact != nullptr ? timeToLoad(contact) : timeToLoadNoParam();
        this->msleep(5000);
    }
}
