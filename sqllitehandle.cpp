#include "sqllitehandle.h"
#include <QApplication>
extern int intervalTime;
extern QString areaId;    
extern QString siteCode;   
extern QString siteName;   
extern int siteType;
extern int checkType;
extern int platePic;
extern int frontPic;
extern int backPic;
extern int sidePic;
extern int topPic;
extern int globleVideo;
extern int reverseSource;
extern QQueue<JmVehSqlInfo *> resultVec;
extern QMutex *m_pSendMutex;
SqlLiteHandle::SqlLiteHandle()
{
    m_pSqlMutex =new QMutex();
}
SqlLiteHandle::~SqlLiteHandle()
{
    database.close();
}
long SqlLiteHandle::connectSql()
{
    long ret =0;
    QString path = QDir::currentPath();
    QApplication::addLibraryPath(path+QString("\\plugins"));
    QPluginLoader loader(path+QString("\\plugins\\sqldrivers\\qsqlite.dll"));
    database = QSqlDatabase::addDatabase("QSQLITE","old_db");
    database.setDatabaseName("C:/jm2102/jm_sql.db");
    database.setUserName("root");
    database.setPassword("jm123456");
    if (!database.open())
    {
        qDebug() << "Error: Failed to connect database." << database.lastError();
        ret =-1;
    }
    else
    {
        qDebug() << "Succeed to connect database.";
        m_pSqlHandle =new QSqlQuery(database);
        ret =0;
    }
    return ret;
}
long SqlLiteHandle::insert_vehicle_information(JmVehSqlInfo &Veh)
{
    m_pSqlMutex->lock();
    char logInfo[256]={0};
    if(Veh.checkNo.isEmpty())
    {
        sprintf_s(logInfo,"????????");
        logWarning(logInfo);
        m_pSqlMutex->unlock();
        return -2;
    }
    QSqlQuery query(database);
    QDateTime current_date =QDateTime::currentDateTime();
    char sql[1024*4] = {0};
    sprintf(sql,"insert into jm_veh_information (checkNo,siteCode,siteName,siteType,checkType,lane,checkTime,plateNum,\
vehicleColor,plateNumType,direction,speed,axleCnt,axleType,sumWeight,limitWeight,\
overWeight,overRate,width,limitWidth,overWidth,overWidthRate,length,limitLength,overLength,overLengthRate,\
frontPlatePicPath,frontPicPath,backPicPath,sidePicPath,topPicPath,videoPath,isUp,isMatchFinished,insertTime) values \
('%s','%s','%s',%d,%d,%d,'%s','%s',%d,%d,%d,%d,%d,%d,%d,%d,\
%d,%.2f,%d,%d,%d,%.2f,%d,%d,%d,%.2f,'%s','%s','%s','%s','%s','%s',%d,%d,'%s')",
            Veh.checkNo.toStdString().c_str(),Veh.siteCode.toStdString().c_str(),Veh.siteName.toStdString().c_str(),Veh.siteType,\
            Veh.checkType,Veh.lane,Veh.checkTime.toStdString().c_str(),Veh.plateNum.toStdString().c_str(),\
            Veh.plateNumColor,Veh.plateNumType,Veh.direction,Veh.speed,Veh.axleCnt,Veh.axleType,Veh.sumWeight,Veh.limitWeight,\
            Veh.overWeight,Veh.overRate,Veh.width,Veh.limitWidth,Veh.overWidth,Veh.overWidthRate,Veh.length,Veh.limitLength,Veh.overLength,Veh.overLengthRate,\
            Veh.frontPlatePicPath.toStdString().c_str(),Veh.frontPicPath.toStdString().c_str(),Veh.backPicPath.toStdString().c_str(),\
            Veh.sidePicPath.toStdString().c_str(),Veh.topPicPath.toStdString().c_str(),Veh.videoPath.toStdString().c_str(),0,1,current_date.toString().toStdString().c_str());
    bool succ = query.exec(sql);
    logDebug(sql);
    if(succ)
    {
        m_pSqlMutex->unlock();
        return 0;
    }
    else
    {
        m_pSqlMutex->unlock();
        return -1;
    }
}

