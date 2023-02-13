#ifndef ZYLTOOLS_H
#define ZYLTOOLS_H

#include <QObject>
#include "commheader.h"
#include <QTimer>
class ZYLTools : public QObject
{
    Q_OBJECT
public:
    explicit ZYLTools(QObject *parent = nullptr);
//判定记录是否符合要求,形参为订单号,通过订单号查询轴数和总重.
    void judgeRecord_isOk(QString str_checkNo);

signals:
    /// 发送上传信息信号
    void send_isVideoUp();
public slots:
private:
    int FtpUpLoadFile(QString path);
    ///获取随机数
    int getRandom(int min,int max);
    ///判断是否开始上数据,内部增加时间判断
    void judgeisUpReady(int n_listSize);
    /// 生成时间目录
    QString returnTimeDataPath();
    QTimer *m_QTimer_OverData;


};

#endif // ZYLTOOLS_H
