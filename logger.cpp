#include "Logger.h"
#include <QMutex>
#include <QFile>
#include <QDir>
#include <QDateTime>
extern QString logPath;
void logDebug(char *data)
{
    qDebug() << data;
    return;
}
void logWarning(char *data)
{
    qDebug() << data;
    //qWarning() << data;
    return;
}
void logCritical(char *data)
{
    qDebug() << data;
    //qCritical() << data;
    return;
}
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static QMutex mutex;
    mutex.lock();
    QString text;
    switch (type) {
        case QtDebugMsg:
        text =QString("Debug:");
            break;
        case QtWarningMsg:
            text =QString("Warning:");
            break;
        case QtCriticalMsg:
            text =QString("Critical:");
        break;
        case QtFatalMsg:
            text =QString("Fatal:");
            mutex.unlock();
            return;
        default:break;
    }
    QString context_info = QString("File:(%1) Line:(%2)").arg(QString(context.file)).arg(context.line);
    QString current_date_time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss ddd");
    QString current_date = QString("(%1)").arg(current_date_time);
    QString message = QString("%1 %2 %3 %4").arg(current_date).arg(context_info).arg(text).arg(msg);
    //QString message = QString("%1 %2 %3").arg(current_date).arg(text).arg(msg);
    QDate currentDate =QDate::currentDate();
    char pic[128] ={0};
    sprintf(pic,"%s\\%d\\%02d\\%02d",logPath.toStdString().c_str(),currentDate.year(),currentDate.month(),currentDate.day());
    QString dir_str;
    dir_str.sprintf("%s",pic);
    QDir dir;
    if (!dir.exists(dir_str))
    {
        bool res = dir.mkpath(dir_str);
        if(true==res)
        {
            qDebug()<<"mkpath ok"<<dir_str;
        }
        else {
            qDebug()<<"mkpath error"<<dir_str;
        }
    }
    QString filePath;
    filePath.sprintf("%s\\log.txt",pic);
    QFile file(filePath);
    file.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream text_stream(&file);
    text_stream << message << "\r\n";
    file.flush();
    file.close();
    mutex.unlock();
}

