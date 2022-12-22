#ifndef MY_TCPSERVER_H
#define MY_TCPSERVER_H
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMutex>
#include <QTimer>
#include "CJsonObject.h"

class my_tcpserver : public QObject
{
    Q_OBJECT
public:
    explicit my_tcpserver(QObject *parent = nullptr);
    bool start_listen();
signals:

public slots:
    void server_new_connection();
    void server_read_data();
private:

private:
    QTcpServer *m_pTcpServer;
    QTcpSocket *m_pTcpSocket;
    QTimer *m_pTimer;
    QVector <QString> m_qvec;
    char * m_pPicBuf;
};

#endif // MY_TCPSERVER_H
