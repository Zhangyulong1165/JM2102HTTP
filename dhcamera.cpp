#include "dhcamera.h"

DHCamera::DHCamera(QObject *parent) : QObject(parent)
{

}
void DHCamera::setParam(QString ip,int port,QString user,QString pw)
{
    current_ip =ip;
    current_port =port;
    current_user =user;
    current_pw =pw;
}
int DHCamera::loginCam()
{

}
