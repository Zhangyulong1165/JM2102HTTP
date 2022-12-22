#include "my_match_thread.h"
#include "commheader.h"
#include "sqllitehandle.h"
#include "my_http.h"
#include <QMutex>
#include <vector>
#include <QDateTime>
#include <QDebug>
extern QString areaId;     //区县编号
extern QString siteCode;   //站点编号
extern QMutex *m_pRecvWeightMutex;
extern QQueue <RecvVecData *> weight_vec;
extern QMutex *m_pRecvCamMutex;
extern QQueue <RecvCamData *> cam_vec;
extern QMutex *m_pRecvPicMutex;
extern QQueue <RecvPicData *> pic_vec;
extern QString areaId;
extern QString siteCode;
extern QString siteName;
//21：超限检测站31:非现场检测点;41:高速公路入口检测;42:高速公路出口检测; 71:重点源头企业
extern int siteType;
//站点类型为21时 检测类型必填 检测类型: 1：初检； 2：复检
extern int checkType;
//匹配标志，0代表没有新数据 1重量数据  2 cam数据  3图片数据
extern int match_flag;
extern QMutex m_match_mutex;
//数据库操作
extern SqlLiteHandle *m_pSqliteHandle;
extern  my_http* m_pHttp;
my_match_thread::my_match_thread()
{

}
void my_match_thread::run()
{
    while(true)
    {
        m_match_mutex.lock();
        if(match_flag==0)
        {
            m_match_mutex.unlock();
            continue;
        }
        m_match_mutex.unlock();
        m_pRecvWeightMutex->lock();
        while(!weight_vec.isEmpty())
        {
            RecvVecData *data =weight_vec.dequeue();
            m_pSqliteHandle->insert_fxc_weight(data);
            delete data;
        }
        m_pRecvWeightMutex->unlock();
        m_pRecvCamMutex->lock();
        while(!cam_vec.isEmpty())
        {
            RecvCamData *data =cam_vec.dequeue();
            m_pSqliteHandle->insert_cam_weight(data);
            delete data;
        }
        m_pRecvCamMutex->unlock();
        m_pRecvPicMutex->lock();
        while(!pic_vec.isEmpty())
        {
            RecvPicData *data =pic_vec.dequeue();
            m_pSqliteHandle->insert_pic_weight(data);
            delete data;
        }
        m_pRecvPicMutex->unlock();
        m_match_mutex.lock();
        match_flag=0;
        m_match_mutex.unlock();
    }
}
void my_match_thread::VecToJmVehSqlInfo(JmVehSqlInfo &info,RecvVecData* data)
{
    if(data==nullptr)
    {
        qDebug()<<"RecvVecData is null";
        return ;
    }
    QTime current_time =QTime::currentTime();
    QDate current_date =QDate::currentDate();
    QString tmp;
    info.checkNo.append(areaId);
    info.checkNo.append("_");
    info.checkNo.append(siteCode);
    info.checkNo.append("_");
    tmp.sprintf("%d",data->lane);
    info.checkNo.append(tmp);
    info.checkNo.append("_");
    tmp.sprintf("%d%02d%02d",current_date.year(),current_date.month(),current_date.day());
    info.checkNo.append(tmp);
    tmp.sprintf("%02d%02d%02d%03d",current_time.hour(),current_time.minute(),current_time.second(),current_time.msec());
    info.checkNo.append(tmp);
    info.siteCode =siteCode;
    info.siteName =siteName;
    info.siteType =siteType;
    info.checkType=checkType;
    info.lane =data->lane;
    info.checkTime =QString::fromStdString(data->dateTime);
    info.speed =data->speed;
    info.axleCnt =data->axleCnt;
    info.axleType=data->axleType;
    info.sumWeight=data->sumWeight;
    switch (info.axleCnt)
    {
        case 2:
            info.limitWeight =18000;
            break;
        case 3:
            info.limitWeight =27000;
            break;
        case 4:
            info.limitWeight =36000;
            break;
        case 5:
            info.limitWeight =43000;
            break;
        case 6:
            info.limitWeight =49000;
            break;
        default:
            qDebug()<<"轴数异常:"<<info.axleCnt;
            break;
    }
    info.overWeight =info.sumWeight-info.limitWeight;
    info.overWeight =info.overWeight>0?info.overWeight:0;
    info.overRate =(double)info.overWeight/info.limitWeight*100;
}
void my_match_thread::CamToJmVehSqlInfo(JmVehSqlInfo &info,RecvCamData* data)
{
    if(data==nullptr)
    {
        qDebug()<<"CamToJmVehSqlInfo is null";
        return ;
    }
    info.plateNum =QString::fromStdString(data->plateNum);
    info.plateNumType =data->plateType;
    info.plateNumColor =data->plateColor;
}
void my_match_thread::PicToJmVehSqlInfo(JmVehSqlInfo &info,RecvPicData* data)
{
    if(data==nullptr)
    {
        qDebug()<<"PicToJmVehSqlInfo is null";
        return ;
    }
    switch (data->cameraDir)
    {
        case 21:
        info.frontPlatePicPath =QString::fromStdString(data->filePath);
        break;
        case 1:
        info.frontPicPath =QString::fromStdString(data->filePath);
        break;
        case 2:
        info.backPicPath =QString::fromStdString(data->filePath);
        break;
        case 3:
        info.sidePicPath =QString::fromStdString(data->filePath);
        break;
        case 4:
        info.topPicPath =QString::fromStdString(data->filePath);
        break;
        case 11:
        info.videoPath =QString::fromStdString(data->filePath);
        break;
        default:
        break;
    }
}
