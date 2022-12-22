#ifndef MY_MATCH_THREAD_H
#define MY_MATCH_THREAD_H
#include <QThread>
#include "commheader.h"
class my_match_thread : public QThread
{
public:
    my_match_thread();
    void run();
private:
    void VecToJmVehSqlInfo(JmVehSqlInfo &info,RecvVecData* data);
    void CamToJmVehSqlInfo(JmVehSqlInfo &info,RecvCamData* data);
    void PicToJmVehSqlInfo(JmVehSqlInfo &info,RecvPicData* data);
};

#endif // MY_MATCH_THREAD_H
