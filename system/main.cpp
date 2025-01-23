#include <signal.h>

#include <QCoreApplication>
#include <QMetaType>
#include <QLibraryInfo>
#include <QTextCodec>
#include <QDebug>
#include <QByteArray>
#include <QtLogWrapper.h>
#include <CSingleTon.h>
#include <QDateTime>

#include "Defines.h"
#include "Instance.h"
#include "SmartVM.h"

static int setup_signal_handlers()
{
    struct sigaction hup, term, intr, usr1, usr2, segv;

    hup.sa_handler = SmartVM::signalActorUpdate;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, nullptr) > 0)
        return SIGHUP;

    term.sa_handler = SmartVM::signalActorFinalize;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, nullptr) > 0)
        return SIGTERM;

    intr.sa_handler = SmartVM::signalActorFinalize;
    sigemptyset(&intr.sa_mask);
    intr.sa_flags = 0;
    intr.sa_flags |= SA_RESTART;

    if (sigaction(SIGINT, &intr, nullptr) > 0)
        return SIGINT;

    usr1.sa_handler = SmartVM::signalActorUpdate;
    sigemptyset(&usr1.sa_mask);
    usr1.sa_flags = 0;
    usr1.sa_flags |= SA_RESTART;

    if (sigaction(SIGUSR1, &usr1, nullptr) > 0)
        return SIGUSR1;

    usr2.sa_handler = SmartVM::signalActorUpdate;
    sigemptyset(&usr2.sa_mask);
    usr2.sa_flags = 0;
    usr2.sa_flags |= SA_RESTART;

    if (sigaction(SIGUSR2, &usr2, nullptr) > 0)
        return SIGUSR2;

    segv.sa_handler = SmartVM::signalActorFinalize;
    sigemptyset(&segv.sa_mask);
    segv.sa_flags = 0;
    segv.sa_flags |= SA_RESTART;

    if (sigaction(SIGSEGV, &segv, nullptr) > 0)
        return SIGSEGV;

    return 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString dateStamp = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString logDir = QString(LOG_PATH+dateStamp+"/log/sys");
    CSingleTon<QtLogWrapper>::getInstancePtr()->SetPath(logDir);
    CSingleTon<QtLogWrapper>::getInstancePtr()->Start();

    qDebug("###### Starting SmartVM compiled at %s ######",
           qPrintable(QString::fromLocal8Bit(BUILDDATE)) );

    qDebug() << "QT library path: " << QCoreApplication::libraryPaths();
    qDebug() << "QT Settings: " << QLibraryInfo::location(QLibraryInfo::SettingsPath);
    qDebug() << "QT Imports: " << QLibraryInfo::location(QLibraryInfo::ImportsPath);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    qDebug() << "system name:" << QLocale::system().name();
    setup_signal_handlers();

    qRegisterMetaType<vision_inp_t>();
    qRegisterMetaType<vision_out_t>();
    qRegisterMetaType<vision_input_t>();
    qRegisterMetaType<QVector<int>>("QVector<int>");
    Instance::instance()->init();

    SmartVM sv(&app);

    return app.exec();
}
