/**
* @projectName   JM2102_http
* @brief         摘要
* 中间类,创建
* @author        ZhangYL
* @date          2023-01-11
*/
#ifndef BRIDGE_H
#define BRIDGE_H

#include <QObject>
#include <QThread>
#include "zyltools.h"

class Bridge : public QObject
{
    Q_OBJECT
public:
    explicit Bridge(QObject *parent = nullptr);
    //公有成员变量

    //子类对象
    ZYLTools *m_ZYLTools;
    //子类线程
    QThread *m_ZYLTools_QThread;
private:


signals:

public slots:

private slots:

    //私有成员变量
private:

};

#endif // BRIDGE_H
