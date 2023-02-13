#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QNetworkAccessManager>
#include "my_http.h"
#include "my_tcpserver.h"
#include "my_recv_handle_thread.h"
#include "my_match_thread.h"
#include "sqllitehandle.h"
#include "bridge.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    my_tcpserver *m_pMyTcpServer;
    my_recv_handle_thread *m_pRecvThread;
    my_match_thread *m_pMatchThread;
    QTimer *m_pSendTimer;
    QTimer *m_pDeleteTimer;
    QTimer *m_pReplyTimer;
    my_http* m_pHttp;
    QNetworkAccessManager *m_pManager;
    QNetworkRequest  m_RequestWeight;
    QNetworkRequest  m_RequestPic;
    /// 超限数据 Added By ZhangYL 2023-01-11
    QNetworkRequest  m_RequestOverData;
    Bridge *m_Bridge;
private slots:
    void on_pushButton_select_clicked();
    void on_pushButton_setting_clicked();
    void on_pushButton_quit_clicked();
private slots:
    void send_data();
    /// 发送超限数据
    void sendOverdata();

    void delete_data();
    void delete_pic(QString path);
    void reply_data();
    void get_video();
};

#endif // MAINWINDOW_H
