#ifndef WATCHDOORMANAGER_H
#define WATCHDOORMANAGER_H
#include <QThread>
#include <QByteArray>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <mqueue.h>
#include <zmq.h>
#include <QTime>
#include <QFileInfo>
#include "Instance.h"

class WatchDoorManager: public QThread
{
    Q_OBJECT
public:
    explicit WatchDoorManager();
    ~WatchDoorManager() override;
    bool proc();

signals:
    void https_process(const QString &);
    void watch_door();
public slots:

private:
    void run() override;

public:
    void *context;
    void *sender;
    void *receiver;
    char req_buffer[1024];
    char res_buffer[1024];
};

#endif //WATCHDOORMANAGER_H
