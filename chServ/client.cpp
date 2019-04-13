#include "client.h"

xClientList::xClientList(): mutex(QMutex::Recursive)
{

}

int xClientList::seek(int userId) {
    mutex.lock();
    for( int i=0; i<count(); ++i )
        if ( at(i)->GetUserId()==userId) {mutex.unlock(); return i;}
    mutex.unlock();
    return -1;
}


void xClientList::insert(xClient* user) {
    mutex.lock();
    if (seek(user->GetUserId())<0)
        this->append(user);
    mutex.unlock();
}


int xClientList::NewUserId()
{
    mutex.lock();
    if (!length()) {mutex.unlock();return 0;}
    int max=first()->GetUserId();
    for (auto it:*this) if (max<it->GetUserId()) max=it->GetUserId();
    mutex.unlock();
    return max+1;
}


