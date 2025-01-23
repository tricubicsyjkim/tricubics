#ifndef MTDEVICE_H
#define MTDEVICE_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <mtdev.h>
#include <vector>

#define EVENT_SIZE sizeof(struct input_event)
#define DIM_EVENTS 512
#define DIM_BUFFER (DIM_EVENTS * EVENT_SIZE)
#define LEGACY_API_NUM_MT_AXES 11
#define MAX_MT_VALUE 40000

struct mtdev_iobuf {
    int head, tail;
    char data[DIM_BUFFER];
};

struct mtdev_evbuf {
    int head;
    int tail;
    struct input_event buffer[DIM_EVENTS];
};

struct mtdev_state {
    int has_ext_abs[MT_ABS_SIZE - LEGACY_API_NUM_MT_AXES];
    struct input_absinfo ext_abs[MT_ABS_SIZE - LEGACY_API_NUM_MT_AXES];
    struct mtdev_iobuf iobuf;
    struct mtdev_evbuf inbuf;
    struct mtdev_evbuf outbuf;
};

struct mtdev_slot {
    int touch_major;
    int touch_minor;
    int width_major;
    int width_minor;
    int orientation;
    int position_x;
    int position_y;
    int tool_type;
    int blob_id;
    int tracking_id;
    int pressure;
    int distance;
};

class MTDevice : public QThread
{
    Q_OBJECT
public:
    MTDevice();
    MTDevice(QString mtdev_path);
    ~MTDevice();

    void stop();
    bool proc();
    int mtdev_empty(struct mtdev *dev);
    int mtdev_get(struct mtdev *dev, int fd, struct input_event* ev, int ev_max);
    int mtdev_fetch_event(struct mtdev *dev, int fd, struct input_event *ev);
    void process_typeB(struct mtdev_state *state);
    void mtdev_put_event(struct mtdev *dev, const struct input_event *ev);
    void mtdev_get_event(struct mtdev *dev, struct input_event* ev);

    static inline int evbuf_empty(const struct mtdev_evbuf *evbuf);
    static inline void evbuf_put(struct mtdev_evbuf *evbuf,const struct input_event *ev);
    static inline void evbuf_get(struct mtdev_evbuf *evbuf, struct input_event *ev);

    void print_event(const struct input_event *ev);
    void analysis_event(const struct input_event *ev, mtdev_slot &mtdev_data);
    void check_x_value(int x);
    int get_shelf_id(int x);
    int get_shelf_id_from_vec(int x);

signals:
    void hand_in_process(const int &);
    void hands_out_process();

protected:
    void closeMTDev();

private:
    void run() override;

private:
    int fd;
    int length = 0;
    std::vector<mtdev_slot> mtdev_infos;
    int index = -1;
    int shelf_roi[7] = {20000, 14600, 12390, 10100, 6800,  3820, 10};

};

#endif // MTDEVICE_H
