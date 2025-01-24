#include "ImageWriter.h"
#include "Defines.h"
#include "V4LDevice.h"
#include "Instance.h"
#include "SmartVM.h"
#include <QDebug>

ImageWriter::ImageWriter(const V4L2Parameters& param, char* data, size_t len, e_imageType type)
    : path_("")
{
#ifdef DEBUG
    // qDebug()<<"devname: "<<param.devName_;
#endif
    QString dateStamp = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString camId = findCamId(param.devName_);
    QString sDir = QString("../../debug/%1/origin/%2/%3/%4").arg(dateStamp).arg(_CONF->user_id).arg(_CONF->time).arg(camId);
    QString img_folder =  QString("../../debug/%1/origin/%2/%3").arg(dateStamp).arg(_CONF->user_id).arg(_CONF->time);
    //qDebug()<<"img folder: "<<img_folder;
    _CONF->img_folder = img_folder;
    //qDebug()<<"sdir: "<<sDir;
    QString savefile =
            sDir +
            QString("/") +
            QDate::currentDate().toString("yyyyMMdd") + QString("_") +
            QTime::currentTime().toString("hhmmss_zzz");

    savefile += (type == e_imageType::jpg ? QString(".jpg"):
                                            type == e_imageType::ppm ? QString(".ppm"):"unknown");

    QDir d;
#ifdef DEBUG
    //  qDebug()<<"sdir: "<<sDir;
#endif
    d.mkpath(sDir);
#ifdef DEBUG
    //qDebug()<<"save file: "<<savefile;
#endif
    QFile file(savefile);
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }

    QDataStream out(&file);

    switch (type) {
    case e_imageType::jpg: {
        out.writeRawData(data, static_cast<int>(len));
    }
        break;
    case e_imageType::ppm: {
        QString sHeader = QString("P6\n%1 %2 255\n").arg(param.src_width_).arg(param.src_height_);
        out.writeRawData(sHeader.toStdString().c_str(), sHeader.length());
        out.writeRawData(data, static_cast<int>(len));
    }
        break;
    default:
        break;
    }

    if(file.isOpen())
        file.close();

    path_ = file.fileName();
}

QSharedPointer<ImageWriter> ImageWriter::ImageSave(const V4L2Parameters& param, char* data, size_t len, e_imageType type)
{
    QSharedPointer<ImageWriter> pWriter = QSharedPointer<ImageWriter>::create(param, data, len, type);
    return pWriter;
}

e_imageType ImageWriter::imageType(const QString& img)
{
    return ( img=="jpg" ? e_imageType::jpg:
                          img=="ppm" ? e_imageType::ppm:e_imageType::unknown );
}

uint32_t ImageWriter::imageFormat(const QString& img)
{
    return ( img=="jpg" ? V4L2_PIX_FMT_MJPEG:
                          img=="ppm" ? V4L2_PIX_FMT_RGB24:V4L2_PIX_FMT_YUYV);
}

QString ImageWriter::findCamId(const QString devName)
{
    for( auto &it:   _CONF->shelf_configs_["shelf_1"].cam_devices){
        if(it.second == devName){
            int cam_index =    it.first.mid(sizeof(CONFIG_CAMERA_DEVNAME) - 1).toInt();
            QString cam_id = "video";
            cam_id.append(QString::number(cam_index*2));
            return cam_id;
        }
    }
    //sean_050124
    return "0";
}
