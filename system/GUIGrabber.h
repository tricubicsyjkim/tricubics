#ifndef GUIGRABBER_H
#define GUIGRABBER_H
#include <QThread>
#include <QObject>
#include <zmq.h>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>


class GUIGrabber : public QThread
{
    Q_OBJECT
public:
    explicit GUIGrabber();
    ~GUIGrabber() override;
    void send_gui_msg(int cls_id, const QString status);
public:
    void *context;
    void *requester;
    char req_buffer[1024];
};

#endif // GUIGRABBER_H
