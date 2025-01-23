#include <QDebug>
#include <QProcess>
#include <QTime>

#include <unistd.h>

#include "Defines.h"
#include "Config.h"


Config::Config()
{
    settings_ = NULL;
}

Config::~Config()
{
    save();
    delete settings_;
}

void Config::init(QString path)
{
    if (settings_)
        delete settings_;

    settings_ = new QSettings(path, QSettings::IniFormat);
    load();
    load_params();
}

bool Config::load()
{
    settings_->sync();

    shelf_configs_.clear();

    weight_polling_interval_ = settings_->value(CONFIG_WEIGHT_POLLING_INTERVAL, 50).toUInt();
    weight_threshold_noise_ = settings_->value(CONFIG_WEIGHT_THRESHOLD_NOISE, 10).toUInt();
    weight_smooth_queue_size_ = settings_->value(CONFIG_WEIGHT_SMOOTH_QUEUE_SIZE, 10).toUInt();
    weight_settling_count_ = settings_->value(CONFIG_WEIGHT_SETTLING_COUNTS, 10).toUInt();

    weight_device_ = settings_->value(CONFIG_WEIGHT_DEVNAME, "").toString();
    camera_src_width_ = settings_->value(CONFIG_CARMERA_SRC_WIDTH, 1920).toUInt();
    camera_src_height_ = settings_->value(CONFIG_CARMERA_SRC_HEIGHT, 1080).toUInt();
    camera_dst_width_ = settings_->value(CONFIG_CARMERA_DST_WIDTH, 960).toUInt();
    camera_dst_height_ = settings_->value(CONFIG_CARMERA_DST_HEIGHT, 540).toUInt();
    camera_exposure_ = settings_->value(CONFIG_CARMERA_EXPOSURE, 8).toUInt();
    camera_fps_ = settings_->value(CONFIG_CARMERA_FPS, 10).toUInt();
    camera_buffer_counts_ = settings_->value(CONFIG_CARMERA_BUFFER_COUNTS, 10).toUInt();
    camera_captureformat_ = settings_->value(CONFIG_CARMERA_CAPTURE_FORMAT, "jpg").toString();

    camera_recognition_delay = settings_->value(CONFIG_CARMERA_RECOGNITION_DELAY, "3000").toUInt();
    //add camera shift counts for ring buffer
    cameras_shift_counts_ = settings_->value(CONFIG_CARMERA_SHIFT_COUNTS, 10).toInt();

    photo_index_ = settings_->value(CONFIG_PHOTO_INDEX, 1).toInt();

    //device id
    device_id = settings_->value(CONFIG_DEVICE_ID, 0 ).toInt();

    //mdlist path
    mdlist_path = settings_->value(CONFIG_MDLIST_PATH, "").toString();

    //ioboard version
    ioboard_version_ = settings_->value(CONFIG_IOBOARD_VERSION, "").toString();
    qDebug()<<"ioboard version: "<<ioboard_version_;
    project_name_ = settings_->value(CONFIG_PROJECT_NAME, "").toString();

    QStringList groups = settings_->childGroups();

    for (auto s : groups) {
        qDebug("groups");
        qDebug()<<s;
        if ( (s.mid(0, QString(CONFIG_SHELF_NAME).size()) != CONFIG_SHELF_NAME))
            continue;

        shelf shelfdata;
        settings_->beginGroup(s);
        QStringList keys = settings_->childKeys();
        for (auto k : keys) {
            if(k.mid(0,QString(CONFIG_WEIGHT_ID).size())==CONFIG_WEIGHT_ID) {
                int weight_index = k.mid(QString(CONFIG_WEIGHT_ID).size()).toInt();
                qDebug()<<"weight index: "<<weight_index;
                int  keyVal = settings_->value(k, -1).toInt();
                shelfdata.weight_ids[weight_index] = keyVal;
                weight_ids.push_back(keyVal);
                qDebug()<<"setting file  weight id: "<<keyVal;
            }

            if (k.mid(0, QString(CONFIG_CAMERA_DEVNAME).size()) == CONFIG_CAMERA_DEVNAME) {
                QString keyVal = settings_->value(k, "").toString();
                shelfdata.cam_devices[k] = keyVal;
            }
        }

        if((s.mid(0, QString(CONFIG_SHELF_NAME).size()) == CONFIG_SHELF_NAME))
            shelf_configs_[s] = shelfdata;

        settings_->endGroup();

    }

    settings_->sync();

    return true;
}

