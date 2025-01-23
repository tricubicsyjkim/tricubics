#include "SignalActorImpl.h"

#include <errno.h>
#include <signal.h>
#include <unistd.h>

static SignalActorImpl *pSignalParent = nullptr;

SignalActorImpl::SignalActorImpl(QObject *parent) : QObject(parent) 
{
    pSignalParent = this;
}

void SignalActorImpl::signalActorUpdate(int sig) 
{
    pSignalParent->on_updated(sig);
}

void SignalActorImpl::signalActorFinalize(int sig) 
{
    pSignalParent->on_finalize(sig);
}

//////////////////////////////////////////////////////////////////////////////////////////////
QString SignalActorImpl::getSignalString(int nSignal) 
{
    switch (nSignal) {
    case SIGTERM: return "SIGTERM";
    case SIGINT:  return "SIGINT";
    case SIGSEGV: return "SIGSEGV";
    case SIGHUP:  return "SIGHUP";
    case SIGUSR1: return "SIGUSR1";
    case SIGUSR2: return "SIGUSR2";
    }
    return QString("UNKNOWN SIGNAL(%1)").arg(nSignal);
}

void SignalActorImpl::run() 
{
}

void SignalActorImpl::on_finalize(int type) 
{ 
    Q_UNUSED(type)
}

void SignalActorImpl::on_updated(int type) 
{ 
    Q_UNUSED(type)
}
