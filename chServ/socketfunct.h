#ifndef SOCKETFUNCT_H
#define SOCKETFUNCT_H

#include "QThread"
#include "QTimer"
#include "QDateTime"
#include "QJsonObject"
#include "QJsonDocument"
#include "QWebSocket"
#include "QDebug"

QString jsonToString(QJsonObject json);
QJsonObject jsonFromString(const QString& in);
class xWaitMsg : public QThread
{
    int _timeOut;
    bool receivedMsg;
    Q_OBJECT
public:
    xWaitMsg(int _timeOut ) : _timeOut(_timeOut)
    {
    }
    void run()
    {
        auto startT=QDateTime::currentSecsSinceEpoch();
        receivedMsg=false;
        while (QDateTime::currentSecsSinceEpoch()-startT<_timeOut)
            if (receivedMsg) {qDebug() <<"msg!"; break;}
        exec();
    }
public slots:
    void resMsg()
    {
        receivedMsg=true;
    }

};

class xSocketThread : public QThread {
    Q_OBJECT
private:

    bool msgRes;
    QString msg;
    xWaitMsg * waitClient;
    QWebSocket *socket;
public:
    xSocketThread(QWebSocket *socket ) : socket(socket)
    {
        waitClient = new xWaitMsg(5);
        connect(socket, &QWebSocket::textMessageReceived,
                this, &xSocketThread::SocketMsg);
        connect(this, &xSocketThread::_msgRes, waitClient,  &xWaitMsg::resMsg);
    }

    void run()
    {
       QJsonObject js;
        auto res=CallFunct("GetId", js);
        qDebug () << res;
        exec();
    }
    QJsonObject CallFunct(QString functName, QJsonObject args)
    {
        QJsonObject js;
        js["cmd"]="Call";
        js["funct"]=functName;
        js["args"]=args;
        msgRes=false;
        waitClient->start();
        emit sendMsg(jsonToString(js));
        waitClient->wait();
        if (msgRes) return jsonFromString(msg);
        QJsonObject ret;
        ret["error"]="time_out";
        return ret;
    }

signals:
    void _msgRes ();
    void sendMsg(QString);

public slots:
    void SocketMsg(QString msg)
    {
        this->msg=msg; msgRes=true;
        emit _msgRes();
        QJsonObject js=jsonFromString(msg);
        if (js["funct"]=="functName") {this->msg=msg; msgRes=true;}
    }
};



#endif // SOCKETFUNCT_H
