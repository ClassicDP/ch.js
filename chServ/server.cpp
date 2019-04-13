
#include "server.h"

#include <QtWebSockets>
#include <QtCore>
#include <socketfunct.h>
#include <cstdio>
using namespace std;

QT_USE_NAMESPACE




static QString getIdentifier(QWebSocket *peer)
{
    return QStringLiteral("%1:%2").arg(peer->peerAddress().toString(),
                                       QString::number(peer->peerPort()));
}

//! [constructor]
Server::Server(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(new QWebSocketServer(QStringLiteral("xServer"),
                                            QWebSocketServer::NonSecureMode,
                                            this))
{

    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        QTextStream(stdout) << "Server listening on port " << port << '\n';
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &Server::onNewConnection);
    }
    list=new xClientList();
}

Server::~Server()
{
    m_pWebSocketServer->close();
    delete list;
}
//! [constructor]

//! [onNewConnection]
void Server::onNewConnection()
{

    pSocket = m_pWebSocketServer->nextPendingConnection();
    QTextStream(stdout) << getIdentifier(pSocket) << " connected!\n";
    pSocket->setParent(this);

    connect(pSocket, &QWebSocket::textMessageReceived,
            this, &Server::processMessage);
    connect(pSocket, &QWebSocket::disconnected,
            this, &Server::socketDisconnected);
    auto xClient=new xSocketClient(list, pSocket);
//    m_clients << new _client(pSocket,thread);
    connect(pSocket, &QWebSocket::textMessageReceived,
            xClient, &xSocketClient::SocketMsg, Qt::QueuedConnection);
    connect(xClient, &xSocketClient::sendMsg,
            this, &Server::sendMessage, Qt::QueuedConnection);
    QThread* thread = new QThread;
    xClient->moveToThread(thread);
    connect(xClient, SIGNAL (error(QString)), this, SLOT (errorString(QString)));
    connect(thread, SIGNAL (started()), xClient, SLOT (start()));
    connect(xClient, SIGNAL (finished()), thread, SLOT (quit()));
    connect(xClient, SIGNAL (finished()), xClient, SLOT (deleteLater()));
    connect(thread, SIGNAL (finished()), thread, SLOT (deleteLater()));
    thread->start();
    QThread * xThread= new QThread;
    xClient->moveToThread(xThread);
}
//! [onNewConnection]

//! [processMessage]
void Server::processMessage( const QString &message)
{
    //    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    //    for (QWebSocket *pClient : qAsConst(m_clients)) {
    //        if (pClient != pSender) //don't echo message back to sender
    //            pClient->sendTextMessage(message);
    //    }
}

//_client *Server::searchBySocket(QWebSocket *socket)
//{
//    foreach(auto it, m_clients) {
//        if (it->socket==socket)  return it;
//    }
//    return NULL;
//}

void Server::sendMessage(QWebSocket * socket, QString msg)
{    
    mutex.lock();
//    if (!searchBySocket(socket)) {qDebug () << "--NoSocket!"; mutex.unlock(); return;}
    socket->sendTextMessage(msg);
    mutex.unlock();
}
//! [processMessage]

//! [socketDisconnected]
void Server::socketDisconnected()
{
    mutex.lock();
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
//    _client * client=searchBySocket(pClient);
//    pClient->abort();
//    pClient->deleteLater();
//    if (client)
    {

//        m_clients.removeAll(client);

//        delete client;
    }
    mutex.unlock();
    qDebug () << "Disc complete";
}
//! [socketDisconnected]


//_client::_client(QWebSocket *socket, QThread *thread):socket(socket),thread(thread){
//    connect(this, SIGNAL(quitThread()), thread, SLOT(quit()));
//}

//bool _client::operator ==(const _client &a) {return (socket==a.socket && thread==a.thread);}

//_client::~_client() {qDebug () << "quit"; thread->quit(); }
