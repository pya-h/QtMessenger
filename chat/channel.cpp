#include "channel.h"

Channel::Channel(QString name, QString title, QObject *parent)
    : Contact{name, parent}
{
    this->title = title;
}
