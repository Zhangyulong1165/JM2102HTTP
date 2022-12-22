#ifndef MY_MATCH_H
#define MY_MATCH_H

#include <QObject>

class my_match : public QObject
{
    Q_OBJECT
public:
    explicit my_match(QObject *parent = nullptr);

public:
    void matchWeight();
    void matchCam();
    void matchFrontPic();
    void matchBackPic();
signals:

};

#endif // MY_MATCH_H