bool Config::save()
{
    settings_->setValue(CONFIG_WEIGHT_POLLING_INTERVAL, weight_polling_interval_);
    settings_->setValue(CONFIG_WEIGHT_THRESHOLD_NOISE, weight_threshold_noise_);
    settings_->setValue(CONFIG_WEIGHT_SMOOTH_QUEUE_SIZE, weight_smooth_queue_size_);
    settings_->setValue(CONFIG_WEIGHT_SETTLING_COUNTS, weight_settling_count_);

    settings_->setValue(CONFIG_WEIGHT_DEVNAME, weight_device_);

    settings_->setValue(CONFIG_CARMERA_SRC_WIDTH, camera_src_width_);
    settings_->setValue(CONFIG_CARMERA_SRC_HEIGHT, camera_src_height_);
    settings_->setValue(CONFIG_CARMERA_DST_WIDTH, camera_dst_width_);
    settings_->setValue(CONFIG_CARMERA_DST_HEIGHT, camera_dst_height_);
    settings_->setValue(CONFIG_CARMERA_EXPOSURE, camera_exposure_);
    settings_->setValue(CONFIG_CARMERA_FPS, camera_fps_);
    settings_->setValue(CONFIG_CARMERA_BUFFER_COUNTS, camera_buffer_counts_);
    settings_->setValue(CONFIG_CARMERA_CAPTURE_FORMAT, camera_captureformat_);

    settings_->setValue(CONFIG_CARMERA_RECOGNITION_DELAY, camera_recognition_delay);
    //add camera shift counts for ring buffer
    settings_->setValue(CONFIG_CARMERA_SHIFT_COUNTS, cameras_shift_counts_);

    settings_->setValue(CONFIG_PHOTO_INDEX, photo_index_);

    //device id
    settings_->setValue(CONFIG_DEVICE_ID, device_id);

    //mdlist path
    settings_->setValue(CONFIG_MDLIST_PATH, mdlist_path);

    //ioboard version
    settings_->setValue(CONFIG_IOBOARD_VERSION, ioboard_version_);
    settings_->setValue(CONFIG_PROJECT_NAME, project_name_);

    for (auto shelf : shelf_configs_.toStdMap()) {
        settings_->beginGroup(shelf.first);
        for(auto i : shelf.second.weight_devices){
            settings_->setValue(QString("%1%2").arg(CONFIG_WEIGHT_DEVNAME).arg(i.first), i.second);
        }
#ifdef SNACK
        for(auto i : shelf.second.weight_ids){
            settings_->setValue(QString("%1%2").arg(CONFIG_WEIGHT_ID).arg(i.first), i.second);
        }
#else
        settings_->setValue(CONFIG_WEIGHT_ID, shelf.second.weight_id);
#endif
        for (auto it : shelf.second.cam_devices) {
            settings_->setValue(it.first, it.second);
        }
        settings_->endGroup();
    }

    settings_->sync();

    sync();

    return true;
}

bool Config::load_params()
{
    settings_->sync();
    std::map<QString, QString> cam_devices_ = shelf_configs_["shelf_1"].cam_devices;

    for(auto iter = cam_devices_.begin(); iter != cam_devices_.end(); iter++) {
        QMap<QString, int> cam_params;
        qDebug() << iter->first << " --- " << iter->second;
        settings_->beginGroup(iter->first);
        QStringList keys = settings_->childKeys();
        for (auto k : keys) {
            int  keyVal = settings_->value(k).toInt();
            qDebug()<<k<< " : "<< keyVal;
            cam_params.insert(k, keyVal);
        }
        all_cams_params.insert(iter->second, cam_params);
        settings_->endGroup();
        settings_->sync();
    }
    return true;
}

int Config::shelfCounts()
{
    return shelf_configs_.size();
}

int Config::camCounts(unsigned int shelf_no)
{
    QString shelf_str = QString("%1%2").arg(CONFIG_SHELF_NAME).arg(shelf_no);
    return static_cast<int>(shelf_configs_[shelf_str].cam_devices.size());
}

int Config::totalCamCounts()
{
    int counts = 0;
    for (auto shelf : shelf_configs_.toStdMap()) {
        settings_->beginGroup(shelf.first);
        counts += shelf.second.cam_devices.size();
        settings_->endGroup();
    }
    return counts;
}
