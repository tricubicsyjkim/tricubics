#include "CameraManager.h"
#include "Instance.h"
#include "SmartVM.h"

#include <QDebug>
#include <QThread>
#include <iostream>
#include <iterator>
#include <vector>

CameraManager::CameraManager() {
}

CameraManager::~CameraManager() 
{
    qDebug("cameraManager delete");
}

unsigned int CameraManager::getShelfId(const QString &shelf) 
{
    return shelf.mid(sizeof(CONFIG_SHELF_NAME) - 1).toUInt();
}

unsigned int CameraManager::getCamId(const QString &cam_device) 
{
    return cam_device.mid(sizeof(CONFIG_CAMERA_DEVNAME) - 1).toUInt();
}

void CameraManager::start() 
{
    qRegisterMetaType<QSharedPointer<V4LGrabber>>();

#if 1
    for (auto shelf : _CONF->shelf_configs_.toStdMap()) {
        unsigned int shelf_id = getShelfId(shelf.first);
        for (auto &it : shelf.second.cam_devices) {
            unsigned int cam_id = getCamId(it.first);
            devices_[shelf.first].push_back(V4LGrabber::create(V4L2Parameters(
                                                                   shelf_id,
                                                                   cam_id,
                                                                   it.second,
                                                                   _CONF->camera_src_width_,
                                                                   _CONF->camera_src_height_,
                                                                   _CONF->camera_dst_width_,
                                                                   _CONF->camera_dst_height_,
                                                                   _CONF->camera_fps_)));

            if (!devices_[shelf.first].last().isNull()){
                connect(devices_[shelf.first].last().data(),
                        SIGNAL(capture(const QString &, int)),
                        this,
                        SLOT(on_captured(const QString &, int)));
                //mt index
                connect(devices_[shelf.first].last().data(),
                        SIGNAL(capture(int, const QString &, int)),
                        this,
                        SLOT(on_captured(int, const QString &, int)));
                connect(devices_[shelf.first].last().data(),
                        SIGNAL(usb_disconnected(const QString &, int)),
                        this,
                        SLOT(on_usb_disconnected(const QString &, int)),
                        Qt::QueuedConnection);
                connect(devices_[shelf.first].last().data(),
                        SIGNAL(usb_disconnected(const QString &, int, int)),
                        this,
                        SLOT(on_usb_disconnected(const QString &, int, int)),
                        Qt::QueuedConnection);
            }
        }
    }

#endif

    for (auto m : devices_.toStdMap()) {
        for (auto grabber : m.second) {
            if (!grabber.isNull()) {
                grabber->getDevice()->printCapabilities();
                grabber->start();
                //grabber->stream_on();
            }
        }
    }

    cam_timer.setSingleShot(true);
    connect(&cam_timer, SIGNAL(timeout()), this, SLOT(cam_timeout()));
}

void CameraManager::start(uint32_t shelf_no, uint32_t cam_no) 
{
    Q_UNUSED(shelf_no);
    Q_UNUSED(cam_no);
}

void CameraManager::on_off(int mt_index, int shelf_no, bool camera_is_on)
{
    QString shelf_no_str = "shelf_";
    shelf_no_str.append(QString::number(shelf_no));
#ifdef DEBUG
    qDebug() << "camera on off: " << shelf_no_str<<" , "<<camera_is_on<<" , mt_index: "<< mt_index;
#endif
    QMap<QString, QVector<QSharedPointer<V4LGrabber>>>::Iterator grabber_iter;
    grabber_iter = devices_.find(shelf_no_str);
    if (grabber_iter != devices_.end()) {
        for (auto grabber : grabber_iter.value()) {
            if (!grabber.isNull()) {
                if (camera_is_on) {
                    grabber->save_image = true;
                    grabber->set_multi_touch_off(false);
                    grabber->mt_index = mt_index;
                    qDebug()<<"cam manager id:  "<<grabber->getDevice()->getParam()->id_;
                } else {
                    grabber->save_image = false;
                    grabber->set_multi_touch_off(true);
                    //grabber->mt_index = mt_index;
                }
            }else{
                qDebug("grabber is null");
                qDebug()<<"shelf: "<<grabber_iter.key();
            }
        }
    }else{
        qDebug("grabber_iter is null");
    }
}

