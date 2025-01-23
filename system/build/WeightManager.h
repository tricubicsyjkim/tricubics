#ifndef WEIGHTMANAGER_H
#define WEIGHTMANAGER_H

#include "SensorDAQImpl.h"
#include "Weight_rs232.h"
#include <QMap>
#include <QTimer>
#include <QElapsedTimer>
#include <QDate>
#include <QTime>
#include "AIVsionDefines.h"
#include <limits>

#define SNACK
#define CLOSE_DOOR_T  10

class WeightManager : public SensorDAQImpl
{
    Q_OBJECT

public:
    explicit WeightManager();
    ~WeightManager();

    void start();
    void stop();
    void capture_start(int shelf_no, int mt_index);
    void multi_capture_start(int shelf_no, int mt_index);
    void multi_capture_end(int shelf_no, int mt_index);
    int get_weight_sensor_id(int shelf_no);
    //snack shelf
    std::map<int, int> get_weight_sensor_ids(int shelf_no);
    void ioboard_usb_on();
    void ioboard_usb_off();
    void ioboard_usb_reset();
    void ioboard_door_on();
    void ioboard_lock_on();
    void ioboard_door_off();
    void ioboard_lock_off();
    void set_device_opened(bool device_opened_);

    //weight trigger
    void check_weight(int weight, int scale_id);
    int get_shelf_id(int scale_id);
    void whether_start_trigger(int weight, int scale_id);
    void   whether_stop_trigger(int weight, int scale_id);
    WeightChangedInfo check_start_time(int shelf_id);
    void init_weight_trigger();
    void init_error_prop();
    void set_timer(int duration);
    int get_lock_state();
signals:
    void weight_trigger_proc(const int &, const int &);
    void weight_trigger_proc(const int &, const int &, const int &);
    void weight_trigger_proc(const int &, const int &,  const int &, const int &);
    void usb_port_opened(const int &);
    void weight_sensor_error(const QString&, const int &);
    void door_closed();
    void door_opened();
    void loadcell_trigger(WeightChangedInfo);
    void loadcell_trigger_4vision(WeightChangedInfo);
    void reset_error_prop();
    void lock_all_opened();
    void deadbolt_error();
    void reset_deadbolt();
public slots:

    void on_weight(int scale_id, int value);
    void on_delta_triggered(int, int);
    void on_delta_triggered(int, int, int);
    //snack shelf
    void on_delta_triggered(int, int, int, int);  //mt_index, shelf_no, weight_index, delta
    void on_mcu_alive();
    void on_usb_on();
    void on_usb_off();
    void on_usb_reset();
    void on_usb_status(int port_id);
    void on_door_open();
    void on_door_close();
    void on_usb_disconnected();
    void on_ioboard_polling();
    void on_polling();
    void on_lock_all_opened();
    void on_deadbolt_error();
    void on_reset_deadbolt();
private:
    Weight_rs232 *weight_serial_;
    QMap<int, QMap<int, int>>  weight_before;
    //snack shelf
    QMap<int, QMap<int, QMap<int, int>>> weights_before;
    QMap<int, QMap<int, int>> weight_after;
    //snack shelf
    QMap<int, QMap<int, QMap<int, int>>> weights_after;
    QMap<int, QMap<int, QMap<int, int>>> weights_delta;
    QTimer reset_cmd_timer_;
    QTimer polling_timer_;
    int polling_count = 0;
    //door lock
    QElapsedTimer door_opened_t;
    bool door_is_opened = false;

    //weight trigger
    QMap<int, int> weight_values; // weight_id, weight
    QMap<int, bool> weight_changed; //weight_id, bool
    QMap<int, bool> shelf_trigger; // shelf_id, bool
    QMap<int, int> weight_stable_times; //weight_id, times
    QMap<int, int> loadcell_before; //weight_id, weight value
    QMap<int, WeightChangedInfo> changed_info;
    QMap<int, WeightChangedInfo> changed_info_4vision;
    QMap<int, QVector<WeightChangedInfo>> changed_info_shelf;
    QMap<int, qint64> loadcell_start_time;
    QMap<int, int> max_delta_weights;  //weight_id, max_delta_weight
    QMap<int, int> total_noise_weight;
    QMap<int, bool> beside_weight_changed; //weight_id, bool
    //remove the interval wait for weight sensor stable
    bool device_opened = false;
};

#endif // WEIGHTMANAGER_H
