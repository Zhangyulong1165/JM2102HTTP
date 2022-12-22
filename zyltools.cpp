#include "zyltools.h"

#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkReply>

ZYLTools::ZYLTools(QObject *parent) : QObject(parent)
{

}
int ZYLTools::FtpUpLoadFile(QString path)
{
    QFile*file = new QFile(path);
    file->open(QIODevice::ReadOnly);
    QByteArray byte_file = file->readAll();

    QNetworkAccessManager *accessManager = new QNetworkAccessManager(this);
    accessManager->setNetworkAccessible(QNetworkAccessManager::Accessible);
    QUrl url("ftp://***.**.***.**/picture/report.pdf");//填服务器地址
    url.setPort(21);
    url.setUserName("****");  // ftp服务器 用户名
    url.setPassword("*********");// ftp 服务器密码

    QNetworkRequest request(url);
    QNetworkReply*reply = accessManager->put(request, byte_file);
    connect(reply,&QNetworkReply::uploadProgress,[=](qint64 cur,qint64 total){
//        ui->progressBar->setMaximum(total);
//        ui->progressBar->setValue(cur);
    });
    connect(accessManager,&QNetworkAccessManager::finished,[=](){
//        QMessageBox::information(this,"提示","上传完成!");
        return 0;
    });

}