int SqlLiteHandle::insert_fxc_weight(RecvVecData *data)
{
    m_pSqlMutex->lock();
    char logInfo[1024*4]={0};
    if(data==nullptr)
    {
        m_pSqlMutex->unlock();
        return -2;
    }
    if(data->matchNo.empty())
    {
        m_pSqlMutex->unlock();
        return -2;
    }
    QSqlQuery query(database);
    char sql[1024*4] = {0};
    sprintf(sql,"select * from jm_veh_information where matchNo='%s'",data->matchNo.c_str());
    logDebug(sql);
    bool succ =query.exec(sql);
    if(succ)
    {
        sprintf_s(logInfo,"sql handle ok");
        logDebug(logInfo);
    }
    else
    {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
        m_pSqlMutex->unlock();
        return -1;
    }
    QTime current_time =QTime::currentTime();
    QDate current_date =QDate::currentDate();
    QString tmp;QString checkNo;
    tmp.sprintf("%d",data->lane);
    checkNo.append(areaId);
    checkNo.append("_");
    checkNo.append(siteCode);
    checkNo.append("_");
    checkNo.append(tmp);
    checkNo.append("_");
    tmp.sprintf("%d%02d%02d",current_date.year(),current_date.month(),current_date.day());
    checkNo.append(tmp);
    tmp.sprintf("%02d%02d%02d%03d",current_time.hour(),current_time.minute(),current_time.second(),current_time.msec());
    checkNo.append(tmp);
    query.last();
    int nRow = query.at();
    int limitWeight =0;
    switch (data->axleCnt)
    {
        case 2:
            limitWeight =18000;
            break;
        case 3:
            limitWeight =27000;
            break;
        case 4:
            limitWeight =36000;
            break;
        case 5:
            limitWeight =43000;
            break;
        case 6:
            limitWeight =49000;
            break;
        default:
            qDebug()<<"????:"<<data->axleCnt;
            limitWeight =49000;
            break;
    }
    qDebug()<<"sunWeight:"<<data->sumWeight;
    int overWeight =data->sumWeight-limitWeight;
    qDebug()<<"overWeight:"<<overWeight;
    overWeight =overWeight>0?overWeight:0;
    qDebug()<<"overWeight:"<<overWeight;
    double overRate =(double)overWeight/limitWeight*100;
    qDebug()<<"overRate:"<<overRate;
    if(nRow<0)
    {
        QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        sprintf(sql,"insert into jm_veh_information (checkNo,matchNo,siteCode,siteName,siteType,checkType,lane,checkTime,\
    speed,axleCnt,sumWeight,limitWeight,overWeight,overRate,reverse,isUp,isMatchFinished,insertTime,plateColor) values \
    ('%s','%s','%s','%s',%d,%d,%d,'%s',%d,%d,%d,%d,%d,%.2f,%d,%d,%d,'%s',%d)",checkNo.toStdString().c_str(),
                data->matchNo.c_str(),siteCode.toStdString().c_str(),siteName.toStdString().c_str(),siteType,checkType,\
                data->lane,data->dateTime.c_str(),data->speed,data->axleCnt,(int)data->sumWeight,limitWeight,overWeight,overRate,\
                data->reverse,0,1,current_date_time.toStdString().c_str(),0);
    }
    else
    {
        if(reverseSource==1)
        {
            sprintf(sql,"UPDATE jm_veh_information SET checkNo='%s',siteCode='%s',siteName='%s',siteType=%d,checkType=%d,lane=%d,\
                    checkTime='%s',speed=%d,axleCnt=%d,sumWeight=%d,limitWeight=%d,overWeight=%d,overRate=%.2f,reverse=%d WHERE matchNo='%s'",
                    checkNo.toStdString().c_str(),siteCode.toStdString().c_str(),siteName.toStdString().c_str(),siteType,checkType,data->lane,
                    data->dateTime.c_str(),data->speed,data->axleCnt,data->sumWeight,limitWeight,overWeight,
                    overRate,data->reverse,data->matchNo.c_str());
        }
        else {
            sprintf(sql,"UPDATE jm_veh_information SET checkNo='%s',siteCode='%s',siteName='%s',siteType=%d,checkType=%d,lane=%d,\
                    checkTime='%s',speed=%d,axleCnt=%d,sumWeight=%d,limitWeight=%d,overWeight=%d,overRate=%.2f WHERE matchNo='%s'",
                    checkNo.toStdString().c_str(),siteCode.toStdString().c_str(),siteName.toStdString().c_str(),siteType,checkType,data->lane,
                    data->dateTime.c_str(),data->speed,data->axleCnt,data->sumWeight,limitWeight,overWeight,
                    overRate,data->matchNo.c_str());
        }
    }
    logDebug(sql);
    succ = query.exec(sql);
    if(succ)
    {
//        sprintf_s(logInfo,"sql handle ok");
//        logDebug(logInfo);
    }
    else {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
        m_pSqlMutex->unlock();
        return  -1;
    }
    m_pSqlMutex->unlock();
    return 0;
}
int SqlLiteHandle::insert_cam_weight(RecvCamData *data)
{
    char logInfo[256]={0};
    if(data->cameraDir!=1)
    {
        return -2;
    }
    m_pSqlMutex->lock();
    if(data==nullptr)
    {
        m_pSqlMutex->unlock();
        return -2;
    }
    if(data->matchNo.empty())
    {
        m_pSqlMutex->unlock();
        return -2;
    }
    QSqlQuery query(database);
    char sql[1024*4] = {0};
    sprintf(sql,"select * from jm_veh_information where matchNo='%s'",data->matchNo.c_str());
    logDebug(sql);
    bool succ =query.exec(sql);
    if(succ)
    {
//        sprintf_s(logInfo,"sql handle ok");
//        logDebug(logInfo);
    }
    else
    {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
        m_pSqlMutex->unlock();
        return -1;
    }
    query.last();
    int nRow = query.at();
    if(nRow<0)
    {
        if(reverseSource==2)
        {
            QTime current_time =QTime::currentTime();
            QDate current_date =QDate::currentDate();
            QString tmp;QString checkNo;
            tmp.sprintf("%d",data->lane);
            checkNo.append(areaId);
            checkNo.append("_");
            checkNo.append(siteCode);
            checkNo.append("_");
            checkNo.append(tmp);
            checkNo.append("_");
            tmp.sprintf("%d%02d%02d",current_date.year(),current_date.month(),current_date.day());
            checkNo.append(tmp);
            tmp.sprintf("%02d%02d%02d%03d",current_time.hour(),current_time.minute(),current_time.second(),current_time.msec());
            checkNo.append(tmp);
            checkNo.append("_");
            tmp.sprintf("%02d",data->cameraDir);
            checkNo.append(tmp);
            QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
            sprintf(sql,"insert into jm_veh_information (checkNo,matchNo,siteCode,siteName,siteType,checkType,lane,checkTime,\
        plateNum,direction,plateColor,vehicleType,reverse,isUp,isMatchFinished,insertTime,snapTime) values \
                   ('%s','%s','%s','%s',%d,%d,%d,'%s','%s',%d,%d,%d,%d,%d,'%d','%s','%s')",
                    checkNo.toStdString().c_str(),data->matchNo.c_str(),siteCode.toStdString().c_str(),\
                    siteName.toStdString().c_str(),siteType,checkType,data->lane,data->dateTime.c_str(),\
                    data->plateNum.c_str(),data->direction,data->plateColor,data->vehicleType,data->reverse,0,0,current_date_time.toStdString().c_str(),data->snapTime.c_str());
        }


    }
    else
    {
        if(reverseSource==2)
        {
            sprintf(sql,"UPDATE jm_veh_information SET plateNum='%s',direction=%d,plateColor=%d,vehicleType =%d,reverse=%d,snapTime='%s' WHERE matchNo='%s'",
                    data->plateNum.c_str(),data->direction,data->plateColor,data->vehicleType,data->reverse,data->snapTime.c_str(),data->matchNo.c_str());
        }
        else {
            sprintf(sql,"UPDATE jm_veh_information SET plateNum='%s',direction=%d,plateColor=%d,vehicleType =%d,snapTime='%s' WHERE matchNo='%s'",
                    data->plateNum.c_str(),data->direction,data->plateColor,data->vehicleType,data->snapTime.c_str(),data->matchNo.c_str());
        }
    }
    logDebug(sql);
    succ = query.exec(sql);
    if(succ)
    {
//        sprintf_s(logInfo,"sql handle ok");
//        logDebug(logInfo);
    }
    else {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
        m_pSqlMutex->unlock();
        return  -1;
    }
    m_pSqlMutex->unlock();
    return 0;
}
int SqlLiteHandle::insert_pic_weight(RecvPicData *data)
{
    char logInfo[256]={0};
    m_pSqlMutex->lock();
    if(data==nullptr)
    {
        m_pSqlMutex->unlock();
        return -2;
    }
    if(data->matchNo.empty())
    {
        m_pSqlMutex->unlock();
        return -2;
    }
    if(data->filePath.empty())
    {
        sprintf_s(logInfo,"filePath is empty");
        logWarning(logInfo);
        m_pSqlMutex->unlock();
        return -2;
    }
    QSqlQuery query(database);
    char sql[1024*4] = {0};
    sprintf(sql,"select * from jm_veh_information where matchNo='%s'",data->matchNo.c_str());
    logDebug(sql);
    bool succ =query.exec(sql);
    if(succ)
    {
//        sprintf_s(logInfo,"sql handle ok");
//        logDebug(logInfo);
    }
    else
    {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
        m_pSqlMutex->unlock();
        return -1;
    }
    query.last();
    int nRow = query.at();
    if(nRow<0)
    {
        QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        switch (data->cameraDir)
        {
        case 21:
            sprintf(sql,"insert into jm_veh_information (matchNo,frontPlatePicPath,isUp,insertTime) values ('%s','%s',%d,'%s')",
            data->matchNo.c_str(),data->filePath.c_str(),0,current_date_time.toStdString().c_str());
            break;
        case 1:
            sprintf(sql,"insert into jm_veh_information (matchNo,frontPicPath,isUp,insertTime) values ('%s','%s',%d,'%s')",
            data->matchNo.c_str(),data->filePath.c_str(),0,current_date_time.toStdString().c_str());
            break;
        case 2:
            sprintf(sql,"insert into jm_veh_information (matchNo,backPicPath,isUp,insertTime) values ('%s','%s',%d,'%s')",
            data->matchNo.c_str(),data->filePath.c_str(),0,current_date_time.toStdString().c_str());
            break;
        case 3:
            sprintf(sql,"insert into jm_veh_information (matchNo,sidePicPath,isUp,insertTime) values ('%s','%s',%d,'%s')",
            data->matchNo.c_str(),data->filePath.c_str(),0,current_date_time.toStdString().c_str());
            break;
        case 4:
            sprintf(sql,"insert into jm_veh_information (matchNo,topPicPath,isUp,insertTime) values ('%s','%s',%d,'%s')",
            data->matchNo.c_str(),data->filePath.c_str(),0,current_date_time.toStdString().c_str());
            break;
        default:
            sprintf(logInfo,"camedir error:%d",data->cameraDir);
            break;
        }
    }
    else
    {
        switch (data->cameraDir)
        {
        case 21:
            sprintf(sql,"UPDATE jm_veh_information SET frontPlatePicPath='%s' WHERE matchNo='%s'",
data->filePath.c_str(),data->matchNo.c_str());
            break;
        case 1:
            sprintf(sql,"UPDATE jm_veh_information SET frontPicPath='%s' WHERE matchNo='%s'",
data->filePath.c_str(),data->matchNo.c_str());
            break;
        case 2:
            sprintf(sql,"UPDATE jm_veh_information SET backPicPath='%s' WHERE matchNo='%s'",
data->filePath.c_str(),data->matchNo.c_str());
            break;
        case 3:
            sprintf(sql,"UPDATE jm_veh_information SET sidePicPath='%s' WHERE matchNo='%s'",
data->filePath.c_str(),data->matchNo.c_str());
            break;
        case 4:
            sprintf(sql,"UPDATE jm_veh_information SET topPicPath='%s' WHERE matchNo='%s'",
data->filePath.c_str(),data->matchNo.c_str());
            break;
        default:
            sprintf(logInfo,"camedir error:%d",data->cameraDir);
            break;
        }
    }
    logDebug(sql);
    succ = query.exec(sql);
    if(succ)
    {
//        sprintf_s(logInfo,"sql handle ok");
//        logDebug(logInfo);
    }
    else {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
        m_pSqlMutex->unlock();
        return  -1;
    }
    m_pSqlMutex->unlock();
    return 0;
}
void SqlLiteHandle::select_sql_not_up()
{
    char logInfo[1024*4]={0};
    m_pSqlMutex->lock();
    QSqlQuery query(database);
    QString sql;
    QString sql1;
    sql1.sprintf("select * from jm_veh_information where isUp=%d AND plateNum!=''",0);
    sql.append(sql1);
    if(1==platePic)
    {
        sql1.sprintf(" AND frontPlatePicPath!=''");
        sql.append(sql1);
    }
    if(1==frontPic)
    {
        sql1.sprintf(" AND frontPicPath!=''");
        sql.append(sql1);
    }
    if(1==backPic)
    {
        sql1.sprintf(" AND backPicPath!=''");
        sql.append(sql1);
    }
    if(1==sidePic)
    {
        sql1.sprintf(" AND sidePicPath!=''");
        sql.append(sql1);
    }
    if(1==topPic)
    {
        sql1.sprintf(" AND topPicPath!=''");
        sql.append(sql1);
    }
    if(1==globleVideo)
    {
        sql1.sprintf(" AND videoPath!=''");
        sql.append(sql1);
    }
    sql.append(" ORDER BY checkTime DESC");
    sql.append(" LIMIT 10");
    logDebug(sql.toLatin1().data());
    bool succ =query.exec(sql);
    if(succ)
    {
//        sprintf_s(logInfo,"sql handle ok");
//        logDebug(logInfo);
    }
    else
    {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
    }
    QSqlRecord recode =query.record();
    while(query.next())
    {
        JmVehSqlInfo *info =new JmVehSqlInfo();
        info->checkNo =query.value("checkNo").toString();
        info->siteCode =query.value("siteCode").toString();
        info->siteName =query.value("siteName").toString();
        info->siteType =query.value("siteType").toInt();
        info->checkType =query.value("checkType").toInt();
        info->lane =query.value("lane").toInt();
        info->vehicleType =query.value("vehicleType").toInt();
        info->checkTime =query.value("checkTime").toString();
        info->plateNum =query.value("plateNum").toString();
        info->plateNumColor =query.value("plateColor").toInt();
        info->plateNumType =query.value("plateNumType").toInt();
        info->direction =query.value("direction").toInt();
        info->speed =query.value("speed").toInt();
        info->axleCnt =query.value("axleCnt").toInt();
        info->axleType =query.value("axleType").toInt();
        //??
        info->sumWeight =query.value("sumWeight").toInt();
        info->limitWeight =query.value("limitWeight").toInt();
        info->overWeight =query.value("overWeight").toInt();
        info->overRate =query.value("overRate").toDouble();
        //?
        info->width =query.value("width").toInt();
        info->limitWidth =query.value("limitWidth").toInt();
        info->overWidth =query.value("overWidth").toInt();
        info->overWidthRate =query.value("overWidthRate").toDouble();
        //?
        info->height =query.value("height").toInt();
        info->limitHeight =query.value("limitHeight").toInt();
        info->overHeight =query.value("overHeight").toInt();
        info->overHeightRate =query.value("overHeightRate").toDouble();
        //?
        info->length =query.value("length").toInt();
        info->limitLength =query.value("limitLength").toInt();
        info->overLength =query.value("overLength").toInt();
        info->overLengthRate =query.value("overLengthRate").toDouble();
        //????
        info->reverse =query.value("reverse").toInt();
        //??????
        info->frontPlatePicPath =query.value("frontPlatePicPath").toString();
        info->frontPicPath =query.value("frontPicPath").toString();
        info->backPicPath =query.value("backPicPath").toString();
        info->sidePicPath =query.value("sidePicPath").toString();
        info->topPicPath =query.value("topPicPath").toString();
        info->videoPath =query.value("videoPath").toString();
        info->snapTime =query.value("snapTime").toString();
        m_pSendMutex->lock();
        resultVec.enqueue(info);
        m_pSendMutex->unlock();
    }
    m_pSqlMutex->unlock();
}
void SqlLiteHandle::select_sql_isup()
{
    char logInfo[1024*4]={0};
    m_pSqlMutex->lock();
    QSqlQuery query(database);
    QString sql;
    QString sql1;
    sql1.sprintf("select * from jm_veh_information where isUp=%d",1);
    sql.append(sql1);
    sql.append(" ORDER BY checkTime ASC");
    sql.append(" LIMIT 10");
    logDebug(sql.toLatin1().data());
    bool succ =query.exec(sql);
    if(succ)
    {
//        sprintf_s(logInfo,"sql handle ok");
//        logDebug(logInfo);
    }
    else
    {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
        return;
    }
    QSqlRecord recode =query.record();
    while(query.next())
    {
        JmVehSqlInfo info;
        info.checkNo =query.value("checkNo").toString();
        info.siteCode =query.value("siteCode").toString();
        info.siteName =query.value("siteName").toString();
        info.siteType =query.value("siteType").toInt();
        info.checkType =query.value("checkType").toInt();
        info.lane =query.value("lane").toInt();
        info.checkTime =query.value("checkTime").toString();
        info.plateNum =query.value("plateNum").toString();
        info.plateNumType =query.value("plateNumType").toInt();
        info.direction =query.value("direction").toInt();
        info.speed =query.value("speed").toInt();
        info.axleCnt =query.value("axleCnt").toInt();
        info.axleType =query.value("axleType").toInt();
        info.plateNumColor =query.value("plateColor").toInt();
        //重量
        info.sumWeight =query.value("sumWeight").toInt();
        info.limitWeight =query.value("limitWeight").toInt();
        info.overWeight =query.value("overWeight").toInt();
        info.overRate =query.value("overRate").toDouble();
        //宽
        info.width =query.value("width").toInt();
        info.limitWidth =query.value("limitWidth").toInt();
        info.overWidth =query.value("overWidth").toInt();
        info.overWidthRate =query.value("overWidthRate").toDouble();
        //高
        info.height =query.value("height").toInt();
        info.limitHeight =query.value("limitHeight").toInt();
        info.overHeight =query.value("overHeight").toInt();
        info.overHeightRate =query.value("overHeightRate").toDouble();
        //长
        info.length =query.value("length").toInt();
        info.limitLength =query.value("limitLength").toInt();
        info.overLength =query.value("overLength").toInt();
        info.overLengthRate =query.value("overLengthRate").toDouble();
        //图片视频路径
        info.frontPlatePicPath =query.value("frontPlatePicPath").toString();
        info.frontPicPath =query.value("frontPicPath").toString();
        info.backPicPath =query.value("backPicPath").toString();
        info.sidePicPath =query.value("sidePicPath").toString();
        info.topPicPath =query.value("topPicPath").toString();
        info.videoPath =query.value("videoPath").toString();
        info.snapTime =query.value("snapTime").toString();
        //resultVec.push_back(info);
    }
    m_pSqlMutex->unlock();
    return;
}
int SqlLiteHandle::set_is_up(QString checkNo,int type)
{
    char logInfo[256]={0};
    m_pSqlMutex->lock();
    QSqlQuery query(database);
    char sql[1024] = {0};
    sprintf(sql,"select * from jm_veh_information where checkNo='%s'",checkNo.toStdString().c_str());
    logDebug(sql);
    bool succ =query.exec(sql);
    if(succ)
    {
//        sprintf_s(logInfo,"sql handle ok");
//        logDebug(logInfo);
    }
    else
    {
        sprintf_s(logInfo,"sql handle error %s",query.lastError().text().toStdString().c_str());
        logCritical(logInfo);
        m_pSqlMutex->unlock();
        return -1;
    }
    query.last();
    int nRow = query.at();
    if(nRow>=0)
    {
        sprintf(sql,"UPDATE jm_veh_information SET isUp=%d WHERE checkNo='%s'",1,checkNo.toStdString().c_str());
        succ = query.exec(sql);
    }
    m_pSqlMutex->unlock();
}
