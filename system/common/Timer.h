#ifndef TIMER_H
#define TIMER_H

#include <QObject>
#include <QTimer>
#include <QDebug>


class Timer : public QTimer
{
    Q_OBJECT
public:
    Timer(QObject *parent = 0);
    ~Timer();

    enum {
        EVENT_NONE = 0,
        EVENT_INIT_PROC,
        EVENT_CAM,
        EVENT_PHOTO
    };

    void start(int msec, int nEvent = EVENT_NONE, int id=-1);
    int getEvent() { return m_nEvent; }
    int getId() { return m_nEvent; }

private:
    int m_nEvent;
    int m_nId;
};

#endif // TIMER_H
