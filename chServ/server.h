
#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebSocket>
#include <QThread>
#include "socketfunct.h"



QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)




class xClient
{
    QWebSocket * socket;
    QThread *thread;
    QString sId;
    int id;
public:
    xClient (QWebSocket * socket, QThread *thread):socket(socket), thread(thread) {}
    xClient (int id):id(id){this->sId=QString(id);}
    QString sGetUserId(){return sId;}
    int iGetUserId() {return id;}
};
class xClientList: public QList <xClient>
{
public:
    bool seek (QString  user);
    bool seek (int  user);
    void insert (xClient user);
    void insert (int user);
    QString NewUserId();
};

class xCommands
{
    xClientList list;
public:
    xCommands(xClientList list);
    QString GivId();
    void actions(QJsonObject js);

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

class Server : public QObject
{
    Q_OBJECT
    QWebSocket *pSocket;
    xClientList list;
    xCommands *xC;
public:
    explicit Server(quint16 port, QObject *parent = nullptr);
    ~Server() override;

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
    void sendMessage(QString msg);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
    QList<QWebSocket *> m_clients;
};

#endif //CHATSERVER_H
