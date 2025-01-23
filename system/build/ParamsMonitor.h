#ifndef PARAMSMONITOR_H
#define PARAMSMONITOR_H
#include "Instance.h"
#include<QThread>
#include<QObject>
#include<QString>
#include<QDebug>
#include<QTimer>
#include <memory>
#include <stdlib.h>

class ParamsMonitor: public QThread
{
    Q_OBJECT
public:
    explicit   ParamsMonitor(QObject *parent = nullptr);
    ~ParamsMonitor() override;
    bool proc();
    int get_param_value(QString device, QString param);
    void set_param_value(QString device, QString param, int param_value);
    QString  get_command(QString device, QString param, QString type);
    QString run_command(QString command);
public slots:
    void on_polling();
private:
    void run() override;
    QString file_name_;
    QTimer polling_timer_;
public:
};

#endif // PARAMSMONITOR_H
