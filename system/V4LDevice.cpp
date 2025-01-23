#include "V4LDevice.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTime>
#include <QDataStream>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/select.h>

#include "Instance.h"

#if 1
inline int xioctl(int fd, int request, void *arg)
{
    int r;

    do {
        r = v4l2_ioctl(fd, request, arg);
    } while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

    if (r == -1) {
        //qDebug( "xioctl(0x%x) error %d, %s", request, errno, strerror(errno));
    }

    return r;
}
#endif

std::string fourcc(unsigned int format)
{
    char formatArray[] = {(char)(format & 0xff), (char)((format >> 8) & 0xff), (char)((format >> 16) & 0xff), (char)((format >> 24) & 0xff), 0};
    return std::string(formatArray, strlen(formatArray));
}

V4LDevice::V4LDevice(const V4L2Parameters &param)
    : param_(param), camera_state_(CAMERA_STATE_INACTIVE), fd_(-1), buffer_counts_(0)
{
}

V4LDevice::~V4LDevice()
{
    /*   UnInitialize  device  */
    release();
}

bool V4LDevice::isReady()
{
    return false;
}

bool V4LDevice::isOpened()
{
    return (fd_ != -1);
}

bool V4LDevice::streamOn()
{
    if (camera_state_ == CAMERA_STATE_STREAM_ON)
        return true;

    // start stream
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd_, VIDIOC_STREAMON, &type))
    {
        //qDebug("VIDIOC_STREAMON error !");
        return false;
    }

    camera_state_ = CAMERA_STATE_STREAM_ON;
    //qDebug("VIDIOC_STREAMON ok  !");

    return true;
}

bool V4LDevice::streamOff()
{
    if (camera_state_ == CAMERA_STATE_STREAM_OFF)
        return true;

    // stop  stream
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd_, VIDIOC_STREAMOFF, &type))
    {
        //qDebug("VIDIOC_STREAMOFF errpr !");
        return false;
    }

    camera_state_ = CAMERA_STATE_STREAM_OFF;
    //qDebug("CAMERA_STATE_STREAM_OFF ok  !");

    return true;
}

bool V4LDevice::cleanup()
{
    bool ret = true;

    for (unsigned int i = 0; i < buffer_counts_; ++i)
    {
        if (-1 == v4l2_munmap(mmap_buffers_[i].start, mmap_buffers_[i].length))
        {
            //qDebug("%d munmap failed", i);
            ret = false;
        }
    }

    // free buffers
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = 0;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    if (-1 == xioctl(fd_, VIDIOC_REQBUFS, &req))
    {
        perror("VIDIOC_REQBUFS");
        ret = false;
    }

    buffer_counts_ = 0;

    return ret;
}

/*   UnInitialize  device  */
bool V4LDevice::release()
{
    bool processed = false;

    if (isOpened()) {
        this->streamOff();
        this->cleanup();

        v4l2_close(fd_);

        processed = true;
    }

    //qDebug("V4LDevice cleanup %s", processed ? "OK" : "not needed");
    return processed;
}

bool V4LDevice::initDevice()
{
    fd_ = v4l2_open(qPrintable(param_.devName_), O_RDWR | O_NONBLOCK, 0);

    if (fd_ != -1) {

        configureFPS();
        configureFormat(ImageWriter::imageFormat(_CONF->camera_captureformat_));
        if (configureMMAP()) {
            camera_state_ = CAMERA_STATE_ACTIVE;
        }
    }else{
        qDebug()<<"fd_ = -1";
    }

    frame_filter = 0;
    return isOpened();
}

void V4LDevice::printFrameInterval(int fd, unsigned int fmt, unsigned int width, unsigned int height)
{
    struct v4l2_frmivalenum frmival;
    memset(&frmival, 0, sizeof(frmival));
    frmival.pixel_format = fmt;
    frmival.width = width;
    frmival.height = height;
    while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) == 0)
    {
        if (frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE)
        {
            //qDebug("    [%dx%d] %f fps\n", width, height, 1.0 * frmival.discrete.denominator / frmival.discrete.numerator);
        }
        else
        {
            //   qDebug("    [%dx%d] [%f,%f] fps\n", width, height,1.0 * frmival.stepwise.max.denominator / frmival.stepwise.max.numerator, 1.0 * frmival.stepwise.min.denominator / frmival.stepwise.min.numerator);
        }
        frmival.index++;
    }
}

