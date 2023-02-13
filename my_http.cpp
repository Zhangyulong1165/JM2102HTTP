#include "my_http.h"
#include "Logger.h"
#include <QDebug>
#include <QNetworkReply>
#include <QFile>
#include "sqllitehandle.h"
extern int isRelease;
extern SqlLiteHandle *m_pSqliteHandle;
extern QList <ReplyData *> reply_list;
extern QList <ReplyData *> reply_overDataList;
extern QMutex *m_pReplyMutex;


my_http::my_http(QObject *parent) : QObject(parent)
{
}
void my_http::sendHttp(QNetworkAccessManager *m_pManager,QNetworkRequest &weight,QNetworkRequest &pic,JmVehSqlInfo *info)
{
    //重量数据内容
    WeightJsonInfo weight_info;
    weight_info.checkNo =info->checkNo;
    weight_info.siteCode =info->siteCode;
    weight_info.siteName =info->siteName;
    weight_info.siteType =info->siteType;
    weight_info.checkType =info->checkType;
    weight_info.lane =info->lane;
    weight_info.vehicleType =info->vehicleType;
    if(0==isRelease)
    {
        weight_info.vehicleType =1;
        qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
        int month =qrand()%(10);
        month = month>0?month:1;
        int day =qrand()%(31);
        day =day>0?day:1;
        int hour=qrand()%(25);
        int minute=qrand()%(60);
        int second=qrand()%(60);
        weight_info.checkTime.sprintf("%d-%02d-%02d %02d:%02d:%02d",2021,month,day,hour,minute,second);
    }
    else {
        weight_info.checkTime =info->checkTime;
    }
    weight_info.plateNum =info->plateNum;
    weight_info.plateNumColor =info->plateNumColor;
    weight_info.plateNumType =info->plateNumType;
    weight_info.direction =info->direction;
    weight_info.speed =info->speed;
    weight_info.axleCnt =info->axleCnt;
    weight_info.axleType =info->axleType;
    weight_info.sumWeight =info->sumWeight;
    weight_info.limitWeight =info->limitWeight;
    weight_info.overWeight =info->overWeight;
    weight_info.overRate =info->overRate;
    weight_info.width =info->width;
    weight_info.limitWidth =info->limitWidth;
    weight_info.overWidth =info->overWidth;
    weight_info.overWidthRate =info->overWidthRate;
    weight_info.height =info->height;
    weight_info.limitHeight =info->limitHeight;
    weight_info.overHeight =info->overHeight;
    weight_info.overHeightRate =info->overHeightRate;
    weight_info.length =info->length;
    weight_info.limitLength =info->limitHeight;
    weight_info.overLength =info->overLength;
    weight_info.overLengthRate =info->overLengthRate;
    weight_info.reverse =info->reverse;
    sendWeightJson(m_pManager,weight,weight_info);
    //图片数据内容
    PicJsonInfo pic_info;
    pic_info.checkNo =info->checkNo;
    pic_info.siteCode =info->siteCode;
    pic_info.siteName =info->siteName;
    pic_info.siteType =info->siteType;
    pic_info.overRate =info->overRate;
    pic_info.lane =info->lane;
    pic_info.direction =info->direction;
    pic_info.snapTime =info->snapTime;
    pic_info.transType =2;
    //车牌小图
    if(!info->frontPlatePicPath.isEmpty())
    {
        pic_info.fileType =11;
        QFile m_file(info->frontPlatePicPath);
        QByteArray file_array;
        if(m_file.open(QFile::ReadOnly))
        {
            file_array =m_file.readAll();
            pic_info.fileContent =file_array.toBase64();
            pic_info.postfix ="jpg";
            m_file.close();
        }
        sendPicJson(m_pManager,pic,pic_info);
        file_array.clear();
    }
    //前抓拍
    if(!info->frontPicPath.isEmpty())
    {
        pic_info.fileType =12;
        QFile m_file(info->frontPicPath);
        QByteArray file_array;
        if(m_file.open(QFile::ReadOnly))
        {
            file_array =m_file.readAll();
            pic_info.fileContent =file_array.toBase64();
            pic_info.postfix ="jpg";
            m_file.close();
        }
        sendPicJson(m_pManager,pic,pic_info);
        file_array.clear();
    }
    //后抓拍
    if(!info->backPicPath.isEmpty())
    {
        pic_info.fileType =13;
        QFile m_file(info->backPicPath);
        QByteArray file_array;
        if(m_file.open(QFile::ReadOnly))
        {
            file_array =m_file.readAll();
            pic_info.fileContent =file_array.toBase64();
            pic_info.postfix ="jpg";
            m_file.close();
        }
        sendPicJson(m_pManager,pic,pic_info);
        file_array.clear();
    }
    //侧抓拍
    if(!info->sidePicPath.isEmpty())
    {
        pic_info.fileType =14;
        QFile m_file(info->sidePicPath);
        QByteArray file_array;
        if(m_file.open(QFile::ReadOnly))
        {
            file_array =m_file.readAll();
            pic_info.fileContent =file_array.toBase64();
            pic_info.postfix ="jpg";
            m_file.close();
        }
        sendPicJson(m_pManager,pic,pic_info);
        file_array.clear();
    }
    //顶抓拍
    //    if(!info.topPicPath.isEmpty())
    //    {
    //        pic_info.fileType =13;
    //        QFile m_file(info.topPicPath);
    //        QByteArray file_array;
    //        if(m_file.open(QFile::ReadOnly))
    //        {
    //            file_array =m_file.readAll();
    //            pic_info.fileContent =file_array.toBase64();
    //            pic_info.postfix ="jpg";
    //        }
    //        sendPicJson(pic_info);
    //    }
    //全景小视频
    if(!info->videoPath.isEmpty())
    {
        pic_info.fileType =21;
        QFile m_file(info->videoPath);
        QByteArray file_array;
        if(m_file.open(QFile::ReadOnly))
        {
            file_array =m_file.readAll();
            pic_info.fileContent =file_array.toBase64();
            pic_info.postfix ="mp4";
            m_file.close();
        }
        sendPicJson(m_pManager,pic,pic_info);
        file_array.clear();
    }
    //m_pSqliteHandle->set_is_up(weight_info.checkNo,0);
}

