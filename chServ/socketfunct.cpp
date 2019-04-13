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

xSocketClient::xSocketClient(xClientList *list, QWebSocket *socket): socket(socket),list(list),mutex(QMutex::Recursive) {
    timer = new QTimer;
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeOUT()));
    connect(this, SIGNAL(timerStart(int )), timer,SLOT(start(int )));
    connect(this, SIGNAL(timerStop()), timer,SLOT(stop()));
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()), Qt::DirectConnection);
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
        qDebug () << CallFunct("SetChOnBoard", client->Pos->toJson());
    }
}

xSocketClient::~xSocketClient()
{
    mutex.lock();
    emit timerStop();
    timer->deleteLater();
    mutex.unlock();
}


QJsonObject xSocketClient::CallFunct(QString functName, QJsonObject args)
{
    QJsonObject js;
    js["cmd"]="Call";
    js["funct"]=functName;
    js["args"]=args;
    msgRes=false;
    emit timerStart(20);
    qDebug () <<this << js;
    emit sendMsg(socket, jsonToString(js));
    QEventLoop  loop;
    QMetaObject::Connection conn =  connect(this, SIGNAL(loopExit()), &loop, SLOT(quit()));
    loop.exec();
    disconnect(conn);
    qDebug  () << "loop exit";
    if (msgRes) {return jsonFromString(msg);}
    QJsonObject ret;
    ret["error"]="time_out";
    return ret;
}

void xSocketClient::timeOUT()
{
    mutex.lock();
    emit loopExit();
    mutex.unlock();
}



void xSocketClient::SocketMsg(QString msg)
{
    mutex.lock();
    qDebug () << QThread::currentThread();
    emit timerStop();
    QJsonObject js=jsonFromString(msg);
    qDebug () <<this<<js;
    processCmd(js);
    if (js["complete"]=="yes") {this->msg=msg; msgRes=true;}
    mutex.unlock();
    qDebug () << "xSocketThread::SocketMsg --- unlock";
    emit loopExit();
}
