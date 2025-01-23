#include "WeightManager.h"
#include "Instance.h"
#include <QDebug>
#include "Purchase.h"

QElapsedTimer weight_timer;
WeightManager::WeightManager()
{
    weight_serial_ = new Weight_rs232(_CONF->weight_device_);
    connect(weight_serial_,
            SIGNAL(weight_value(const int &, const int &)),
            this,
            SLOT(on_weight(const int &, const int &)),
            Qt::QueuedConnection);

    connect(weight_serial_,
            SIGNAL(mcu_alive()),
            this,
            SLOT(on_mcu_alive()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(usb_on()),
            this,
            SLOT(on_usb_on()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(usb_off()),
            this,
            SLOT(on_usb_off()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(usb_reset()),
            this,
            SLOT(on_usb_reset()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(usb_status(int)),
            this,
            SLOT(on_usb_status(int)),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(door_opened()),
            this,
            SLOT(on_door_open()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(door_closed()),
            this,
            SLOT(on_door_close()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(usb_disconnected()),
            this,
            SLOT(on_usb_disconnected()),
            Qt::QueuedConnection);
    connect(this,
            SIGNAL(reset_error_prop()),
            weight_serial_,
            SLOT(on_reset_error_prop()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(lock_all_opened()),
            this,
            SLOT(on_lock_all_opened()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(deadbolt_error()),
            this,
            SLOT(on_deadbolt_error()),
            Qt::QueuedConnection);
    connect(weight_serial_,
            SIGNAL(reset_deadbolt()),
            this,
            SLOT(on_reset_deadbolt()),
            Qt::QueuedConnection);
    reset_cmd_timer_.setSingleShot(true);
    connect(&reset_cmd_timer_, SIGNAL(timeout()), this, SLOT(on_ioboard_polling()));
    connect(&polling_timer_, SIGNAL(timeout()), this, SLOT(on_polling()));
}

WeightManager::~WeightManager()
{
    delete weight_serial_;
}



void WeightManager::start()
{
    for (auto id_ : _CONF->weight_ids) {
        int id = id_;
        //qDebug()<<"weightid: "<<id;
        if (id < 0)
            continue;

        weight_serial_->add_scale_id(id);
        add_sensor(id, DAQ_MANUAL,
                   static_cast<int>(_CONF->weight_threshold_noise_),
                   static_cast<int>(_CONF->weight_settling_count_),
                   static_cast<int>(_CONF->weight_smooth_queue_size_)
                   );
    }
    weight_serial_->start();
}

void WeightManager::stop()
{
}

void WeightManager::capture_start(int shelf_no, int mt_index)
{
    int weight_id =  get_weight_sensor_id(shelf_no);
    qDebug()<<"shelf no: "<<shelf_no;
    qDebug()<<"weight id: "<<weight_id;
    daq_data *p = get_sensor_ptr(weight_id);
    if (!p)
        return;
    //weight_before[shelf_no] = p->last_stable_value;
    weight_before[mt_index][shelf_no] = p->capture_value;
    qDebug()<<"mt_index: "<<mt_index<<" , weight_before : "<<  weight_before[mt_index][shelf_no];
}

void WeightManager::multi_capture_start(int shelf_no, int mt_index)
{
    std::map<int, int> weight_ids =  get_weight_sensor_ids(shelf_no);
    for(auto weight_pair : weight_ids){
        int weight_index = weight_pair.first;
        int weight_id = weight_pair.second;
        qDebug()<<"shelf no: "<<shelf_no<<" , weight index: "<<weight_index <<" , weight id: "<<weight_id;
        daq_data *p = get_sensor_ptr(weight_id);
        if (!p){
            return;
        }else{
            qDebug()<<"found weight pointer";
            weights_before[mt_index][shelf_no][weight_index] = p->capture_value;
            qDebug()<<"weight capture value: "<<p->capture_value;
            qDebug()<<"mt_index: "<<mt_index<< "weight index: "<<weight_index<<" , weight_before : "<<  weights_before[mt_index][shelf_no][weight_index];
        }
    }
}

void WeightManager::multi_capture_end(int shelf_no, int mt_index)
{
    std::map<int, int> weight_ids =  get_weight_sensor_ids(shelf_no);
    for(auto weight_pair : weight_ids){
        int weight_index = weight_pair.first;
        int weight_id = weight_pair.second;
        daq_data *p = get_sensor_ptr(weight_id);
        if (!p){
            return;
        }else{
            qDebug()<<"found weight pointer";
            weights_after[mt_index][shelf_no][weight_index] = p->capture_value;
            qDebug()<<"weight capture value: "<<p->capture_value;
            qDebug()<<"mt_index: "<<mt_index<<"shelf no: "<<shelf_no<< "weight index: "<<weight_index<<" , weight id: "<<weight_id<<" , weight_after : "<<  weights_after[mt_index][shelf_no][weight_index];
            qDebug()<<"total_noise_weight: "<<total_noise_weight[weight_id];
            qDebug()<<"original delta weight: "<<weights_after[mt_index][shelf_no][weight_index] - weights_before[mt_index][shelf_no][weight_index];
            int delta = weights_after[mt_index][shelf_no][weight_index] - weights_before[mt_index][shelf_no][weight_index] - total_noise_weight[weight_id];
            qDebug()<<"after filter noise, delta weight:  "<<delta;
            emit value_of_steady_state(mt_index, shelf_no, weight_index, delta);
        }
    }
}

int WeightManager::get_weight_sensor_id(int shelf_no)
{
    QString shelf_no_str = "shelf_" + QString::number(shelf_no,10);
    int weight_id =  _CONF->shelf_configs_[shelf_no_str].weight_id;
    return weight_id;
}

std::map<int, int> WeightManager::get_weight_sensor_ids(int shelf_no)
{
    QString shelf_no_str = "shelf_" + QString::number(shelf_no,10);
    qDebug()<<"shelf_no_str: "<<shelf_no_str;
    std::map<int, int> weight_ids =  _CONF->shelf_configs_[shelf_no_str].weight_ids;
    return weight_ids;
}

void WeightManager::ioboard_usb_on()
{
    weight_serial_->send_command_req(0x10);
}

void WeightManager::ioboard_usb_off()
{
    weight_serial_->send_command_req(0x11);
}

void WeightManager::ioboard_usb_reset()
{
    weight_serial_->send_command_req(0x12);
    qDebug()<<"ioboard_usb_reset";
    reset_cmd_timer_.start();
}

void WeightManager::ioboard_door_on()
{
    on_polling();
    qDebug()<<"send open door cmd" ;
}

void WeightManager::ioboard_lock_on()
{
    weight_serial_->send_command_relay(0x41);
}

void WeightManager::ioboard_door_off()
{
    weight_serial_->send_command_req(0x20);
}

void WeightManager::ioboard_lock_off()
{
    weight_serial_->send_command_relay(0x61);
}

void WeightManager::set_device_opened(bool device_opened_)
{
    device_opened = device_opened_;
    qDebug()<<"device_opened: "<<device_opened;
}

void WeightManager::check_weight(int weight, int scale_id)
{
    if(weight_values[scale_id] != 0){
        //        int shelf_id = get_shelf_id(scale_id);

        if(weight_changed[scale_id] == false){
            if(door_is_opened == true && device_opened == false){
                whether_start_trigger(weight, scale_id);
                loadcell_start_time[scale_id] = QDateTime::currentMSecsSinceEpoch();
            }
        }else if(weight_changed[scale_id] ==  true){
            whether_stop_trigger(weight, scale_id);
        }
    }else{
        qDebug()<<"init weight_values--- scale_id: "<<scale_id<<" , weight: "<<weight;
    }
    weight_values[scale_id] = weight;
}

int WeightManager::get_shelf_id(int scale_id)
{
    int shelf_id =0 ;;
    if(scale_id % 2 == 0){
        shelf_id = scale_id/2;
    }else{
        shelf_id = scale_id/2 +1;
    }
    return  shelf_id;
}

void WeightManager::whether_start_trigger(int weight, int scale_id)
{
    int last_weight =  weight_values[scale_id];
    int shelf_id = get_shelf_id(scale_id);
    if(abs(last_weight - weight) > 5 && weight_changed[scale_id] == false ){
        qDebug()<<"last_weight: "<<last_weight<<" , weight: "<<weight;
        qDebug()<<"shelf_id: "<<shelf_id<<" , scale_id:"<<scale_id;
        max_delta_weights[scale_id] = weight - last_weight;
        qDebug()<<"first_changed_value: "<< (weight - last_weight);
        qDebug()<<"max_delta_weight: "<<max_delta_weights[scale_id]<<" , scale_id: "<< scale_id;
        qDebug()<<"send start_trigger";
        shelf_trigger[shelf_id] = true;
        weight_changed[scale_id] = true;
        loadcell_before[scale_id] = last_weight;
        WeightChangedInfo info, info_vision;
        info_vision.start_time = info.start_time = loadcell_start_time[scale_id];
        changed_info[scale_id] = info;
        info_vision.first_changed_value = weight - last_weight;
        changed_info_4vision[scale_id] = info_vision;

        //        qDebug()<<"whether_start_trigger  start_time: "<<changed_info[scale_id].start_time;
        qDebug()<<"weight_start_change: "<<QDateTime::fromMSecsSinceEpoch(loadcell_start_time[scale_id]);
        //        emit start_trigger(shelf_id);
        int beside_weight_id = (scale_id % 2 == 0 ? scale_id -1: scale_id +1);
        if(weight_changed[beside_weight_id] == true){
            beside_weight_changed[beside_weight_id] = true;
            qDebug()<<"beside loadcell have started to change";
        }else{
            beside_weight_changed[beside_weight_id] = false;
            qDebug()<<"beside loadcell does not chang";
        }
    }
}

void WeightManager::whether_stop_trigger(int weight, int scale_id)
{
    int last_weight =  weight_values[scale_id];
    int shelf_id = get_shelf_id(scale_id);
    int tmp_delta = weight - loadcell_before[scale_id];
    if(abs(tmp_delta) > abs(max_delta_weights[scale_id])){
        max_delta_weights[scale_id] = tmp_delta;
    }
    if(abs(last_weight - weight) < 10){
        if(weight_stable_times[scale_id] >= 8){
            qDebug()<<"whether_stop_trigger";
            qDebug()<<"last_weight: "<<last_weight<<" , weight: "<<weight;
            weight_stable_times[scale_id] = 0;
            weight_changed[scale_id] = false;
            changed_info[scale_id].shelf_id = shelf_id;
            changed_info_4vision[scale_id].end_time = changed_info[scale_id].end_time = QDateTime::currentMSecsSinceEpoch();
            qDebug()<<"weight_stop_change: "<<QDateTime::currentDateTime();
            changed_info_4vision[scale_id].delta_weight = changed_info[scale_id].delta_weight = weight - loadcell_before[scale_id];
            changed_info_4vision[scale_id].weight_id = changed_info[scale_id].weight_id = scale_id;
            float scale = static_cast<float>(changed_info_4vision[scale_id].first_changed_value) / static_cast<float>(changed_info_4vision[scale_id].delta_weight);
            qDebug()<<"scale: "<<scale;
            changed_info_4vision[scale_id].start_time -= 450;
            changed_info_4vision[scale_id].end_time -= 150;
            qDebug()<<"start time in stop trigger: "<< changed_info[scale_id].start_time;
            changed_info_shelf[shelf_id].push_back(changed_info[scale_id]);
            int beside_weight_id = (scale_id % 2 == 0 ? scale_id -1: scale_id +1);
            if(weight_changed[beside_weight_id] == false){
                // emit to smartvm;
                shelf_trigger[shelf_id] = false;
                WeightChangedInfo info =   check_start_time(shelf_id);
                qDebug()<<"door_is_opened: "<<door_is_opened;
                emit loadcell_trigger(info);
                changed_info_shelf[shelf_id].clear();
            }

            if(abs(max_delta_weights[scale_id]) <= 15){
                qDebug()<<"it is noise--- "<< " max_delta_weight: "<<max_delta_weights[scale_id]<< " , scale_id: "<<scale_id;
                total_noise_weight[scale_id] += max_delta_weights[scale_id];

            }else{
                emit loadcell_trigger_4vision(changed_info_4vision[scale_id]);
            }
        }else{
            weight_stable_times[scale_id] += 1;
            qDebug()<<"weight_stable_times: "<<weight_stable_times[scale_id]<<"  , scale : "<<scale_id;
        }
    }else{
        weight_stable_times[scale_id] = 0;
        qDebug()<<"reset weight_stable_times";
    }
}

WeightChangedInfo WeightManager::check_start_time(int shelf_id)
{
    QVector<WeightChangedInfo> infos = changed_info_shelf[shelf_id];
    QVector<WeightChangedInfo>::iterator iter;
    qint64 min_start_time = 9223372036854775807;
    qint64 max_end_time = std::numeric_limits<qint64>::min();
    for (iter=infos.begin();iter!=infos.end();iter++){
        int delta_weight = iter->delta_weight;
        qDebug()<<"delta_weight: "<<delta_weight;
        if(delta_weight > 5){
            iter->end_time += 10;
            iter->start_time -= 2200;
        }

        if(delta_weight < 5){
            iter->start_time -= 1200;
            iter->end_time += 1010;
        }

        if(iter->start_time < min_start_time){
            min_start_time = iter->start_time;
        }

        if(iter->end_time > max_end_time){
            max_end_time = iter->end_time;
        }
    }
    qDebug()<<"min_start_time: "<<QDateTime::fromMSecsSinceEpoch(min_start_time);
    qDebug()<<"min_start_time int : "<< min_start_time;
    qDebug()<<"max_end_time: "<<QDateTime::fromMSecsSinceEpoch(max_end_time);
    WeightChangedInfo info;
    info.shelf_id = shelf_id;
    info.start_time = min_start_time;
    info.end_time = max_end_time;
    return  info;
}

void WeightManager::init_weight_trigger()
{
    weight_values.clear();
    weight_changed.clear();
    shelf_trigger.clear();
    weight_stable_times.clear();
    loadcell_before.clear();
    changed_info.clear();
    changed_info_4vision.clear();
    changed_info_shelf.clear();
    loadcell_start_time.clear();
    total_noise_weight.clear();
}

void WeightManager::init_error_prop()
{
    emit reset_error_prop();
}

void WeightManager::set_timer(int duration)
{
    weight_serial_->set_timer(duration);
}

int WeightManager::get_lock_state()
{
    int lock_state = weight_serial_->get_lock_state();
    qDebug()<<"get lock state: "<< lock_state;
    return lock_state;
}

void WeightManager::on_weight(int scale_id, int value)
{
    int weight;
    weight = value;
#if 1
    if(door_is_opened)
        qDebug("OnWeight: %d , scale id: %d", weight, scale_id);
#endif
    check_weight(weight, scale_id);
    updateData(scale_id, weight);
}

void WeightManager::on_delta_triggered(int shelf_no, int delta)
{
    emit weight_trigger_proc(shelf_no,delta );
    qDebug(" * on_delta_triggered: [%d]---> %d", shelf_no, delta);
}

void WeightManager::on_delta_triggered(int mt_index, int shelf_no, int delta)
{
    emit weight_trigger_proc(mt_index, shelf_no,delta );
    qDebug(" * on_delta_triggered: [%d][%d]---> %d", mt_index, shelf_no, delta);
}

void WeightManager::on_delta_triggered(int mt_index, int shelf_no, int weight_index, int delta)
{
    emit weight_trigger_proc(mt_index, shelf_no,weight_index, delta);
    qDebug(" * on_delta_triggered: mt_index[%d]shelf_no[%d]weight_index[%d]---> %d", mt_index, shelf_no, weight_index, delta);
}

void WeightManager::on_mcu_alive()
{
    qDebug()<<"mcu alive";
}

void WeightManager::on_usb_on()
{
    qDebug()<<"usb on";
}

void WeightManager::on_usb_off()
{
    qDebug()<<"usb off";
}

void WeightManager::on_usb_reset()
{
    reset_cmd_timer_.stop();
    qDebug()<<"usb reset";
}

void WeightManager::on_usb_status(int port_id)
{
    qDebug()<<"on_usb_status"<< port_id;
    emit usb_port_opened(port_id);
}

void WeightManager::on_door_open()
{
    if(!door_is_opened){
        qDebug()<<"door open";
        door_is_opened = true;
        emit door_opened();
        polling_timer_.stop();
    }
}

void WeightManager::on_door_close()
{
    qDebug()<<"on_door_close : "<<door_is_opened;
    emit door_closed();
    door_is_opened = false;
}

void WeightManager::on_usb_disconnected()
{
    qDebug()<<"weight sensor usb diconnected";
    emit weight_sensor_error(_CONF->weight_device_, 0);
}

void WeightManager::on_ioboard_polling()
{
    qDebug()<<"reset ioboard again";
    weight_serial_->send_command_req(0x12);
    reset_cmd_timer_.start(5000);
}

void WeightManager::on_polling()
{
    weight_serial_->send_command_req(0x21);
    weight_serial_->open_door_timeout();
}

void WeightManager::on_lock_all_opened()
{
    qDebug()<<"lock all opened";
    emit lock_all_opened();
}

void WeightManager::on_deadbolt_error()
{
    qDebug()<<"deadbolt error";
    emit deadbolt_error();
}

void WeightManager::on_reset_deadbolt()
{
    emit reset_deadbolt();
}
