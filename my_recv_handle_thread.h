#ifndef MY_RECV_HANDLE_THREAD_H
#define MY_RECV_HANDLE_THREAD_H
#include <QThread>
#include "commheader.h"
class my_recv_handle_thread : public QThread
{
public:
    my_recv_handle_thread();
    void run();
private:
    int handle_weight(QByteArray weight_array);
    int handle_pic_header(QByteArray array);
    int handle_pic_info(QByteArray array);
    int handle_camera(QByteArray array);
    void illegalCode_to_reverse(string code,int &reverse);
};

#endif // MY_RECV_HANDLE_THREAD_H
