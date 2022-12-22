//unsigned int __stdcall logThread(PVOID pM);
#include <QDebug>
void logDebug(char *data);
void logWarning(char *data);
void logCritical(char *data);
//void log(char *data);
void outputMessage(QtMsgType type, const QMessageLogContext &context, const QString &msg);
//void loglane(char *data, int lane);
//unsigned int __stdcall logLaneThread(PVOID pM);
//void logSendData(char *data);
//unsigned int __stdcall logSendDataThread(PVOID pM);
