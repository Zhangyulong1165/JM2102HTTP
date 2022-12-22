//#include "my_send_thread.h"
//#include "sqllitehandle.h"
//#include <QNetworkReply>
//#include <QNetworkProxy>
////extern my_http *m_pHttp;
//extern SqlLiteHandle *m_pSqliteHandle;
//my_send_thread::my_send_thread()
//{

//}
//void my_send_thread::run()
//{
//    //send_test();
////    my_http* m_pHttp =new my_http();
////    QNetworkAccessManager *m_pManager =new QNetworkAccessManager();
////    connect(m_pManager,SIGNAL(finished(QNetworkReply*)),m_pHttp,SLOT(m_ReplyFinished(QNetworkReply*)));
////    QNetworkRequest  m_RequestWeight;
////    QNetworkRequest  m_RequestPic;
////    m_RequestWeight.setUrl(QUrl(weightUrl));
////    m_RequestWeight.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=utf-8");
////    m_RequestPic.setUrl(QUrl(picUrl));
////    m_RequestPic.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=utf-8");
////    vector <JmVehSqlInfo> info;
////    info =m_pSqliteHandle->select_sql_not_up();
////    if(info.empty())
////    {
////        sleep(3);
////    }
////    for (int i=0;i<info.size();i++)
////    {
////        m_pHttp->sendHttp(m_pManager,m_RequestWeight,m_RequestPic,info.at(i));
////        qDebug()<<"sendHttp ok";
////    }
////    return;
////    while(true)
////    {
////        vector <JmVehSqlInfo> info;
////        info =m_pSqliteHandle->select_sql_not_up();
////        if(info.empty())
////        {
////            sleep(3);
////            continue;
////        }
////        for (int i=0;i<info.size();i++)
////        {
////            m_pHttp->sendHttp(m_pManager,m_RequestWeight,m_RequestPic,info.at(i));
////        }
////        break;
////    }
//}
//void my_send_thread::m_ReplyFinished(QNetworkReply *reply)
//{

//}
//void my_send_thread::send_test()
//{

//}
