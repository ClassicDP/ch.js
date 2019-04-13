#ifndef CLIENT_H
#define CLIENT_H
#include "QThread"
#include "QTimer"
#include "QDateTime"
#include "QJsonObject"
#include "QJsonDocument"
#include "QWebSocket"
#include "QDebug"
#include "QEventLoop"
#include "QMutex"
#include "game.h"
class xClient:public Game
{
Q_OBJECT
    int id;
public:

    xClient (int id):Game::Game(8){this->id=id;}
    int GetUserId() const {return id;}

};
class xClientList: public QList <xClient *>
{
     QMutex mutex;
public:
    xClientList();
    int seek(int  userId);
    xClient  GetById(int userId);
    void insert (xClient *user);
    int NewUserId();
};


struct xFunct
{
    QString fName;
    void (*F) (QJsonObject args);
};

class xCallBack
{
    QList <xFunct> fList;
public:
    xCallBack()
    {
//        fList.append({"AskId",AskId});
    }
};


#endif // CLIENT_H


