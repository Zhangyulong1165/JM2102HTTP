#ifndef ZYLTOOLS_H
#define ZYLTOOLS_H

#include <QObject>

class ZYLTools : public QObject
{
    Q_OBJECT
public:
    explicit ZYLTools(QObject *parent = nullptr);


signals:

public slots:
private:
    int FtpUpLoadFile(QString path);
};

#endif // ZYLTOOLS_H
