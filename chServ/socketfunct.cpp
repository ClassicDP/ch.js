#include "socketfunct.h"


QString jsonToString(QJsonObject json)
{
    QJsonDocument Doc(json);
    QByteArray ba = Doc.toJson();
    return QString(ba);
}

QJsonObject jsonFromString(const QString &in)
{
    QJsonDocument jsonResponse = QJsonDocument::fromJson(in.toUtf8());
    QJsonObject jsonObject = jsonResponse.object();
    return jsonObject;
}

xSocketClient::xSocketClient(xClientList *list, QWebSocket *socket): socket(socket),list(list),msgMutex(QMutex::Recursive) {
//    timer = new QTimer;
//    timer->setSingleShot(true);
//    connect(timer, SIGNAL(timeout()), this, SLOT(timeOUT()));
//    connect(this, SIGNAL(timerStart(int )), timer,SLOT(start(int )));
//    connect(this, SIGNAL(timerStop()), timer,SLOT(stop()));
}

void xSocketClient::start()
{
    auto res=CallFunct("GetId", QJsonObject());
    qDebug () <<"1" << this << res;
    auto xx=list->seek(res["result"].toObject()["id"].toInt());
    if (xx<0 || true) {
        auto id=list->NewUserId();
        res["id"]=id;
        res=CallFunct("SetId",res);
        qDebug () <<"2" << this << res;
        if (client) {delete client;client=NULL;}
        client = new xClient(id);
        list->insert(client);
    } else {
        auto cli=list->at(xx);
        client=cli;}
    res=CallFunct("GetId", QJsonObject());
    qDebug () <<"3" << this << res;
    connect(client,&xClient::Call,this,&xSocketClient::CallFunct);
    res=CallFunct("SetChOnBoard", client->Pos->toJson());
    qDebug () <<"4" << this << res;

}


void xSocketClient::processCmd(QJsonObject &js)
{
    if (js["cmd"]=="loop") {
        cmdMutex.lock();
        qDebug () << CallFunct("SetChOnBoard", client->Pos->toJson());
        cmdMutex.unlock();
    }
}

xSocketClient::~xSocketClient()
{
//    timer->deleteLater();
}


QJsonObject xSocketClient::CallFunct(QString functName, QJsonObject args)
{
    QJsonObject js;
    js["cmd"]="Call";
    js["funct"]=functName;
    js["args"]=args;
    msgRes=false;
    qDebug () <<this << js;
    emit sendMsg(socket, jsonToString(js));
    QEventLoop  loop;
    connect(this, SIGNAL(loopExit()), &loop, SLOT(quit()));
    QTimer timer;
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeOUT()));
        connect(this, SIGNAL(timerStart(int )), &timer,SLOT(start(int )));
        connect(this, SIGNAL(timerStop()), &timer,SLOT(stop()));

    timer.setSingleShot(true);
    timer.start(1);
    loop.exec();
    qDebug  () << "loop exit";
    tmrMutex.lock();
    if (msgRes) {tmrMutex.unlock();return jsonFromString(msg);}
    tmrMutex.unlock();
    QJsonObject ret;
    ret["error"]="time_out";
    return ret;
}

void xSocketClient::timeOUT()
{
    tmrMutex.lock();
    emit loopExit();
    tmrMutex.unlock();
}



void xSocketClient::SocketMsg(QString msg)
{
    msgMutex.lock();
    qDebug () << QThread::currentThread();
    QJsonObject js=jsonFromString(msg);
    qDebug () <<this<<js;
    processCmd(js);
    if (js["complete"]=="yes") {
        tmrMutex.lock();
        emit timerStop();
        this->msg=msg; msgRes=true;
        tmrMutex.unlock();
        emit loopExit();
    }
    msgMutex.unlock();
    qDebug () << "xSocketThread::SocketMsg --- unlock";

}

void xSocketClient::disconnect()
{
    msgMutex.lock();
    msgMutex.unlock();
    tmrMutex.lock();
    tmrMutex.unlock();
    cmdMutex.lock();
    cmdMutex.unlock();
    emit finished();
}
