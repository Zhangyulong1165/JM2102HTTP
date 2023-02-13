#include "bridge.h"

Bridge::Bridge(QObject *parent) : QObject(parent)
{
    m_ZYLTools = new ZYLTools();
    m_ZYLTools_QThread = new QThread;
    //m_ZYLTools->moveToThread(m_ZYLTools_QThread);

}
