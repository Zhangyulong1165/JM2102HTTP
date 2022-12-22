#ifndef MY_HTTP_H
#define MY_HTTP_H
#include <QObject>
#include <QNetworkAccessManager>
#include "commheader.h"
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
//class QNetworkReply;
class my_http : public QObject
{
    Q_OBJECT
public:
    explicit my_http(QObject *parent = nullptr);
    void sendHttp(QNetworkAccessManager *m_pManager,QNetworkRequest &weight,QNetworkRequest &pic,JmVehSqlInfo *info);
    void sendWeightJson(QNetworkAccessManager *m_pManager,QNetworkRequest &weight,WeightJsonInfo &weight_info);
    void sendPicJson(QNetworkAccessManager *m_pManager,QNetworkRequest &pic,PicJsonInfo &pic_info);
    void recv_json();
signals:

public slots:
    void m_ReplyFinished(QNetworkReply *reply);
};

#endif // MY_HTTP_H