int V4LDevice::printCapabilities()
{
    struct v4l2_capability cap;
    memset(&(cap), 0, sizeof(cap));
    if (-1 == xioctl(fd_, VIDIOC_QUERYCAP, &cap))
    {
        //qDebug() << "Cannot get capabilities for device:" << param_.devName_ << " "
        //<< strerror(errno);
        return -1;
    }

    // capabilities
    if ((cap.capabilities & V4L2_CAP_VIDEO_OUTPUT)) //qDebug() << qPrintable(param_.devName_) << "cap. support output";
        if ((cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) //qDebug() << qPrintable(param_.devName_) << "cap. support capture";
            if ((cap.capabilities & V4L2_CAP_READWRITE)) //qDebug() << qPrintable(param_.devName_) << "cap. support read/write";
                if ((cap.capabilities & V4L2_CAP_STREAMING))  //qDebug() << qPrintable(param_.devName_) << "cap. support streaming";
                    if ((cap.capabilities & V4L2_CAP_TIMEPERFRAME))  //qDebug() << qPrintable(param_.devName_) << "cap. support timeperframe";
                        // supported formats and resolutions
                    {
                        struct v4l2_fmtdesc fmt;
                        struct v4l2_frmsizeenum frmsize;
                        struct v4l2_frmivalenum frmival;

                        fmt.index = 0;
                        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        while (xioctl(fd_, VIDIOC_ENUM_FMT, &fmt) >= 0) {
                            frmsize.pixel_format = fmt.pixelformat;
                            frmsize.index = 0;
                            //qDebug("%s support : %s", qPrintable(param_.devName_), fmt.description);

                            while (xioctl(fd_, VIDIOC_ENUM_FRAMESIZES, &frmsize) >= 0) {
                                if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                                    //qDebug("    DISCRETE: %dx%d", frmsize.discrete.width,
                                    //      frmsize.discrete.height);
                                    printFrameInterval(fd_, frmsize.pixel_format, frmsize.discrete.width, frmsize.discrete.height);
                                } else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
                                    for (unsigned width = frmsize.stepwise.min_width; width < frmsize.stepwise.max_width; width += frmsize.stepwise.step_width)
                                        for (unsigned height = frmsize.stepwise.min_height; height < frmsize.stepwise.max_height; height += frmsize.stepwise.step_height)
                                            printFrameInterval(fd_, frmsize.pixel_format, width, height);
                                }
                                frmsize.index++;
                            }
                            fmt.index++;
                        }
                    }

    return 0;
}

int V4LDevice::configureFormat(uint32_t req_fmt)
{
    struct v4l2_format fmt;
    memset(&(fmt), 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = param_.src_width_;
    fmt.fmt.pix.height = param_.src_height_;
    fmt.fmt.pix.pixelformat = req_fmt;
    fmt.fmt.pix.field = V4L2_FIELD_ANY; // V4L2_FIELD_NONE // V4L2_FIELD_INTERLACED; //
    // V4L2_FIELD_ANY; // what's the differences ???

    if (xioctl(fd_, VIDIOC_S_FMT, &fmt) == -1)
    {
        //qDebug() << "Cannot set format for device:" << param_.devName_ << " "
        //<< strerror(errno);
        return -1;
    }
    if (fmt.fmt.pix.pixelformat != req_fmt)
    {
        //qDebug() << "Cannot set pixelformat to " << fourcc(req_fmt).c_str() << ",   current format is:" << fourcc(fmt.fmt.pix.pixelformat).c_str();
    }
    if ((fmt.fmt.pix.width != param_.src_width_) || (fmt.fmt.pix.height != param_.src_height_))
    {
        //qDebug() << "Cannot set size to:" <<
        //param_.src_width_ << "x" << param_.src_height_ << " current  size is:" << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height;
        return -1;
    }

    //qDebug() << "requested format " << param_.devName_ << ":" << fourcc(fmt.fmt.pix.pixelformat).c_str()
    //<< " size:" << param_.src_width_ << "x" << param_.src_height_
    //<< " buffer size:" << fmt.fmt.pix.sizeimage;

    format_ = req_fmt;

    return 0;
}

int V4LDevice::configureFPS()
{
    if (param_.fps_ != 0)
    {
        struct v4l2_streamparm v4l2_param;
        memset(&v4l2_param, 0, sizeof(v4l2_param));
        v4l2_param.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        v4l2_param.parm.capture.timeperframe.numerator = 1;
        v4l2_param.parm.capture.timeperframe.denominator = param_.fps_;
        // v4l2_param.parm.capture.capturemode = 0;

        if (xioctl(fd_, VIDIOC_S_PARM, &v4l2_param) == -1)
        {
            qDebug() << "Cannot set param for device:" << param_.devName_ << " " << strerror(errno);
        }


        if (v4l2_param.parm.capture.timeperframe.denominator != param_.fps_) {
            qDebug() << "setup fps:" << param_.fps_ << " not supported."<<param_.devName_ <<", dev id: "<<param_.id_;
            fps_denominator = v4l2_param.parm.capture.timeperframe.denominator/param_.fps_;
            qDebug()<<"denominator : "<<fps_denominator;
        }else{
            fps_denominator = 1;
        }

        qDebug() << "current fps:" << v4l2_param.parm.capture.timeperframe.numerator << "/" << v4l2_param.parm.capture.timeperframe.denominator;
        qDebug() << "read buffer counts :" << v4l2_param.parm.capture.readbuffers;
    }

    return 0;
}

bool V4LDevice::configureMMAP()
{
    bool success = true;
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = _CONF->camera_buffer_counts_;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    mmap_buffers_.reserve(req.count);

    if (-1 == xioctl(fd_, VIDIOC_REQBUFS, &req))
    {
        if (EINVAL == errno)
        {
            qDebug() << param_.devName_ << " does not support memory mapping";
            success = false;
        }
        else
        {
            qDebug("VIDIOC_REQBUFS error !");
            success = false;
        }
    }
    else
    {
        // allocate buffers
        for (buffer_counts_ = 0; buffer_counts_ < req.count; ++buffer_counts_)
        {
            memset(&mmap_buffers_[buffer_counts_], 0, sizeof(MMAP_BUFFER));

            struct v4l2_buffer buf;
            memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = buffer_counts_;

            if (-1 == xioctl(fd_, VIDIOC_QUERYBUF, &buf))
            {
                qDebug("VIDIOC_QUERYBUF error !");
                success = false;
            }
            else
            {
                qDebug() << qPrintable(param_.devName_) << " buffer idx:" << buffer_counts_ << " size:" << buf.length;
                mmap_buffers_[buffer_counts_].length = buf.length;
                mmap_buffers_[buffer_counts_].start = v4l2_mmap(
                            NULL /* start anywhere */,
                            buf.length,
                            PROT_READ | PROT_WRITE /* required */,
                            MAP_SHARED /* recommended */,
                            fd_,
                            buf.m.offset);

                if (MAP_FAILED == mmap_buffers_[buffer_counts_].start)
                {
                    qDebug("%d mmap failed.", buffer_counts_);
                    success = false;
                }
            }
        }

        // queue buffers
        for (unsigned int i = 0; i < buffer_counts_; ++i)
        {
            struct v4l2_buffer buf;
            memset(&buf, 0, sizeof(buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = V4L2_MEMORY_MMAP;
            buf.index = i;

            if (-1 == xioctl(fd_, VIDIOC_QBUF, &buf))
            {
                qDebug("VIDIOC_QBUF error !");
                success = false;
            }
        }
    }
    return success;
}

int V4LDevice::isReadable(unsigned int sec)
{
    timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = 0;
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(fd_, &fdset);

    return select(fd_ + 1, &fdset, nullptr, nullptr, &tv);
}

size_t V4LDevice::readImpl(char *data, size_t len)
{
    size_t size = 0;

    if (buffer_counts_ > 0)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd_, VIDIOC_DQBUF, &buf))
        {
            qDebug("VIDIOC_DQBUF");
            size = -1;
        }
        else if (buf.index < buffer_counts_)
        {
            size = len;
            if (size > buf.length)
            {
                size = buf.length;
                qDebug() << "Device " << param_.devName_ << " data truncated available:" << buf.length << " needed:" << size;
            }
            memcpy(mmap_buffers_[buf.index].start, data, size);
            buf.bytesused = size;

            if (-1 == xioctl(fd_, VIDIOC_QBUF, &buf))
            {
                qDebug("VIDIOC_QBUF");
                size = -1;
            }
        }
    }

    return size;
}