void CameraManager::on_off(int mt_index, int shelf_no, bool camera_is_on, bool device_opened)
{
    QString shelf_no_str = "shelf_";
    shelf_no_str.append(QString::number(shelf_no));
#ifdef DEBUG
    qDebug() << "camera on off: " << shelf_no_str<<" , "<<camera_is_on<<" , mt_index: "<< mt_index;
#endif
    QMap<QString, QVector<QSharedPointer<V4LGrabber>>>::Iterator grabber_iter;
    grabber_iter = devices_.find(shelf_no_str);
    if (grabber_iter != devices_.end()) {
        for (auto grabber : grabber_iter.value()) {
            if (!grabber.isNull()) {
                if (camera_is_on) {
                    int cam_id = grabber->getDevice()->getParam()->id_;
                    qDebug()<<"cam id in on_off: "<<cam_id;
                    if(device_opened && cam_id != 0){
                        grabber->save_image = false;
                    }else{
                        grabber->save_image = true;
                    }
                    grabber->set_multi_touch_off(false);
                    grabber->mt_index = mt_index;
                } else {
                    grabber->save_image = false;
                    grabber->set_multi_touch_off(true);
                    //grabber->mt_index = mt_index;
                }
            }else{
                qDebug("grabber is null");
                qDebug()<<"shelf: "<<grabber_iter.key();
            }
        }
    }else{
        qDebug("grabber_iter is null");
    }
}


void CameraManager::set_img_path_file(QString file_name)
{
    img_path_filename = file_name;
}

QString CameraManager::find_device(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    QString device = QString::fromStdString(result);
    return device;
}

void CameraManager::stop(uint32_t shelf_no, uint32_t cam_no) 
{
    Q_UNUSED(shelf_no);
    Q_UNUSED(cam_no);
}

void CameraManager::on_captured(const QString &path, int cam_id)
{
#ifdef DEBUG
    qDebug()<<"capture_signal: "<< capture_signal_t.elapsed()<<" ms";
#endif
    QFileInfo fileInfo(path);
    QString abs_path = fileInfo.absoluteFilePath();
    qDebug()<<"abs path: "<<abs_path;
    emit transmit_image(abs_path, cam_id);
}

void CameraManager::on_captured(int mt_index, const QString &path, int cam_id)
{
    QFileInfo fileInfo(path);
    QString abs_path = fileInfo.absoluteFilePath();
    //qDebug()<<"abs path: "<<abs_path<<" , cam_id: "<< cam_id<<" , mt_index: "<<mt_index;
    emit transmit_image(mt_index, abs_path, cam_id);
}

void CameraManager::cam_timeout()
{
    on_off(0,1,true);
    //    cam_timer.start(50);
}

void CameraManager::on_usb_disconnected(const QString &dev_name, int shelf_id)
{
    qDebug()<<" shelf"<<shelf_id<<" ,"<<dev_name<<" is broken";
    emit camera_error(dev_name, shelf_id);
}

void CameraManager::on_usb_disconnected(const QString &dev_name, int shelf_id, int cam_id)
{
    qDebug()<<" shelf"<<shelf_id<<" ,"<<dev_name<<" , cam_id: "<< cam_id;
    QString shelf_name = "shelf_" + QString::number(shelf_id);
    devices_[shelf_name][cam_id].clear();
    QString command = "bash ./find_serial.sh " +  dev_name;
    QString cam_device =  find_device(command.toLocal8Bit().data());
    int error_index = 0;
    while(cam_device.length() <=0 && error_index < 500){
        qDebug()<<"search camera again";
        error_index++;
        cam_device = find_device(command.toLocal8Bit().data());
    }
    error_index = 0;
    qDebug()<<"found camera: "<< cam_device;
    if(cam_device == ""){
        emit camera_error(dev_name, shelf_id);
        return;
    }

    QSharedPointer<V4LGrabber> grabber = V4LGrabber::create(V4L2Parameters(
                                                                shelf_id,
                                                                cam_id,
                                                                dev_name,
                                                                _CONF->camera_src_width_,
                                                                _CONF->camera_src_height_,
                                                                _CONF->camera_dst_width_,
                                                                _CONF->camera_dst_height_,
                                                                _CONF->camera_fps_));

    connect(grabber.data(),
            SIGNAL(capture(const QString &, int)),
            this,
            SLOT(on_captured(const QString &, int)));
    //mt index
    connect(grabber.data(),
            SIGNAL(capture(int, const QString &, int)),
            this,
            SLOT(on_captured(int, const QString &, int)));
    connect(grabber.data(),
            SIGNAL(usb_disconnected(const QString &, int)),
            this,
            SLOT(on_usb_disconnected(const QString &, int)),
            Qt::QueuedConnection);
    connect(grabber.data(),
            SIGNAL(usb_disconnected(const QString &, int, int)),
            this,
            SLOT(on_usb_disconnected(const QString &, int, int)),
            Qt::QueuedConnection);
    grabber->start();
    devices_[shelf_name][cam_id] = grabber;
    if(is_opened){
        emit camera_error(dev_name, shelf_id);
    }
}
