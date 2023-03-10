#ifndef SQLLITEHANDLE_H
#define SQLLITEHANDLE_H
#include <QtSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTableWidget>
#include "Logger.h"
#include "commheader.h"
class SqlLiteHandle
{
public:
    SqlLiteHandle();
    ~SqlLiteHandle();
    long connectSql();
    //车辆信息操作
    long insert_vehicle_information(JmVehSqlInfo &Veh);
    int insert_fxc_weight(RecvVecData *data);
    int insert_cam_weight(RecvCamData *data);
    int insert_pic_weight(RecvPicData *data);
    int set_is_up(QString checkNo,int type);
    void select_sql_not_up();
    void select_sql_isup();
    QString select_picPath(QString num);
    ///查询车辆轴数情况
    QString select_axleCntweight(QString checkNo);
    ///查询车辆实重情况
    QString select_totalweight(QString checkNo);
    ///查询车辆超重情况
    int select_overweight(QString checkNo);
    ///修改以上传到省厅的结果
    void set_isVideoUp(QString checkNo);
    int SelectOverlimitdata(QString checkNo);

public:
    QSqlDatabase database;
    QSqlQuery *m_pSqlHandle;
    QMutex *m_pSqlMutex;
};

#endif // SQLLITEHANDLE_H
