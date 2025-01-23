#ifndef CONFIG_H
#define CONFIG_H

#include <QSettings>
#include <QProcess>
#include <QMap>
#include <QString>
#include <QFile>

#define CONFIG_WEIGHT_POLLING_INTERVAL "weight_polling_interval"
#define CONFIG_WEIGHT_THRESHOLD_NOISE "weight_threshold_noise"
#define CONFIG_WEIGHT_SMOOTH_QUEUE_SIZE "weight_smooth_queue_size"
#define CONFIG_WEIGHT_SETTLING_COUNTS "weight_settling_counts"

#define CONFIG_SHELF_NAME "shelf_"
#define CONFIG_CAMERA_DEVNAME "camera_dev"
#define CONFIG_WEIGHT_DEVNAME "weight_dev"
#define CONFIG_MT_DEVNAME "mt_dev"

#define CONFIG_WEIGHT_ID "weight_id"
#define CONFIG_PHOTO_IDNAME "photo_id"

#define CONFIG_CARMERA_SRC_WIDTH "camera_src_width"
#define CONFIG_CARMERA_SRC_HEIGHT "camera_src_height"
#define CONFIG_CARMERA_DST_WIDTH "camera_dest_width"
#define CONFIG_CARMERA_DST_HEIGHT "camera_dest_height"
#define CONFIG_CARMERA_EXPOSURE "camera_exposure"
#define CONFIG_CARMERA_FPS "camera_fps"
#define CONFIG_CARMERA_CAPTURE_FORMAT "camera_capture_format" // "jpg" or "ppm"
#define CONFIG_CARMERA_BUFFER_COUNTS "camera_buffer_counts"
#define CONFIG_CARMERA_RECOGNITION_DELAY "camera_recognition_delay" // ms
//add camera shift counts for ring buffer
#define CONFIG_CARMERA_SHIFT_COUNTS "camera_shift_counts"

//debug
#define CONFIG_DEBUG_NAME "debug"
#define CONFIG_DEBUG_OBJ_ID "obj_id"
#define CONFIG_PHOTO_INDEX "photo_index"
#define CONFIG_ERROR_CASE_NUM "error_num"

//avg weight value
#define CONFIG_AVG_WEIGHT "avg_weight"
// snack shelf
#define SNACK
//decision error
#define CONFIG_DEVICE_ID "device_id"
//mdlist
#define CONFIG_MDLIST_PATH "mdlist_path"
//ioboard version
#define CONFIG_IOBOARD_VERSION "ioboard_version"
#define CONFIG_PROJECT_NAME "project_name"

typedef struct {
    int weight_id;
    std::vector<QString> mcu_devices;
    std::map<QString, QString> cam_devices;
    std::map<int, QString> weight_devices;
    std::map<int, int> weight_ids;
} shelf;


class Config
{
public:
    Config();
    ~Config();

    void init(QString path);

    bool load();
    bool save();
    bool load_params();

public:
    unsigned int weight_polling_interval_;
    unsigned int weight_threshold_noise_;
    unsigned int weight_smooth_queue_size_;
    unsigned int weight_settling_count_;

    unsigned int threshold_mcu_;
    unsigned int threshold_weight_;

    unsigned int camera_src_width_;  // source capture width
    unsigned int camera_src_height_; // source capture height
    unsigned int camera_dst_width_;  // dest
    unsigned int camera_dst_height_;
    unsigned int camera_exposure_;         // camera exposure
    unsigned int camera_fps_;              // camera fps
    unsigned int camera_buffer_counts_;    // mmap buffer count
    unsigned int camera_recognition_delay; // delay for camera power input which
    //add camera shift counts for ring buffer
    int cameras_shift_counts_;
    // is set up through ioboard power
    // relay.
    QString camera_captureformat_; // capture format "jpg" or "ppm"
    QString weight_device_;

    QMap<QString, shelf> shelf_configs_;

    int photo_index_;

    // snack shelf
    std::vector<int> weight_ids;
    QString user_id;

    // decision error
    int device_id = 0;
    QString img_folder;
    QString time;

    //mdlist
    QString mdlist_path;

    int shelfCounts();
    int camCounts(unsigned int shelf_no);
    int totalCamCounts();

    //ioboard version
    QString ioboard_version_;
	QMap<QString, QMap<QString, int>> all_cams_params;
    QString project_name_;

private:
    QSettings *settings_;
};

#endif // CONFIG_H
