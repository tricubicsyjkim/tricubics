#include "V4LGrabber.h"
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <QDebug>
#include <QElapsedTimer>
#include "AIVsionDefines.h"
#include "SmartVM.h"

double full_darknet = 0;
QElapsedTimer capture_signal_t;

V4LGrabber::V4LGrabber(QSharedPointer<V4LDevice> device)
    : QThread(nullptr), device_(device)
{
}

V4LGrabber::~V4LGrabber() 
{

    mutex_.lock();
    quit_ = true;
    cond_.wakeOne();
    mutex_.unlock();
    wait();

    device_->streamOff();
    device_->cleanup();
    qDebug("V4LGrabber::~V4LGrabber()");
}

QSharedPointer<V4LGrabber> V4LGrabber::create(const V4L2Parameters &param) 
{
    QSharedPointer<V4LDevice> videoDevice = QSharedPointer<V4LDevice>::create(param);
    QSharedPointer<V4LGrabber> videoGrabber;

    if (!videoDevice->initDevice()) {
        videoDevice.reset();
        qDebug("%s initializing error...!", qPrintable(param.devName_));
    } else {
        videoGrabber = QSharedPointer<V4LGrabber>::create(videoDevice);
    }

    return videoGrabber;
}

int V4LGrabber::isReadable(unsigned int sec) 
{
    return device_->isReadable(sec);
}

size_t V4LGrabber::read(char *data, size_t len) 
{
    return device_->readImpl(data, len);
}

size_t V4LGrabber::write(char *data, size_t len) 
{
    return device_->writeImpl(data, len);
}

void V4LGrabber::set_multi_touch_off(bool status)
{
    //    device_->multi_touch_off = status;
}

void V4LGrabber::run() 
{
    qDebug()<<"grabber id :" <<this->currentThreadId() << "is running";
    this->proc();
}

bool V4LGrabber::stop() 
{
    QMutexLocker locker(&mutex_);
    quit_ = true;
    qDebug()<<"stop grabber: "<<quit_<< ", id: "<<this->currentThreadId();
    this->quit();
    return quit_;
}

bool V4LGrabber::proc() 
{

    QElapsedTimer stream_on_t;
    stream_on_t.start();
    if(device_->streamOn()){
        quit_ = false;
    }
    qDebug()<<"stream on timer: "<< stream_on_t.elapsed() <<" ms";

    while (!quit_)
    {
        usleep(1);
        QElapsedTimer profile_t;
        profile_t.start();
        int ret = device_->isReadable(1); // timeout 1 sec

        switch (ret) {
        case -1: // error
            quit_ = true;
            break;
        case 0: // timeout
            break;
        default: {

            //mt index
            if((mt_off == true) && (capture_finished == false)){
                if(frame_counter < _CONF->cameras_shift_counts_){
                    save_image = true;
                }else{
                    capture_finished = true;
                    frame_counter += 1;
                }
            }

            profile_t.start();
            //add ring buffer
            QSharedPointer<ImageWriter> imageWriter = device_->capture_frame(save_image);
            if (imageWriter.isNull()) {

                //camera has something wrong
                if(device_->is_error == true){
                    if(!is_error){
                        is_error = true;
                        qDebug()<<"dev: "<<device_->param_.devName_<<" , image null";
                        emit usb_disconnected(device_->param_.devName_, device_->param_.shelf_, device_->param_.id_);
                    }
                }
            } else {
                capture_signal_t.start();
                usleep(1);
                emit capture(mt_index, imageWriter->path_, static_cast<int>(device_->param_.id_));
//                save_image = false;
            }
        }
        }
    }

    QElapsedTimer finish_t;
    finish_t.start();
    qDebug()<<"camera release : "<<finish_t.elapsed()<<" ms";
    return true;
}
