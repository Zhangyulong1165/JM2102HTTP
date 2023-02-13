#ifndef MY_HTTP_H
#define MY_HTTP_H
#include <QObject>
#include <QNetworkAccessManager>
#include "commheader.h"
#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMutex>
//class QNetworkReply;
class my_http : public QObject
{
    Q_OBJECT
public:
    explicit my_http(QObject *parent = nullptr);
    void sendHttp(QNetworkAccessManager *m_pManager,QNetworkRequest &weight,QNetworkRequest &pic,JmVehSqlInfo *info);
    ///  Added By ZhangYL 2023-01-11
    void sendHttp_overData(QNetworkAccessManager *m_pManager,QNetworkRequest &overData,Overlimitdata *overData_info);

    void sendWeightJson(QNetworkAccessManager *m_pManager,QNetworkRequest &weight,WeightJsonInfo &weight_info);

    void sendPicJson(QNetworkAccessManager *m_pManager,QNetworkRequest &pic,PicJsonInfo &pic_info);

    ///组装超限数据 Added By ZhangYL 2023-01-11
    void sendOverDataJson(QNetworkAccessManager *m_pManager,QNetworkRequest &overData,Overlimitdata *overData_info);

    void recv_json();
signals:
    void sendCheckNo(QString checkNo);
public slots:
    void m_ReplyFinished(QNetworkReply *reply);

private:
    QMutex m_OverDataReplyQMutex;
};

#endif // MY_HTTP_H
