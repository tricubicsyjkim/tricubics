#ifndef QTLOGWRAPPER_H
#define QTLOGWRAPPER_H

#include <QtMessageHandler>
#include <QMutex>
#include <QMutexLocker>
#include <QDate>
#include <QTime>
#include <QFile>


typedef enum {
    LC_TRACE = 1,
    LC_FILE = 2,
    LC_DISABLE = 4,
} logChannel;

class QtLogWrapper
{
public:
    QtLogWrapper();
    ~QtLogWrapper();

    void Start();
    void Output(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    void SetPath( const QString &path);
    void SetChannel(int ch);

public:
    static void _log(QtMsgType type, const QMessageLogContext &context, const QString &msg);

private:
    QMutex logMutex;
    QFile logFile;
    QString logDir, lastPath;
    int lc;

};

#endif // QTLOGWRAPPER_H
