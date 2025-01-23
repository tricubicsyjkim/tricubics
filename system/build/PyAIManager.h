#ifndef PYAIMANAGER_H
#define PYAIMANAGER_H
#include <QThread>
#include <QByteArray>
#include <QDebug>
#include <QVector>
#include <QJsonObject>
#include <QJsonDocument>
#include "AIVsionDefines.h"
#include <mqueue.h>

class PyAIManager  : public QThread
{
    Q_OBJECT
public:
    explicit PyAIManager();
    ~PyAIManager() override;
    bool proc();
    void init_label();
    const QString get_name(int obj_id);

signals:
    void detector_process(const vision_out_t &);

public slots:
    void on_open_cam_processed(const QString &);
private:
    void run() override;

public:
    mqd_t ai_mqd;
    mqd_t sys_mqd;
    char ai_buffer[1024];
    char sys_buffer[1024];
    struct mq_attr ai_attr;
    struct mq_attr sys_attr;
    int ai_result_num = 0;
    int sys_result_num= 0;
    QStringList class_names_;

};

#endif // PYAIMANAGER_H
