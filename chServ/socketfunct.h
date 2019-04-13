#ifndef SOCKETFUNCT_H
#define SOCKETFUNCT_H

#include "QThread"
#include "QTimer"
#include "QDateTime"
#include "QJsonObject"
#include "QJsonDocument"
#include "QWebSocket"
#include "QDebug"
#include "QEventLoop"
//#include "server.h"
#include "client.h"

QString jsonToString(QJsonObject json);
QJsonObject jsonFromString(const QString& in);

class xSocketClient : public QObject{
    Q_OBJECT
private:
    bool msgRes;
    QTimer *timer;
    QWebSocket *socket;
    QString msg;
    xClient *client=NULL;
    xClientList *list;
public:
    QMutex mutex;
    xSocketClient(xClientList *list, QWebSocket * socket);

    void processCmd(QJsonObject & js);
    ~xSocketClient();

signals:
    void _msgRes ();
    void loopExit();
    void sendMsg(QWebSocket *, QString);
    void timerStart(int );
    void timerStop();


public slots:
    void start ();
    void SocketMsg(QString msg);
    QJsonObject CallFunct(QString functName, QJsonObject args);
    void timeOUT();
};



#endif // SOCKETFUNCT_H
