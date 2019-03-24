
#include "server.h"

#include <QtWebSockets>
#include <QtCore>

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
    xC=new xCommands(list);
}

Server::~Server()
{
    m_pWebSocketServer->close();
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

    m_clients << pSocket;
    auto thread=new xSocketThread(pSocket);
    connect(thread, &xSocketThread::sendMsg,
            this, &Server::sendMessage);

    list << *new xClient(pSocket, thread);

    thread->start();


    //    pSocket->sendTextMessage(getIdentifier(pSocket));
}
//! [onNewConnection]

//! [processMessage]
void Server::processMessage(const QString &message)
{
    xC->actions(jsonFromString(message));
    //    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    //    for (QWebSocket *pClient : qAsConst(m_clients)) {
    //        if (pClient != pSender) //don't echo message back to sender
    //            pClient->sendTextMessage(message);
    //    }
}

void Server::sendMessage(QString msg)
{
    pSocket->sendTextMessage(msg);
}
//! [processMessage]

//! [socketDisconnected]
void Server::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    QTextStream(stdout) << getIdentifier(pClient) << " disconnected!\n";
    if (pClient)
    {
        m_clients.removeAll(pClient);
        pClient->deleteLater();
    }
}
//! [socketDisconnected]

xCommands::xCommands(xClientList list):list(list){}

QString xCommands::GivId()
{
    QJsonObject js;
    js["cmd"]="AskId";
    return jsonToString(QJsonObject(js));
}

void xCommands::actions(QJsonObject js)
{
    if (js["cmd"]=="Id") {
        if (!list.seek(js["Id"].toInt())) list.insert(js["Id"].toInt());
    }
}

bool xClientList::seek(QString user) {
    for (auto it:*this)
        if (it.sGetUserId()==user) return true;
    return false;
}

bool xClientList::seek(int user) {
    for (auto it:*this)
        if (it.iGetUserId()==user) return true;
    return false;
}

void xClientList::insert(xClient user) {
    if (!seek(user.iGetUserId())) this->append(user);
}

void xClientList::insert(int user) {
    if (!seek(user)) this->append(xClient(user));
}

QString xClientList::NewUserId()
{
    int max=first().iGetUserId();
    for (auto it:*this) if (max<it.iGetUserId()) max=it.iGetUserId();
    return QString(max+1);
}
