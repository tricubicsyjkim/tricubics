#ifndef SMARTVM_H
#define SMARTVM_H

#include "SignalActorImpl.h"
#include "WeightManager.h"
#include "CameraManager.h"
//#include "Purchase.h"
#include <QMap>
#include <QVector>
//#include "MTManager.h"
#include "HttpsManager.h"
#include <QJsonObject>
#include <QJsonDocument>
#include "AudioManager.h"
#include <QTimer>
#include <QDateTime>
#include "ParamsMonitor.h"
#include "DoorCtrl.h"
#include "ButtonCtrl.h"
#include "ButtonCtrlManager.h"
#include "DoorCtrlManager.h"

enum photo_state
{
    PHOTO_STATE_OFF = 0,
    PHOTO_STATE_ON,
    PHOTO_STANDBY_OFF,
    PHOTO_STANDBY_ON,
};

const QString DOOR_OPENED_= "openDoor";
const QString DOOR_CLOSED_ = "closeDoor";
const QString RELEASE_INFO_ = "releaseInfo";
const QString CHECK_STATUS_="checkStatus";
const QString QUERY_PRODUCT_="queryProduct";
const QString DEVICE_OPEN_="deviceOpen";

#define DEBUG
#define TOTAL_USB_NUM 12
#define USE_IOBOARD
#define USE_SERVER
//#define JUST_CAPTURE
#define ZEROMQ
#define TIMEOUT 120
#define CAMERA_CAPTURE_DELAY 10
#define SHELF_NUM 4


class SmartVM : public SignalActorImpl
{
    Q_OBJECT

public:
    explicit SmartVM(QObject *parent);
    ~SmartVM();

    virtual void run(){};
    virtual void on_finalize(int type);
    virtual void on_updated(int type);

signals:
    void decision_processed(const int &);
    void open_cam_process(const QString &);
    void data_analysis_proc(const vision_out_t &);
    void gpio_open_door();
    void gpio_close_door();
    // play audio
    void play_audio(const int &);

public slots:
    void on_init();
    void on_delayed_cam_run(int id);
    void on_quit();
    void on_door_opened();
    void on_door_closed();
    void on_usb_port_opened(const int& port_id);
    void on_aivison_processed(const vision_out_t &);
    void on_weight_trigger_proc(const int &, const int &, const int &, const int &);    //mt_index, shelf_no, weight_index, delta

    //https server request
    void on_https_processed(const QString &);
    void on_shopping_finished_proc(const QString &);

    // devices have something wrong
    void on_device_error(const QString&, const int &);

    void on_transmit_image(const QString&, const int &);
    void on_transmit_image(int, const QString&, const int &);

    //event off, release image counts
    void on_event_off_proc();

    //  covert QString event_type to int
    EVENT_TYPE  resolveEventType(QString event);

    //audio timeout
    void on_audio_timeout();

    //weight trigger
    void on_loadcell_trigger(WeightChangedInfo changedInfo);
    void on_loadcell_trigger_4vision(WeightChangedInfo changedInfo);

    // lotte
    void on_lock_all_opened();
    void on_deadbolt_error();
    QString get_sensor_type(QString dev_name);

    void on_reset_deadbolt();
    void loop_open_door();
    void on_button_pressed();
    void on_button_released();
    void on_gpio_opened(const bool);
    void on_opened_door_timeout();
    void setTimer(int duration);
private:
    WeightManager weight_manager_;
    CameraManager camera_manager_;
    //    PurchaseManager purchase_manager_;
    QMap<int, photo_state > photo_state_;
    //    MTManager mt_manager_;
    HttpsManager https_manager_;
    //add docker ports to config file
    AudioManager audio_manager_;
    ParamsMonitor params_monitor_;
    int reveive_counter = 0;
    int send_counter = 0;
    std::vector<int> running_shelves;
    QString img_path_filename;
    int mt_status = PHOTO_STATE_OFF;

    //https server request
    QString current_user = "null";
    std::map<QString, int> error_infos;
    //find multi-touch event id
    std::vector<int> events_before;
    std::vector<int> events_after;
    std::vector<int> events_diff;
    //mtdev  index
    int mt_index = 0;
    std::map<int, int> reveive_counter_map;
    std::map<int, int> send_counter_map;
    int photo_index = 1;
    QTimer mt_off_timer_;
    bool door_opened = false;
    QTimer audio_timer_;
    int play_audio_times = 0;
    int loadcell_nums_ = 0;
    std::map<int,int> loadcell_weights_; // weightID, weightValue
    //weight trigger
    std::vector<WeightChangedInfo> changedInfos;
    std::vector<WeightChangedInfo> changedInfosVsion;
    bool device_opened = false;
    QString vendor = NULL;
    QString device_id = "";
    int tIndex = 1;
//    DoorCtrl* door_ctrl_;
    //    ButtonCtrl* button_ctrl_;
    ButtonCtrlManager button_manager_;
    DoorCtrlManager door_manager_;
    QTimer opened_door_timer_;
    int timeout_ = 10000;
    bool isTimeout = false;
    bool isCapture = false;
};

#endif // SMARTVM_H
