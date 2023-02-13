#include "sqllitehandle.h"
#include "zyltools.h"

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>

extern QList<QString> qlist_totalUpData;
//数据库操作
extern SqlLiteHandle *m_pSqliteHandle;
extern int n_TimeInterval;
extern QString ftp_Url;
extern QString ftp_Port;
extern QString ftp_UserName;
extern QString ftp_Pwd;
extern QString overDataUrl;
ZYLTools::ZYLTools(QObject *parent) : QObject(parent)
{
    m_QTimer_OverData = new QTimer(this);
    //
    m_QTimer_OverData->setInterval(n_TimeInterval * 1000);
    connect(m_QTimer_OverData,&QTimer::timeout,this,[=](){
        if (!qlist_totalUpData.isEmpty()) {
            emit send_isVideoUp();
        }
    });
}

void ZYLTools::judgeRecord_isOk(QString str_checkNo)
{
    //判断订单号是否为空
    if(str_checkNo.isEmpty()){
        qInfo()<<"待检测的订单号为空"<<endl;
        qWarning()<<"待检测的订单号为空"<<endl;
        return;
    }
    //通过订单号查询订单详细信息
    if(m_pSqliteHandle->select_overweight(str_checkNo) > 0){
        //超重车辆
        qInfo()<<"符合超重信息";
        qlist_totalUpData.append(str_checkNo);
    }
    //判断队列个数是否符合上传要求
    judgeisUpReady(qlist_totalUpData.size());
    //符合条件的检测单号存入qlist_totalUpData

}

int ZYLTools::FtpUpLoadFile(QString path)
{
    QFile*file = new QFile(path);
    file->open(QIODevice::ReadOnly);
    QByteArray byte_file = file->readAll();

    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    accessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    QUrl url(QString("%1").arg(ftp_Url));//填服务器地址
    url.setPort(ftp_Port.toInt());
    url.setUserName(ftp_UserName); // ftp服务器 用户名
    url.setPassword(ftp_Pwd); //ftp 服务器密码

    QNetworkRequest request(url);
    //QNetworkReply*reply = accessManager->put(request, byte_file);
    //    connect(reply,&QNetworkReply::uploadProgress,[=](qint64 cur,qint64 total){
    //        //        ui->progressBar->setMaximum(total);
    //        //        ui->progressBar->setValue(cur);
    //    });
    connect(accessManager,&QNetworkAccessManager::finished,[=](){
        //        QMessageBox::information(this,"提示","上传完成!");
        return 0;
    });
    return 0;
}

int ZYLTools::getRandom(int min,int max)
{
    qsrand(QTime(0, 0, 0).secsTo(QTime::currentTime()));
    int num = qrand()%(max-min);
    return num;
}

void ZYLTools::judgeisUpReady(int n_listSize)
{
    int size = n_listSize;
    if( size>= 50 ){
        if(!m_QTimer_OverData->isActive()){
            qInfo()<<"***zhangYL***->上传总队的定时器 m_QTimer_OverData 首次启动";
            m_QTimer_OverData->start();
        }else {
            qInfo()<<"***zhangYL***->上传总队的定时器 m_QTimer_OverData 已经启动";
        }
        emit send_isVideoUp();
    }
}

QString ZYLTools::returnTimeDataPath()
{
    QString str_timeDataPath = nullptr;
    //取用时间
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd");
    str_timeDataPath =  current_date_time.insert(3,QString("/"));
    return str_timeDataPath;
}
