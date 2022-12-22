#include "my_recv_handle_thread.h"
#include "commheader.h"
#include "Logger.h"
#include "CJsonObject.h"
#include <QMutex>
#include <vector>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
using namespace neb;
extern QString vKey;
extern QMutex *m_pRecvWeightMutex;
extern QQueue <RecvVecData *> weight_vec;
extern QMutex *m_pRecvCamMutex;
extern QQueue <RecvCamData *> cam_vec;
extern QMutex *m_pRecvPicMutex;
extern QQueue <RecvPicData *> pic_vec;
extern QByteArray recv_array;
extern QMutex *m_pRecvMutex;
extern QString picPath;
extern int min_recv_len;
static QByteArray pic_buf;
//匹配标志，0代表没有新数据 1重量数据  2 cam数据  3图片数据
QMutex m_match_mutex;
int match_flag =0;
static RecvPicData current_pic_data;
my_recv_handle_thread::my_recv_handle_thread()
{

}

void my_recv_handle_thread::run()
{
    QByteArray buf;
    int pack_index =0;
    while(true)
    {
        m_pRecvMutex->lock();
        int recv_len =recv_array.length();
        //长度判断
        if(recv_len<min_recv_len)
        {
            m_pRecvMutex->unlock();
            continue;
        }
        //包头判断
        pack_index=0;
        if(0xAA!=(unsigned char)recv_array.at(pack_index++))
        {
            recv_array.remove(0,1);
            m_pRecvMutex->unlock();
            continue;
        }
        recv_len =recv_array.length();
        if(recv_len<min_recv_len)
        {
            m_pRecvMutex->unlock();
            continue;
        }
        int ver =(unsigned char)recv_array.at(pack_index++);
        if(0xAA==ver)
        {
            recv_array.remove(0,2);
            qDebug()<<"新连接注册包";
            recv_array.clear();
            buf.clear();
            m_pRecvMutex->unlock();
            continue;
        }
        //协议版本为1
        if(0x1==ver)
        {
            int header_len =(unsigned char)recv_array.at(pack_index++);
            pack_index+=header_len;
            long body_len =(unsigned char)recv_array.at(pack_index)+(unsigned char)recv_array.at(pack_index+1)*0x100+(unsigned char)recv_array.at(pack_index+2)*0x10000+(unsigned char)recv_array.at(pack_index+3)*0x1000000;
            pack_index+=4;
            //1--称重数据  2--图片头  3--图片内容 4摄像头信息
            int data_type =(unsigned char)recv_array.at(pack_index);
            if(pack_index+body_len+32<=recv_len)
            {
                QByteArray md5_data =recv_array.mid(0,pack_index+body_len);
                QByteArray read_md5 =recv_array.mid(pack_index+body_len,32);
                md5_data.append(vKey.toLatin1());
                QByteArray my_md5 =QCryptographicHash::hash(md5_data,QCryptographicHash::Md5);
                if(0==read_md5.compare(my_md5.toHex()))
                {
                    QByteArray data_body_info =recv_array.mid(pack_index+1,body_len-1);
                    if(1==data_type)
                    {
                        if(0==handle_weight(data_body_info))
                        {
                            m_match_mutex.lock();
                            match_flag =1;
                            m_match_mutex.unlock();
                        }
                    }
                    else if(2==data_type)
                    {
                        handle_pic_header(data_body_info);
                        m_match_mutex.lock();
                        match_flag =0;
                        m_match_mutex.unlock();
                    }
                    else if(3==data_type)
                    {
                        if(0==handle_pic_info(data_body_info))
                        {
                            m_match_mutex.lock();
                            match_flag =1;
                            m_match_mutex.unlock();
                        }
                    }
                    else if(4==data_type)
                    {
                        if(0==handle_camera(data_body_info))
                        {
                            m_match_mutex.lock();
                            match_flag =1;
                            m_match_mutex.unlock();
                        }
                    }
                }
                else {
                    qDebug()<<"MD5校验失败";
                }
                recv_array.remove(0,pack_index+body_len+32);
                m_pRecvMutex->unlock();
            }
            else {
                m_pRecvMutex->unlock();
            }
        }
        else {
            m_pRecvMutex->unlock();
        }
    }
}
int my_recv_handle_thread::handle_weight(QByteArray weight_array)
{
    CJsonObject cJson(weight_array.toStdString());
    if(cJson.IsEmpty())
    {
        qDebug()<<"cJson is empty";
        return -1;
    }
    CJsonObject data_json;
    cJson.Get("data",data_json);
    if(data_json.IsEmpty())
    {
        qDebug()<<"data_json is empty";
    }
    RecvVecData *m_pVec =new RecvVecData();
    data_json.Get("deviceId",m_pVec->deviceId);
    data_json.Get("stationId",m_pVec->stationId);
    data_json.Get("lane",m_pVec->lane);
    data_json.Get("speed",m_pVec->speed);
    data_json.Get("sumWeight",m_pVec->sumWeight);
    data_json.Get("axleCnt",m_pVec->axleCnt);
    data_json.Get("dateTime",m_pVec->dateTime);
    data_json.Get("matchNo",m_pVec->matchNo);
    data_json.Get("length",m_pVec->length);
    data_json.Get("reverse",m_pVec->reverse);
    logDebug((char*)data_json.ToString().c_str());
    QDateTime time = QDateTime::currentDateTime();
    m_pVec->timeT =time.toTime_t();
    m_pRecvWeightMutex->lock();
    weight_vec.enqueue(m_pVec);
    m_pRecvWeightMutex->unlock();
    return 0;

}
int my_recv_handle_thread::handle_pic_header(QByteArray array)
{
    CJsonObject data_json(array.toStdString());
    if(data_json.IsEmpty())
    {
        qDebug()<<"cJson is empty";
        return -1;
    }
    //RecvPicData *m_pPic =new RecvPicData();
    current_pic_data.isFinish =0;
    data_json.Get("matchNo",current_pic_data.matchNo);
    data_json.Get("size",current_pic_data.size);
    qDebug()<<"图片大小为:"<<current_pic_data.size;
    data_json.Get("direction",current_pic_data.direction);
    data_json.Get("cameraDir",current_pic_data.cameraDir);
    qDebug()<<"图片cameraDir为:"<<current_pic_data.cameraDir;
    data_json.Get("postfix",current_pic_data.postfix);
    current_pic_data.filePath.clear();
    current_pic_data.timeT=QDateTime::currentDateTime().toTime_t();
//    m_pRecvPicMutex->lock();
//    pic_vec.enqueue(m_pPic);
//    m_pRecvPicMutex->unlock();
    return 0;
}
int my_recv_handle_thread::handle_pic_info(QByteArray array)
{
    //m_pRecvPicMutex->lock();
    //RecvPicData *m_pPicHeader =pic_vec.last();
//    if(m_pPicHeader==nullptr)
//    {
//        qDebug()<<"无法此图片头";
//        m_pRecvPicMutex->unlock();
//        return -1;
//    }
    if(array.isEmpty())
    {
        return  -1;
    }
    pic_buf.append(array);
    if(pic_buf.length()!=current_pic_data.size)
    {
        //m_pRecvPicMutex->unlock();
        return -2;
    }
    QTime current_time =QTime::currentTime();
    QDate current_date =QDate::currentDate();
    QString pic;
    pic.sprintf("%s\\%d\\%d\\%d\\%d",picPath.toStdString().c_str(),current_date.year(),current_date.month(),current_date.day(),current_time.hour());
    QDir dir;
    if (!dir.exists(pic))
    {
        bool res = dir.mkpath(pic);
    }
    QString filePath;
    filePath.append(pic);
    pic.sprintf("\\%s_%d.jpg",current_pic_data.matchNo.c_str(),current_pic_data.cameraDir);
    filePath.append(pic);
    errno_t err;
    FILE *fSnapPic = nullptr;
    if ((err = fopen_s(&fSnapPic, filePath.toStdString().c_str(), "wb")) != 0)
    {
        qDebug()<<"Thread-Snap 无法打开此文件"<<filePath<<err;
    }
    else {
        fwrite(pic_buf.data(), pic_buf.length(), 1, fSnapPic);
        fclose(fSnapPic);
        m_pRecvPicMutex->lock();
        RecvPicData *m_pPic =new RecvPicData();
        m_pPic->matchNo =current_pic_data.matchNo;
        m_pPic->cameraDir =current_pic_data.cameraDir;
        m_pPic->size =current_pic_data.size;
        m_pPic->timeT =current_pic_data.timeT;
        m_pPic->postfix =current_pic_data.postfix;
        m_pPic->filePath =filePath.toStdString();
        m_pPic->direction =current_pic_data.direction;
        m_pPic->isFinish =1;
        pic_vec.enqueue(m_pPic);
        m_pRecvPicMutex->unlock();
    }
    //m_pRecvPicMutex->unlock();
    pic_buf.clear();
    return 0;
}
int my_recv_handle_thread::handle_camera(QByteArray array)
{
    CJsonObject data_json(array.toStdString());
    if(data_json.IsEmpty())
    {
        qDebug()<<"cJson is empty";
        return -1;
    }
    RecvCamData *m_pCam =new RecvCamData();
    m_pCam->reverse=0;
    data_json.Get("deviceId",m_pCam->deviceId);
    data_json.Get("cameraDir",m_pCam->cameraDir);
    data_json.Get("stationId",m_pCam->stationId);
    data_json.Get("plateNum",m_pCam->plateNum);
    QString plateNum=QString::fromLocal8Bit(m_pCam->plateNum.c_str());
    m_pCam->plateNum =plateNum.toStdString().c_str();
    data_json.Get("matchNo",m_pCam->matchNo);
    data_json.Get("entireBelieve",m_pCam->entireBelieve);
    data_json.Get("plateType",m_pCam->plateType);
    data_json.Get("plateColor",m_pCam->plateColor);
    data_json.Get("vehicleType",m_pCam->vehicleType);
    data_json.Get("color",m_pCam->color);
    data_json.Get("lane",m_pCam->lane);
    data_json.Get("dateTime",m_pCam->dateTime);
    data_json.Get("direction",m_pCam->direction);
    data_json.Get("dateTime",m_pCam->dateTime);
    data_json.Get("matchNo",m_pCam->matchNo);
    data_json.Get("illegalCode",m_pCam->illegalCode);
    data_json.Get("picNum",m_pCam->picNum);
    data_json.Get("vehicle_type",m_pCam->vehicleType);
    data_json.Get("reverse",m_pCam->reverse);
    data_json.Get("snapTime",m_pCam->snapTime);
    QDateTime time = QDateTime::currentDateTime();
    logDebug((char*)data_json.ToString().c_str());
    m_pCam->timeT =time.toTime_t();
    m_pRecvCamMutex->lock();
    cam_vec.enqueue(m_pCam);
    m_pRecvCamMutex->unlock();
    return 0;
}
void my_recv_handle_thread::illegalCode_to_reverse(string code,int &reverse)
{
    if(code.compare("1301"))
    {
        reverse =1;
    }
    else
    {
        reverse =0;
    }
}
