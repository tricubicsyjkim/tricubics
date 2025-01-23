#include "Timer.h"

Timer::Timer(QObject *parent)
    :  QTimer(parent)
{
    m_nEvent = Timer::EVENT_NONE;
}

Timer::~Timer()
{

}

void Timer::start(int msec, int nEvent, int id)
{
    m_nEvent = nEvent;
    QTimer::start(msec);
    qDebug()<< "id:" << id;
}
