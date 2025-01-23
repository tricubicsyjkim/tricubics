#ifndef SYSUTIL_H
#define SYSUTIL_H

#include <QProcess>
#include <QStringList>

#include <unistd.h>

class SysUtil
{
public:
    SysUtil();
    ~SysUtil();

    static void Shutdown();
    static void Reboot();

    static void TouchFile(QString file);
    static void SetSystemTime(time_t t);
    static void SysToHwClock();
};

#endif // SYSUTIL_H
