#ifndef ZEROMQMANAGER_H
#define ZEROMQMANAGER_H
#include <QObject>
#include <QMap>
#include "ZeromqGrabber.h"
#include "Instance.h"

class ZeromqManager : public QObject
{
    Q_OBJECT
public:
    explicit  ZeromqManager();
    ~ZeromqManager();
    void init_label();
    const QString get_name(int obj_id);
signals:
    void zeromq_req_proc(const QString&, const QString&, const int &);
    void detector_process(const vision_out_t &);
public slots:
    void on_transmit_image(const QString&, const int &);
    //mt index
    void on_transmit_image(int, const QString&, const int &);
    void on_level1_bbox(const QString &);
    void on_level2_bbox(const QString &);
private:
    QMap<int, QMap<int, QSharedPointer<ZeromqGrabber>>> sender_grabbers;
    QMap<int, QMap<int, QSharedPointer<ZeromqGrabber>>> receiver_grabbers;
    QStringList class_names_;
};

#endif // ZEROMQMANAGER_H
