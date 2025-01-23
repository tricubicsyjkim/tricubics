#ifndef V4LGRABBER_H
#define V4LGRABBER_H

#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <QSharedPointer>
#include <QTimer>
#include "V4LDevice.h"
#include <QElapsedTimer>

extern double full_darknet;
extern QElapsedTimer capture_signal_t;

class V4LGrabber : public QThread 
{
    Q_OBJECT

public:
    explicit V4LGrabber(QSharedPointer<V4LDevice> device = nullptr);
    ~V4LGrabber() override;

    static QSharedPointer<V4LGrabber> create(const V4L2Parameters &param);
    QSharedPointer<V4LDevice> getDevice() { return device_; }

    bool proc();
    bool stop();
    void stream_on();

    int isReadable(unsigned int sec);

    size_t read(char *, size_t);
    size_t write(char *, size_t);

    // capture 5 or 10 images after multi-touch sensor off
    void set_multi_touch_off(bool status);
protected:
signals:
    void capture(const QString &s);

    /*180804 send signal with cam id and purchase_on */
    void capture(const QString &s, int cam_id);
    //mt index
    void capture(int mt_index, const QString &s, int cam_id);

    //camera has something wrong
    void usb_disconnected(const QString &, int);
    void usb_disconnected(const QString &, int, int);

    void error(const QString &s);
    void timeout(const QString &s);

public slots:

public:
    bool save_image = false;
    int mt_index = 0;
    int current_index = 0;
    int last_index = 0;
    int frame_counter = 0;
    bool capture_finished = false;
    bool mt_off= false;

private:
    V4LGrabber(const V4LGrabber &);
    V4LGrabber &operator=(const V4LGrabber &);

    void run() override;

    int waitTimeout_ = 0;
    QMutex mutex_;
    QWaitCondition cond_;
    bool quit_= false;
    bool is_error = false;

protected:
    QSharedPointer<V4LDevice> device_;

};

#endif // V4LGRABBER_H
