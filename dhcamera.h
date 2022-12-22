#ifndef DHCAMERA_H
#define DHCAMERA_H

#include <QObject>

class DHCamera : public QObject
{
    Q_OBJECT
public:
    explicit DHCamera(QObject *parent = nullptr);
    void setParam(QString ip,int port,QString user,QString pw);
    int loginCam();
signals:

public slots:
private:
    QString current_ip;
    int current_port;
    QString current_user;
    QString current_pw;
    //STRU_DEVICE_INFO m_struDeviceInfo;
};

#endif // DHCAMERA_H
