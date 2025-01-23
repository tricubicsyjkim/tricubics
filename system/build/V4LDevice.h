#ifndef V4LDevice_H
#define V4LDevice_H

#include <QString>
#include <linux/videodev2.h>
#include <libv4l2.h>
#include "ImageWriter.h"

enum camera_state {
    CAMERA_STATE_INACTIVE = 0,
    CAMERA_STATE_ACTIVE,

    CAMERA_STATE_STREAM_ON,
    CAMERA_STATE_STREAM_OFF
};

#if 0
inline int xioctl( int fd, int request, void *arg)  \
{ \
    int r;\
    \
    do r = v4l2_ioctl (fd, request, arg);\
    while (-1 == r && EINTR == errno);\
    return r;\
}
#endif

struct V4L2Parameters
{
    V4L2Parameters(unsigned int shelf,
                   unsigned int id,
                   const QString &devname,
                   unsigned int src_width, unsigned int src_height,
                   unsigned int tgt_width, unsigned int tgt_height,
                   unsigned int fps):
        shelf_(shelf),
        id_(id),
        devName_(devname),
        src_width_(src_width), src_height_(src_height),
        tgt_width_(tgt_width), tgt_height_(tgt_height),
        fps_(fps)
    {}
    unsigned int shelf_;
    unsigned int id_;
    QString devName_;
    unsigned int src_width_;
    unsigned int src_height_;

    unsigned int tgt_width_;
    unsigned int tgt_height_;

    unsigned int fps_;

};

struct MMAP_BUFFER
{
    void *start;
    size_t length;
};

class V4LDevice
{
    friend class V4LGrabber;

public:
    V4LDevice(const V4L2Parameters &);
    ~V4LDevice();

    bool initDevice();

    void printFrameInterval(int fd, unsigned int fmt, unsigned int width, unsigned int height);
    int printCapabilities();

    bool isOpened();
    bool isReady();
    bool streamOn();
    bool streamOff();
    bool cleanup();
    bool release();

    camera_state getState() { return camera_state_; }
    V4L2Parameters *getParam() { return &param_; }

private:
    int configureFormat(uint32_t);
    int configureFPS();
    bool configureMMAP();

    int isReadable(unsigned int sec);

    QSharedPointer<ImageWriter> capture_frame(bool save = false);
    void request_frame();

    size_t writeImpl(char *, size_t);
    size_t readImpl(char *, size_t);

protected:
    V4L2Parameters param_;
    unsigned int format_;
    camera_state camera_state_;
    int fd_;
    unsigned int buffer_counts_;

    std::vector<MMAP_BUFFER> mmap_buffers_;
    //change 30 frames to 15 or 10 frames
private:
    unsigned int frame_filter;
    unsigned int fps_denominator;

    //camera has something wrong
    bool is_error = false;
};

#endif // V4LDevice_H