void my_http::sendHttp_overData(QNetworkAccessManager *m_pManager, QNetworkRequest &overData, Overlimitdata *overData_info)
{

    //发送超限数据
        sendOverDataJson(m_pManager,overData,overData_info);
}

void my_http::sendWeightJson(QNetworkAccessManager *m_pManager,QNetworkRequest &weight,WeightJsonInfo &weight_info)
{
    QByteArray postData;
    QJsonObject json_obj;
    json_obj.insert("checkNo",weight_info.checkNo);
    json_obj.insert("siteCode",weight_info.siteCode);
    json_obj.insert("siteName",weight_info.siteName);
    json_obj.insert("siteType",weight_info.siteType);
    json_obj.insert("checkType",weight_info.checkType);
    json_obj.insert("lane",weight_info.lane);
    json_obj.insert("vehicleType",weight_info.vehicleType);
    json_obj.insert("checkTime",weight_info.checkTime);
    json_obj.insert("plateNum",weight_info.plateNum);
    json_obj.insert("plateNumColor",weight_info.plateNumColor);
    json_obj.insert("plateNumType",weight_info.plateNumType);
    json_obj.insert("direction",weight_info.direction);
    json_obj.insert("speed",weight_info.speed);
    json_obj.insert("axleCnt",weight_info.axleCnt);
    json_obj.insert("axleType",weight_info.axleType);
    json_obj.insert("sumWeight",weight_info.sumWeight);
    json_obj.insert("limitWeight",weight_info.limitWeight);
    json_obj.insert("overWeight",weight_info.overWeight);
    json_obj.insert("overRate",weight_info.overRate);
    json_obj.insert("reverse",weight_info.reverse);
    QJsonDocument send_json(json_obj);
    postData.append(send_json.toJson());
    QNetworkReply *reply=m_pManager->post(weight,postData);
    ReplyData* reply_data =new ReplyData();
    reply_data->fileType =1;
    reply_data->checkNo =weight_info.checkNo;
    reply_data->isFinish =0;
    reply_data->reply =reply;
    reply_data->timeT =QDateTime::currentDateTime().toTime_t();
    m_pReplyMutex->lock();
    reply_list.push_back(reply_data);
    m_pReplyMutex->unlock();
    logDebug(postData.data());
}
void my_http::sendPicJson(QNetworkAccessManager *m_pManager,QNetworkRequest &pic,PicJsonInfo &pic_info)
{
    QByteArray postData;
    QJsonObject json_obj;
    json_obj.insert("checkNo",pic_info.checkNo);
    json_obj.insert("siteCode",pic_info.siteCode);
    json_obj.insert("siteName",pic_info.siteName);
    json_obj.insert("siteType",pic_info.siteType);
    json_obj.insert("overRate",pic_info.overRate);
    json_obj.insert("transType",pic_info.transType);
    json_obj.insert("fileType",pic_info.fileType);
    json_obj.insert("lane",pic_info.lane);
    json_obj.insert("direction",pic_info.direction);
    json_obj.insert("snapTime",pic_info.snapTime);
    json_obj.insert("fileContent",pic_info.fileContent);
    json_obj.insert("postfix",pic_info.postfix);
    QJsonDocument send_json(json_obj);
    postData.append(send_json.toJson());
    QNetworkReply *reply =m_pManager->post(pic,postData);
    ReplyData* reply_data =new ReplyData();
    reply_data->checkNo =pic_info.checkNo;
    reply_data->isFinish =0;
    reply_data->reply =reply;
    reply_data->fileType =pic_info.fileType;
    reply_data->timeT =QDateTime::currentDateTime().toTime_t();
    m_pReplyMutex->lock();
    reply_list.push_back(reply_data);
    m_pReplyMutex->unlock();
    logDebug(postData.data());
}

