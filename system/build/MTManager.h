#ifndef MTMANAGER_H
#define MTMANAGER_H

#include <QObject>
#include "Instance.h"
#include "MTDevice.h"
#include <QTimer>
#include <QSharedPointer>
#include <QString>
#include <QMap>
#include <string>
#include <memory>
#include <fcntl.h>
#include <unistd.h>
#include <QTimer>


class MTManager : public QObject
{
    Q_OBJECT
public:
    explicit MTManager(QObject *parent = nullptr);
    ~MTManager();

    void start();
    void start(QString &mtdev_path);
    int get_shelf_id(const QString &shelf);
    QString exec_shell(const char* cmd);

signals:
    void mtdev_triggered(const int &, const bool &);

    //mt device has something wrong
    void mt_device_error(const QString&, const int &);

public slots:
    void on_hand_in_process(const int &);
    void hand_is_out(const int &);
    void on_hands_out_process();

    //multi-touch device has something wrong
    void check_device();
    void on_no_res_proc();
public:
    QMap<int, int> devices_state;
    MTDevice *touch_dev;
    //multi-touch device has something wrong
    QTimer *timer;
    bool is_error = false;
    QString mtdev_path_;
    int last_shelf_id = 0;
    QTimer no_res_timer_;
};

#endif // MTMANAGER_H
