#include "SysUtil.h"
#include <time.h>

SysUtil::SysUtil()
{

}

SysUtil::~SysUtil()
{

}

void SysUtil::Shutdown()
{
    sync();

    const QString exe = "/sbin/shutdown";
    QStringList arguments;

    arguments << "-P" << "now";
    QProcess::startDetached(exe, arguments);
}

void SysUtil::Reboot()
{
    sync();

#ifndef QT_DEBUG
    const QString exe = "/sbin/shutdown";
    QStringList arguments;

    arguments << "-r" << "now";
    QProcess::startDetached(exe, arguments);
#else
    qDebug("SysUtil::Reboot()");
#endif
}

void SysUtil::TouchFile(QString file)
{
    const QString exe = "/usr/bin/touch";
    QStringList arguments;

    arguments << qPrintable(file);
    QProcess::startDetached(exe, arguments);
}

void SysUtil::SetSystemTime(time_t t)
{
    ctime(&t);
}

void SysUtil::SysToHwClock()
{
    sync();

    const QString exe = "/sbin/hwclock";
    QStringList arguments;

    arguments << "-w";
    QProcess::startDetached(exe, arguments);
}