size_t V4LDevice::writeImpl(char *data, size_t len)
{
    size_t size = 0;

    if (buffer_counts_ > 0)
    {
        struct v4l2_buffer buf;
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd_, VIDIOC_DQBUF, &buf))
        {
            //qDebug("VIDIOC_DQBUF error !");
            size = -1;
        }
        else if (buf.index < buffer_counts_)
        {
            size = buf.bytesused;
            if (size > len)
            {
                size = len;
                //qDebug() << "Device " << param_.devName_ << " data truncated available:" << len << " needed:" << buf.bytesused;
            }
            memcpy(data, mmap_buffers_[buf.index].start, size);

            if (-1 == xioctl(fd_, VIDIOC_QBUF, &buf))
            {
                //qDebug("VIDIOC_QBUF error !");
                size = -1;
            }
        }
    }

    return size;
}

QSharedPointer<ImageWriter> V4LDevice::capture_frame(bool save)
{
    //qDebug()<<"dev name: "<<param_.devName_<<" , save image: "<<save;
    struct v4l2_buffer buf;

    QSharedPointer<ImageWriter> imageWriter = nullptr;

    if (buffer_counts_ > 0)
    {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;

        if (-1 == xioctl(fd_, VIDIOC_DQBUF, &buf)) {
            //qDebug()<<"VIDIOC_DQBUF error !"<<param_.devName_;

            //camera has something wrong
            if(!is_error){
                is_error = true;
            }

            return nullptr;
        }
        else
        {
            if (save)
            {
                qDebug()<<"top camera";
                imageWriter = ImageWriter::ImageSave(
                            param_, (char *)mmap_buffers_[buf.index].start, buf.bytesused,
                        ImageWriter::imageType(_CONF->camera_captureformat_));
            }

            if (frame_filter < buffer_counts_)
            {
                frame_filter++;
            }
            else
            {
                frame_filter = 0;
            }

            if (-1 == ioctl(fd_, VIDIOC_QBUF, &buf))
            {
                qDebug("VIDIOC_QBUF error !");
                return nullptr;
            }
        }
    }

    return imageWriter;
}

void V4LDevice::request_frame()
{
}
