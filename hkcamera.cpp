#include "hkcamera.h"
#include "Logger.h"
#include <QDateTime>
extern int beforTime;
extern int afterTime;
extern QString globalCam1Ip;
HKCamera::HKCamera(QObject *parent) : QObject(parent)
{

}
void HKCamera::setParam(QString ip,int port,QString user,QString pw)
{
    current_ip =ip;
    current_port =port;
    current_user =user;
    current_pw =pw;
    current_chan =0;
}
int HKCamera::loginCam()
{
    char logInfo[256] ={0};
    NET_DVR_DEVICEINFO_V30 *m_pInfoTmp =new NET_DVR_DEVICEINFO_V30();
    memset(m_pInfoTmp,0,sizeof(NET_DVR_DEVICEINFO_V30));
    lUserID =NET_DVR_Login_V30((char*)current_ip.toStdString().c_str(),current_port,(char*)current_user.toStdString().c_str(),(char*)current_pw.toStdString().c_str(),m_pInfoTmp);
    if(lUserID==-1)
    {
        sprintf(logInfo,"ip:%s port:%d login error %d",current_ip.toStdString().c_str(),current_port,NET_DVR_GetLastError());
        logDebug(logInfo);
        return -1;
    }
    else {
        sprintf(logInfo,"ip:%s port:%d login success",current_ip.toStdString().c_str(),current_port);
        logDebug(logInfo);
        return  0;
    }
}
int HKCamera::getNVRConfig()
{
    char logInfo[128] ={0};
    NET_DVR_IPPARACFG_V40 IpAccessCfg;
    memset(&IpAccessCfg,0,sizeof(IpAccessCfg));
    DWORD  dwReturned;
    int ret = NET_DVR_GetDVRConfig(lUserID,NET_DVR_GET_IPPARACFG_V40,0,&IpAccessCfg,sizeof(NET_DVR_IPPARACFG_V40),&dwReturned);
    sprintf(logInfo,"NET_DVR_GetDVRConfig ret %d",ret);
    logDebug(logInfo);
    int sumChan =IpAccessCfg.dwDChanNum;
    int startChan =IpAccessCfg.dwStartDChan;
    for(int i=0;i<sumChan;i++)
    {
        QString ip;
        ip.sprintf("%s",IpAccessCfg.struIPDevInfo[i].struIP.sIpV4);
        if(0==ip.compare(globalCam1Ip))
        {
            current_chan=startChan+i;
            break;
        }
    }
    sprintf(logInfo,"current_chan == %d",current_chan);
    logDebug(logInfo);
}
int HKCamera::getVideo(QString checkTime,QString videoPath)
{
    char logInfo[256] ={0};
    unsigned int current_time_t =QDateTime::fromString(checkTime,"yyyy-MM-dd hh:mm:ss").toTime_t();
    unsigned int befor_timeT =current_time_t-beforTime;
    unsigned int after_timeT =current_time_t+afterTime;
    QDateTime befor_time =QDateTime::fromTime_t(befor_timeT);
    QDateTime after_time =QDateTime::fromTime_t(after_timeT);
    NET_DVR_TIME StartTime;
    StartTime.dwYear =befor_time.date().year();
    StartTime.dwMonth=befor_time.date().month();
    StartTime.dwDay =befor_time.date().day();
    StartTime.dwHour =befor_time.time().hour();
    StartTime.dwMinute =befor_time.time().minute();
    StartTime.dwSecond =befor_time.time().second();
    NET_DVR_TIME StopTime;
    StopTime.dwYear =after_time.date().year();
    StopTime.dwMonth=after_time.date().month();
    StopTime.dwDay =after_time.date().day();
    StopTime.dwHour =after_time.time().hour();
    StopTime.dwMinute =after_time.time().minute();
    StopTime.dwSecond =after_time.time().second();
    int chanIndex =0;
    LONG m_lDownloadHandle = NET_DVR_GetFileByTime(lUserID,current_chan,&StartTime,&StopTime,(char*)videoPath.toStdString().c_str());
    if(m_lDownloadHandle == -1)
    {
        sprintf(logInfo,"NET_DVR_GetFileByTime error %d",NET_DVR_GetLastError());
        logDebug(logInfo);
        return -1;
    }
    if(NET_DVR_PlayBackControl(m_lDownloadHandle,NET_DVR_PLAYSTART,0,NULL))
    {
        int nPos = 0;
        for(nPos = 0; nPos < 100&&nPos>=0; nPos = NET_DVR_GetDownloadPos(m_lDownloadHandle))
        {
            Sleep(1000);  //millisecond
        }
        if(!NET_DVR_StopGetFile(m_lDownloadHandle))
        {
            NET_DVR_Logout(lUserID);
            NET_DVR_Cleanup();
            return -1;
        }
        if(nPos<0||nPos>100)
        {
            return -2;
        }
        return 0;
    }
}
