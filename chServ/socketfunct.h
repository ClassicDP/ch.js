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

class xThread : public QThread
 {
 public:
     void run()  {
         exec();
     }
 };


QString jsonToString(QJsonObject json);
QJsonObject jsonFromString(const QString& in);

class xSocketClient : public QObject{
    Q_OBJECT
private:
    bool msgRes;
//    QTimer *timer;
    QWebSocket *socket;
    QString msg;
    xClient *client=NULL;
    xClientList *list;
public:
    QMutex msgMutex, cmdMutex, tmrMutex;
    xSocketClient(xClientList *list, QWebSocket * socket);

    void processCmd(QJsonObject & js);
    ~xSocketClient();

signals:
    void _msgRes ();
    void loopExit();
    void sendMsg(QWebSocket *, QString);
    void timerStart(int );
    void timerStop();
    void finished();
    void error(QString);


public slots:
    void start ();
    void SocketMsg(QString msg);
    void disconnect();
    QJsonObject CallFunct(QString functName, QJsonObject args);
    void timeOUT();
};



#endif // SOCKETFUNCT_H
