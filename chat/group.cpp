#include "group.h"

Group::Group(QString name, QString title, QObject *parent)
    : Contact{name, parent}
{
    this->title = title;
}
