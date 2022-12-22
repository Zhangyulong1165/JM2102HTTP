#include "mainwindow.h"
#include "Logger.h"
#include "commheader.h"
#include "sqllitehandle.h"
#include "my_http.h"
#include <QList>
#include <QApplication>
#include <QSettings>
QString logPath ="F:\\jinmai\\log";
QString picPath ="F:\\jinmai\\pic";
QString areaId;     //区县编号
QString siteCode;   //站点编号
QString siteName;   //站点名称
//21：超限检测站31:非现场检测点;41:高速公路入口检测;42:高速公路出口检测; 71:重点源头企业
int siteType=0;
//站点类型为21时 检测类型必填 检测类型: 1：初检； 2：复检
int checkType=0;
//匹配超时时间
int matchTimeout=10;
QString vKey;
int listenPort =6789;
QSettings *m_pSetting;
QMutex *m_pRecvWeightMutex;
QQueue <RecvVecData *> weight_vec;
QMutex *m_pRecvCamMutex;
QQueue <RecvCamData *> cam_vec;
//前抓拍图片
QMutex *m_pRecvPicMutex;
QQueue <RecvPicData *> pic_vec;
QMutex *m_pRecvMutex;
QByteArray recv_array;
QList <ReplyData *> reply_list;
QMutex *m_pReplyMutex;
//非现场最小包长度
int min_recv_len =78;
//图片类型
int platePic=0;
int frontPic=0;
int backPic=0;
int sidePic=0;
int topPic=0;
int globleVideo=0;
int isRelease =1;
SqlLiteHandle *m_pSqliteHandle;
//http 重量和图片URL
QString weightUrl;
QString picUrl;
my_http* m_pHttp;
int reverseSource =2;
int localCacheTime =30;
QQueue <JmVehSqlInfo *> resultVec;
QMutex *m_pSendMutex;
int reply_timeout =10;
//硬盘录像机类型 0没有小视频 1海康硬盘录像机  2大华硬盘录像机
int NVRType=1;
QString NVRIp;
int NVRPort;
QString NVRUser;
QString NVRPassword;
int beforTime=5;
int afterTime=5;
QString globalCam1Ip;
int globalCam1Dir=1;
QString globalCam2Ip;
int globalCam2Dir=2;
int max_weight=100000;
int min_weight=5000;
int init_config()
{
    m_pSetting =new QSettings("C:\\jm2102\\config.ini",QSettings::IniFormat);
    //站点信息
    m_pSetting->beginGroup("/station");
    areaId =m_pSetting->value("/areaId").toString();
    siteCode =m_pSetting->value("/siteCode").toString();
    siteName =m_pSetting->value("/siteName").toString();
    isRelease=m_pSetting->value("/isRelease").toInt();
    listenPort=m_pSetting->value("/listenPort").toInt();
    siteType =m_pSetting->value("/siteType").toInt();
    checkType =m_pSetting->value("/checkType").toInt();
    matchTimeout =m_pSetting->value("/matchTimeout").toInt();
    platePic =m_pSetting->value("/PlatePic").toInt();
    frontPic =m_pSetting->value("/frontPic").toInt();
    backPic =m_pSetting->value("/backPic").toInt();
    sidePic =m_pSetting->value("/sidePic").toInt();
    topPic =m_pSetting->value("/topPic").toInt();
    globleVideo =m_pSetting->value("/globleVideo").toInt();
    vKey =m_pSetting->value("/vKey").toString();
    logPath =m_pSetting->value("/logPath").toString();
    picPath =m_pSetting->value("/picPath").toString();
    weightUrl =m_pSetting->value("/weightUrl").toString();
    picUrl =m_pSetting->value("/picUrl").toString();
    reverseSource =m_pSetting->value("/reverseSource").toInt();
    reply_timeout =m_pSetting->value("/reply_timeout").toInt();
    //NVR参数
    NVRType =m_pSetting->value("/NVRType").toInt();
    NVRIp =m_pSetting->value("/NVRIp").toString();
    NVRPort =m_pSetting->value("/NVRPort").toInt();
    NVRUser =m_pSetting->value("/NVRUser").toString();
    NVRPassword =m_pSetting->value("/NVRPassword").toString();
    beforTime =m_pSetting->value("/beforTime").toInt();
    afterTime =m_pSetting->value("/afterTime").toInt();
    globalCam1Ip =m_pSetting->value("/globalCam1Ip").toString();
    globalCam1Dir =m_pSetting->value("/globalCam1Dir").toInt();
    globalCam2Ip =m_pSetting->value("/globalCam2Ip").toString();
    globalCam2Dir =m_pSetting->value("/globalCam2Dir").toInt();
    min_weight =m_pSetting->value("/min_weight").toInt();
    max_weight =m_pSetting->value("/max_weight").toInt();
    qDebug()<<"开始启动......";
    qDebug()<<weightUrl;
    qDebug()<<picUrl;
    m_pSetting->endGroup();
}
int main(int argc, char *argv[])
{
    char logInfo[256] ={0};
    qInstallMessageHandler(outputMessage);
    init_config();
    long ret =0;
    m_pRecvWeightMutex =new QMutex();
    m_pRecvCamMutex = new QMutex();
    m_pRecvPicMutex =new QMutex();
    m_pRecvMutex =new QMutex();
    m_pReplyMutex =new QMutex();
    m_pSqliteHandle =new SqlLiteHandle();
    m_pSendMutex =new QMutex();
    m_pHttp =new my_http();
    resultVec.clear();
    qDebug()<<logPath;
    qDebug()<<picPath;
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowIcon(QIcon("C:/jm2102/weight_pic/jinmai_icon.png"));
    w.setWindowTitle(QString::fromUtf8("今迈管理程序"));
    w.show();
    return a.exec();
}
