#ifndef HKCAMERA_H
#define HKCAMERA_H

#include <QObject>
#include "HCNetSDK.h"
#include "commheader.h"
class HKCamera : public QObject
{
    Q_OBJECT
public:
    explicit HKCamera(QObject *parent = nullptr);
    void setParam(QString ip,int port,QString user,QString pw);
    int loginCam();
    int getVideo(QString checkTime,QString videoPath);
    int getNVRConfig();
signals:

public slots:
private:
    QString current_ip;
    int current_port;
    QString current_user;
    QString current_pw;
    LONG lUserID;
    STRU_DEVICE_INFO m_struDeviceInfo;
    int current_chan;
};

#endif // HKCAMERA_H
