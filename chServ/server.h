
#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <QtCore/QObject>
#include <QtCore/QList>
#include <QJsonObject>
#include <QJsonDocument>
#include <QWebSocket>
#include <QThread>
#include <QMutex>
#include "socketfunct.h"
#include "game.h"


QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)
QT_FORWARD_DECLARE_CLASS(QString)



//class _client: public QObject {
//    Q_OBJECT
//public:
//    QWebSocket * socket; QThread * thread;
//    _client(QWebSocket * socket, QThread * thread);
//    bool operator ==(const _client & a);
//    ~_client ();
//signals:
//    void quitThread();
//};

class Server : public QObject
{
    Q_OBJECT
    QWebSocket *pSocket;
    xClientList *list;
    QMutex mutex;
public:
    explicit Server(quint16 port, QObject *parent = nullptr);
    ~Server() override;

private slots:
    void onNewConnection();
    void processMessage(const QString &message);
//    _client * searchBySocket(QWebSocket *socket) ;
    void sendMessage(QWebSocket * socket, QString msg);
    void socketDisconnected();

private:
    QWebSocketServer *m_pWebSocketServer;
//    QList<_client *> m_clients;
};

#endif //CHATSERVER_H
