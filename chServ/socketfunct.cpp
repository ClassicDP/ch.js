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
