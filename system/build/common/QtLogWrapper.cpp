#include "QtLogWrapper.h"

#include <iostream>
#include <CSingleTon.h>
#include <QTextStream>
#include <QDir>
#include "SmartVM.h"

template <>
QtLogWrapper*  CSingleTon<QtLogWrapper>::pInstance = NULL;

QtLogWrapper::QtLogWrapper() 
{
    logDir = "/tmp";
    lastPath = "";

    lc = LC_FILE | LC_TRACE;
    // lc =2;
}

QtLogWrapper::~QtLogWrapper() 
{
    if (logFile.isOpen())
        logFile.close();

    CSingleTon<QtLogWrapper>::releaseInstance();
}

void QtLogWrapper::_log(QtMsgType type, const QMessageLogContext &context, const QString &msg) 
{
    CSingleTon<QtLogWrapper>::getInstancePtr()->Output(type, context, msg);
}

void QtLogWrapper::SetPath(const QString &path) 
{
    logDir = path;
    QDir dir;
    dir.mkpath(logDir);
}

void QtLogWrapper::SetChannel(int ch) 
{ 
    lc = ch;
}

void QtLogWrapper::Start() 
{
    qInstallMessageHandler(CSingleTon<QtLogWrapper>::getInstancePtr()->_log);
}

void QtLogWrapper::Output(QtMsgType type, const QMessageLogContext &context, const QString &msg) 
{
    QMutexLocker locker(&logMutex);

    QString timeStamp = QTime::currentTime().toString("hh:mm:ss:zzz");

    // file save
    if (lc & LC_FILE) {
#if 1
        QString savefile = logDir + "/" + QDate::currentDate().toString("yyyyMMdd") + QString(".log");
#else
        QString savefile = logDir + "/" + QString::number(photo_index, 10) + QString(".log");
#endif
        if (savefile != lastPath) {
            logFile.close();
            logFile.setFileName(savefile);
            if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
                qDebug("log open error !! (%s)", qPrintable(savefile));
                return;
            }
        }

        if (!logFile.isOpen())
            return;

        QTextStream out(&logFile);
        // out.setCodec("eucKR");
#ifdef QT_DEBUG
        out << qPrintable(timeStamp) << " " << context.file << "(" << context.line << ") " << msg <<"\n";
#else
        out << qPrintable(timeStamp) << " " << msg << "\n";
#endif
    }

    if (lc & LC_TRACE) {
#ifdef QT_DEBUG
        std::cout << qPrintable(timeStamp) << " " << context.file << "(" << context.line << ") " << qPrintable(msg) << "\n";
#else
        std::cout << qPrintable(timeStamp) << " " << qPrintable(msg) << "\n";
#endif
        std::cout.flush();
    }
}