void my_http::sendOverDataJson(QNetworkAccessManager *m_pManager, QNetworkRequest &overData, Overlimitdata *overData_info)
{
    QByteArray postData;
    QJsonObject json_obj;
    json_obj.insert("cph",overData_info->jm_cph.toStdString().c_str());
    json_obj.insert("zs",overData_info->jm_zs.toStdString().c_str());
    json_obj.insert("zsm",overData_info->jm_zsm.toStdString().c_str());
    json_obj.insert("zlzz",overData_info->jm_zlzz.toStdString().c_str());
    json_obj.insert("cxl",overData_info->jm_cxl.toStdString().c_str());
    json_obj.insert("deptid",overData_info->jm_deptid.toStdString().c_str());
    json_obj.insert("cxll",overData_info->jm_cxll.toStdString().c_str());
    json_obj.insert("vehicle_Length",overData_info->jm_vehicle_Length.toStdString().c_str());
    json_obj.insert("vehicle_Width",overData_info->jm_vehicle_Width.toStdString().c_str());
    json_obj.insert("vehicle_Height",overData_info->jm_vehicle_Height.toStdString().c_str());
    json_obj.insert("vehicle_Speed",overData_info->jm_vehicle_Speed.toStdString().c_str());
    json_obj.insert("clsyr",overData_info->jm_clsyr.toStdString().c_str());
    json_obj.insert("jm_pry_no",overData_info->jm_pry_no.toStdString().c_str());
    json_obj.insert("by1zz",overData_info->jm_by1zz.toStdString().c_str());
    json_obj.insert("illegalpictureurl1",overData_info->jm_illegalpictureurl1.toStdString().c_str());
    json_obj.insert("illegalpictureurl2",overData_info->jm_illegalpictureurl2.toStdString().c_str());
    json_obj.insert("illegalpictureurl3",overData_info->jm_illegalpictureurl3.toStdString().c_str());
    json_obj.insert("illegalpictureurl4",overData_info->jm_illegalpictureurl4.toStdString().c_str());
    json_obj.insert("illegalvideo5",overData_info->jm_illegalvideo5.toStdString().c_str());
    json_obj.insert("illegaltime",overData_info->jm_illegaltime.toStdString().c_str());
    json_obj.insert("illegalstation",overData_info->jm_illegalstation.toStdString().c_str());
    json_obj.insert("stationno",overData_info->jm_stationno.toStdString().c_str());
    json_obj.insert("owneraddr",overData_info->jm_owneraddr.toStdString().c_str());
    json_obj.insert("ownertel",overData_info->jm_ownertel.toStdString().c_str());
    json_obj.insert("passno",overData_info->jm_passno.toStdString().c_str());
    json_obj.insert("passline",overData_info->jm_passline.toStdString().c_str());
    json_obj.insert("passstarttime",overData_info->jm_passstarttime.toStdString().c_str());
    json_obj.insert("passendtime",overData_info->jm_passendtime.toStdString().c_str());
    json_obj.insert("passweight",overData_info->jm_passweight.toStdString().c_str());
    json_obj.insert("passgoods",overData_info->jm_passgoods.toStdString().c_str());
    json_obj.insert("schecknO",overData_info->jm_schecknO.toStdString().c_str());
    json_obj.insert("hpzl",overData_info->jm_hpzl.toStdString().c_str());
    json_obj.insert("roadid",overData_info->jm_roadid.toStdString().c_str());
    json_obj.insert("stake_km",overData_info->jm_stake_km.toStdString().c_str());
    json_obj.insert("stake_m",overData_info->jm_stake_m.toStdString().c_str());
    json_obj.insert("waydir",overData_info->jm_hight_waydir.toStdString().c_str());

    QJsonDocument send_json(json_obj);
    postData.append(send_json.toJson());
    QNetworkReply *reply=m_pManager->post(overData,postData);
    ReplyData* reply_data =new ReplyData();
    reply_data->checkNo =overData_info->jm_cph;
    reply_data->isFinish =0;
    reply_data->reply =reply;
    reply_data->timeT =QDateTime::currentDateTime().toTime_t();

    m_OverDataReplyQMutex.lock();
    reply_overDataList.push_back(reply_data);
    m_OverDataReplyQMutex.unlock();


}
void my_http::m_ReplyFinished(QNetworkReply *reply)
{
    QByteArray array =reply->readAll();
    if(array.isEmpty())
    {
        return;
    }
    qDebug()<<array;
    QJsonParseError m_json_error;
    QJsonValue value;
    QJsonDocument recv_json =QJsonDocument::fromJson(array,&m_json_error);
    if(m_json_error.error == QJsonParseError::NoError)
    {
        if(recv_json.isObject())
        {
            QString checkNo;
            int code =-1;
            QString appmsg;
            int type =-1;

            QString overDataApi;
            QString overDataApiMSG;
            QJsonObject obj =recv_json.object();
            if(obj.contains("appcode"))
            {
                value =obj.take("appcode");
                qDebug()<<value.toVariant();
                code =value.toVariant().toInt();
                qDebug()<<"code:"<<code;
            }
            if(obj.contains("appmsg"))
            {
                value =obj.take("appmsg");
                qDebug()<<value.toVariant();
                appmsg =value.toVariant().toString();
                qDebug()<<"appmsg:"<<appmsg;
            }
            if(obj.contains("appdata"))
            {
                value =obj.take("appdata");
                qDebug()<<"appdata"<<value.toVariant();
                qDebug()<<"appdata:"<<value;
            }
            if(obj.contains("apptype"))
            {
                value =obj.take("apptype");
                type =value.toVariant().toInt();
                qDebug()<<"apptype:"<<type;
            }
            if(obj.contains("appcheckno"))
            {
                value =obj.take("appcheckno");
                checkNo=value.toVariant().toString();
                qDebug()<<"checkNo:"<<checkNo;

            }
            /// 添加超限数据反馈 Added By ZhangYL 2023-01-11

            if(obj.contains("RESULT_CODE"))
            {
                value =obj.take("RESULT_CODE");
                overDataApi=value.toVariant().toString();
                qInfo()<<"***zhangYL***->"<<"overDataApi:"<<overDataApi;
            }
            if(obj.contains("RESULT_MSG"))
            {
                value =obj.take("RESULT_MSG");
                overDataApiMSG=value.toVariant().toString();
                qInfo()<<"***zhangYL***->"<<"overDataApiMSG:"<<overDataApiMSG;

            }
            if(1==code)
            {
                m_pReplyMutex->lock();
                for(int i =0;i<reply_list.size();i++)
                {
                    if((0==reply_list.at(i)->checkNo.compare(checkNo))&&(type==reply_list.at(i)->fileType))
                    {
                        reply_list.at(i)->isFinish =1;
                    }
                }
                m_pReplyMutex->unlock();
            }
            if(code==1&&type==12)
            {
                m_pSqliteHandle->set_is_up(checkNo,type);
                qInfo()<<"+++++++设置上传插入点++++++++++\n";
                qInfo()<<"***zhangYL***->"<<"检测单号:"<<checkNo<<endl;
                //检测单号符合超限存入
                emit sendCheckNo(checkNo);
                //m_ZYLTools->judgeRecord_isOk(checkNo);
            }
        }
        else {
            qDebug()<<"recv_json 不是 object "<<m_json_error.error;
        }
    }
    else {
        qDebug()<<"json 格式错误"<<m_json_error.error;
    }
}
