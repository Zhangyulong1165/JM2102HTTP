#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "commheader.h"
#include <QDebug>
#include <QFile>
extern SqlLiteHandle *m_pSqliteHandle;
extern QString weightUrl;
extern QString picUrl;
extern int localCacheTime;
extern QQueue<JmVehSqlInfo *> resultVec;
extern QQueue<Overlimitdata *> OverVec;
extern QMutex *m_pSendMutex;
extern QList <ReplyData *> reply_list;
extern QMutex *m_pReplyMutex;
extern int reply_timeout;
extern int NVRType;
extern QString NVRIp;
extern int NVRPort;
extern QString NVRUser;
extern QString NVRPassword;
extern int NVRChanIndex;
extern int beforTime;
extern int afterTime;
extern QString globalCam1Ip;
extern int globalCam1Dir;
extern QString globalCam2Ip;
extern int globalCam2Dir;
//STRU_DEVICE_INFO m_struDeviceInfo[MAX_DEVICE];
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_Bridge = new Bridge();
    char logInfo[256] ={0};
    int ret =0;
    ret =m_pSqliteHandle->connectSql();
    if(0==ret)
    {
        sprintf_s(logInfo,"connectSql success");
        logDebug(logInfo);
    }
    else {
        sprintf_s(logInfo,"connectSql error  %d",ret);
        logCritical(logInfo);
    }
    m_pHttp =new my_http();
    m_pManager =new QNetworkAccessManager();
    connect(m_pManager,SIGNAL(finished(QNetworkReply*)),m_pHttp,SLOT(m_ReplyFinished(QNetworkReply*)));
    m_RequestWeight.setUrl(QUrl(weightUrl));
    m_RequestWeight.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=utf-8");
    m_RequestPic.setUrl(QUrl(picUrl));
    m_RequestPic.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=utf-8");
    ///  Added By ZhangYL 2023-01-11
    m_RequestOverData.setUrl(QUrl(picUrl));
    m_RequestOverData.setHeader(QNetworkRequest::ContentTypeHeader,"application/json;charset=utf-8");


    m_pMyTcpServer =new my_tcpserver();
    m_pRecvThread =new my_recv_handle_thread();
    m_pMatchThread =new my_match_thread();
    //发送数据定时器
    m_pSendTimer =new QTimer();
    connect(m_pSendTimer,SIGNAL(timeout()),this,SLOT(send_data()));
    m_pRecvThread->start();
    m_pMyTcpServer->start_listen();
    m_pMatchThread->start();
    m_pSendTimer->start(3000);
    m_pDeleteTimer =new QTimer();
    connect(m_pDeleteTimer,SIGNAL(timeout()),this,SLOT(delete_data()));
    //m_pDeleteTimer->start(1000);

    m_pReplyTimer =new QTimer();
    connect(m_pReplyTimer,SIGNAL(timeout()),this,SLOT(reply_data()));
    m_pReplyTimer->start(5000);
    //截取小视频定时器
    /// 添加接受新增检测编号 Added By ZhangYL 2023-01-11
    connect(m_pHttp,&my_http::sendCheckNo,this,[=](QString checkNo){
        if(checkNo.isEmpty()){
            qInfo()<<"***zhangYL***->接受新增检测编号为空"<<endl;
            return;
        }
        m_Bridge->m_ZYLTools->judgeRecord_isOk(checkNo);
    });
    connect(m_Bridge->m_ZYLTools,&ZYLTools::send_isVideoUp,this,[=](){
        //符合上传条件
        sendOverdata();
    });
    connect(ui->pbn_testUp,&QPushButton::clicked,this,[=](){
    m_Bridge->m_ZYLTools->send_isVideoUp();
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::send_data()
{
    m_pSqliteHandle->select_sql_not_up();
    m_pSendMutex->lock();
    while(!resultVec.isEmpty())
    {
        JmVehSqlInfo *info =resultVec.dequeue();
        if(info->frontPicPath.isEmpty())
        {
            char logInfo[128] ={0};
            sprintf_s(logInfo,"frontPicPath is empty not send");
            logDebug(logInfo);
        }
        else {
            m_pHttp->sendHttp(m_pManager,m_RequestWeight,m_RequestPic,info);
        }
        delete info;
    }
    m_pSendMutex->unlock();
}

void MainWindow::sendOverdata()
{
    m_pSendMutex->lock();
    while(!OverVec.isEmpty())
    {
        Overlimitdata *info =OverVec.dequeue();
        if(info->jm_zs.isEmpty())
        {
            //char logInfo[128] ={0};
            qInfo()<<"***zhangYL***->上传数据轴数异常暂不上传";
        }
        else {
            m_pHttp->sendHttp_overData(m_pManager,m_RequestOverData,info);
        }
        delete info;
    }
    m_pSendMutex->unlock();

}
void MainWindow::reply_data()
{
    m_pReplyMutex->lock();
    unsigned int current_time_t =QDateTime::currentDateTime().toTime_t();
    foreach(auto item,reply_list)
    {
        if((item->isFinish==1)||(current_time_t-item->timeT>reply_timeout))
        {
            item->reply->deleteLater();
            delete item;
            reply_list.removeOne(item);
        }
    }
    m_pReplyMutex->unlock();
}
void MainWindow::delete_data()
{
    char logInfo[256] ={0};
    QFile file("F:\\1.jpg");
    if(file.open(QFile::ReadOnly))
    {
        sprintf(logInfo,"open %s ok",file.fileName().toLatin1().data());
        logDebug(logInfo);
        if(file.remove())
        {
            sprintf(logInfo,"remove ok");
            logDebug(logInfo);
        }
        else {
            sprintf(logInfo,"remove error");
            logDebug(logInfo);
        }
    }
    else {
        sprintf(logInfo,"open %s error",file.fileName().toStdString().c_str());
        logDebug(logInfo);
    }
    //vector <JmVehSqlInfo> info;
//    m_pSqliteHandle->select_sql_isup();
//    for (int i=0;i<info.size();i++)
//    {
//        unsigned int current_time_t =QDateTime::currentDateTime().toTime_t();
//        if(current_time_t-info.at(i).timeT<localCacheTime*24*60*60)
//        {
//            break;
//        }
//        if(!info.at(i).frontPlatePicPath.isEmpty())
//        {
//            if(QFile::remove(info.at(i).frontPlatePicPath))
//            {

//            }
//        }
//        if(!info.at(i).frontPicPath.isEmpty())
//        {
//            if(QFile::remove(info.at(i).frontPicPath))
//            {

//            }
//        }
//        if(!info.at(i).backPicPath.isEmpty())
//        {
//            if(QFile::remove(info.at(i).backPicPath))
//            {

//            }
//        }
//        if(!info.at(i).sidePicPath.isEmpty())
//        {
//            if(QFile::remove(info.at(i).sidePicPath))
//            {

//            }
//        }
//    }
}
void MainWindow::delete_pic(QString path)
{

}
void MainWindow::on_pushButton_select_clicked()
{
    ui->textEdit_show->append("select");
    QString str;char logInfo[256] ={0};

//    QString ip =NVRIp;
//    int port =NVRPort;
//    QString user =NVRUser;
//    QString passwd =NVRPassword;
//    NET_DVR_DEVICEINFO_V30 *m_pInfoTmp =new NET_DVR_DEVICEINFO_V30();
//    memset(m_pInfoTmp,0,sizeof(NET_DVR_DEVICEINFO_V30));
//    LONG lUserID =NET_DVR_Login_V30((char*)ip.toStdString().c_str(),port,(char*)user.toStdString().c_str(),(char*)passwd.toStdString().c_str(),m_pInfoTmp);
//    str.sprintf("NET_DVR_Login_V30 ret == %ld err=%d",lUserID,NET_DVR_GetLastError());
//    ui->textEdit_show->append(str);
//    m_struDeviceInfo.lLoginID = lUserID;
//    m_struDeviceInfo.iDeviceChanNum =m_pInfoTmp->byChanNum;
//    m_struDeviceInfo.iIPChanNum = m_pInfoTmp->byIPChanNum;
//    m_struDeviceInfo.iStartChan  = m_pInfoTmp->byStartChan;
//    ui->textEdit_show->append(ip);
//    ui->textEdit_show->append(QString::number(port));
//    ui->textEdit_show->append(user);
//    ui->textEdit_show->append(passwd);
//    ui->textEdit_show->append(QString::number(m_struDeviceInfo.lLoginID));
//    ui->textEdit_show->append(QString::number(m_struDeviceInfo.iDeviceChanNum));
//    ui->textEdit_show->append(QString::number(m_struDeviceInfo.iIPChanNum));
//    ui->textEdit_show->append(QString::number(m_struDeviceInfo.iStartChan));

//    NET_DVR_IPPARACFG_V40 IpAccessCfg;
//    memset(&IpAccessCfg,0,sizeof(IpAccessCfg));
//    DWORD  dwReturned;
//    m_struDeviceInfo.bIPRet = NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_IPPARACFG_V40,0,&IpAccessCfg,sizeof(NET_DVR_IPPARACFG_V40),&dwReturned);
//    str.sprintf("NET_DVR_GetDVRConfig ret == %d",m_struDeviceInfo.bIPRet);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.dwSize == %d",IpAccessCfg.dwSize);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.dwAChanNum == %d",IpAccessCfg.dwAChanNum);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.dwDChanNum == %d",IpAccessCfg.dwDChanNum);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.dwGroupNum == %d",IpAccessCfg.dwGroupNum);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.dwStartDChan == %d",IpAccessCfg.dwStartDChan);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.wDVRPort == %d",IpAccessCfg.struIPDevInfo[0].wDVRPort);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.sUserName == %s",IpAccessCfg.struIPDevInfo[0].sUserName);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.szDeviceID == %s",IpAccessCfg.struIPDevInfo[0].szDeviceID);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.sIpV4 == %s",IpAccessCfg.struIPDevInfo[0].struIP.sIpV4);
//    ui->textEdit_show->append(str);
//    str.sprintf("IpAccessCfg.byIPv6 == %s",IpAccessCfg.struIPDevInfo[0].struIP.byIPv6);
//    ui->textEdit_show->append(str);
//    int i;
//    if(m_struDeviceInfo.bIPRet)
//    {
//        for(i=0; i<MAX_ANALOG_CHANNUM; i++)
//        {
//            if(i<m_struDeviceInfo.iIPChanNum)
//            {
//                m_struDeviceInfo.struChanInfo[i].iChanIndex=i+m_struDeviceInfo.iStartChan;  //通道号
//                m_struDeviceInfo.struChanInfo[i].bEnable = TRUE;
//            }
//        }
//    }
//    if(!m_struDeviceInfo.bIPRet)   //不支持ip接入,9000以下设备不支持禁用模拟通道
//    {
//        for(i=0; i<MAX_ANALOG_CHANNUM; i++)
//        {
//            if (i < m_struDeviceInfo.iDeviceChanNum)
//            {
//                sprintf(m_struDeviceInfo.struChanInfo[i].chChanName,"camera%d",i+m_struDeviceInfo.iStartChan);
//                m_struDeviceInfo.struChanInfo[i].iChanIndex=i+m_struDeviceInfo.iStartChan;  //通道号
//                m_struDeviceInfo.struChanInfo[i].bEnable = TRUE;

//            }
//            else
//            {
//                m_struDeviceInfo.struChanInfo[i].iChanIndex = -1;
//                m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
//                sprintf(m_struDeviceInfo.struChanInfo[i].chChanName, "");
//            }
//        }
//    }
//    else        //支持IP接入，9000设备
//    {
//        for(i=0; i<MAX_ANALOG_CHANNUM; i++)  //模拟通道
//        {
//            if (i < m_struDeviceInfo.iDeviceChanNum)
//            {
//                sprintf(m_struDeviceInfo.struChanInfo[i].chChanName,"camera%d",i+m_struDeviceInfo.iStartChan);
//                m_struDeviceInfo.struChanInfo[i].iChanIndex=i+m_struDeviceInfo.iStartChan;
//                if(IpAccessCfg.byAnalogChanEnable[i])
//                {
//                    m_struDeviceInfo.struChanInfo[i].bEnable = TRUE;
//                }
//                else
//                {
//                    m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
//                }

//            }
//            else//clear the state of other channel
//            {
//                m_struDeviceInfo.struChanInfo[i].iChanIndex = -1;
//                m_struDeviceInfo.struChanInfo[i].bEnable = FALSE;
//                sprintf(m_struDeviceInfo.struChanInfo[i].chChanName, "");
//            }
//        }

//        //数字通道
//        for(i=0; i<MAX_IP_CHANNEL; i++)
//        {
//            if(IpAccessCfg.struStreamMode[i].uGetStream.struChanInfo.byEnable)  //ip通道在线
//            {
//                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].bEnable = TRUE;
//                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].iChanIndex = IpAccessCfg.dwStartDChan+i;
//                sprintf(m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].chChanName,"IP Camera %d",i+1);

//            }
//            else
//            {
//                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].bEnable = FALSE;
//                m_struDeviceInfo.struChanInfo[i+MAX_ANALOG_CHANNUM].iChanIndex = -1;
//            }
//        }

//    }
//    NET_DVR_TIME StartTime;
//    StartTime.dwYear=2021;StartTime.dwMonth=12;StartTime.dwDay=3;
//    StartTime.dwHour=15;StartTime.dwMinute=20;StartTime.dwSecond=10;
//    NET_DVR_TIME StopTime;
//    StopTime.dwYear=2021;StopTime.dwMonth=12;StopTime.dwDay=3;
//    StopTime.dwHour=15;StopTime.dwMinute=20;StopTime.dwSecond=20;
//    str.sprintf("NVRChanIndex == %d",35);
////    ui->textEdit_show->append(str);
////    int iCurChannel =m_struDeviceInfo.struChanInfo[NVRChanIndex].iChanIndex;
////    str.sprintf("chAccessChanIP == %s",m_struDeviceInfo.struChanInfo[NVRChanIndex].chAccessChanIP);
////    ui->textEdit_show->append(str);
//    LONG LoginID =m_struDeviceInfo.lLoginID;
//    char fName[100]={0};
//    sprintf(fName,"E:\\download_%d%02d%02d%02d%02d%02d.mp4",StartTime.dwYear,StartTime.dwMonth,StartTime.dwDay,StartTime.dwHour,StartTime.dwMinute,StartTime.dwSecond);
//    LONG m_lDownloadHandle = NET_DVR_GetFileByTime(m_struDeviceInfo.lLoginID,35,&StartTime,&StopTime,fName);
//    if(m_lDownloadHandle == -1)
//    {
//        str.sprintf("NET_DVR_GetFileByTime error %d",NET_DVR_GetLastError());
//        ui->textEdit_show->append(str);
//        return;
//    }
//    if(NET_DVR_PlayBackControl(m_lDownloadHandle,NET_DVR_PLAYSTART,0,NULL))
//    {
//        int nPos = 0;
//        for(nPos = 0; nPos < 100&&nPos>=0; nPos = NET_DVR_GetDownloadPos(m_lDownloadHandle))
//        {
//            str.sprintf("Be downloading... %d %%",nPos);
//            ui->textEdit_show->append("download err");
//            Sleep(1000);  //millisecond
//        }
//        if(!NET_DVR_StopGetFile(m_lDownloadHandle))
//        {
//            str.sprintf("NET_DVR_StopGetFile err %d",NET_DVR_GetLastError());
//            ui->textEdit_show->append(str);
//            NET_DVR_Logout(lUserID);
//            NET_DVR_Cleanup();
//            return;
//        }
//        if(nPos<0||nPos>100)
//        {
//            str.sprintf("download err %d",NET_DVR_GetLastError());
//            ui->textEdit_show->append(str);
//            NET_DVR_Logout(lUserID);
//            NET_DVR_Cleanup();
//            return;
//        }

//    }
}
void MainWindow::get_video()
{
    if(1==NVRType)
    {

    }
    else if(2==NVRType)
    {

    }
}

void MainWindow::on_pushButton_setting_clicked()
{
    ui->textEdit_show->append("setting");
}

void MainWindow::on_pushButton_quit_clicked()
{
    ui->textEdit_show->append("quit");
    close();
}
