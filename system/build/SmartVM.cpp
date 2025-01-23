#include "SmartVM.h"
#include <cstdlib>
#include <csignal>
#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include "Instance.h"
#include <QMap>
#include "Defines.h"
#include <CSingleTon.h>
#include <QtLogWrapper.h>
#include <QByteArray>

QElapsedTimer full_darknet_timer;
QElapsedTimer photo_off_timer;
QElapsedTimer cycle_interval_t;
QElapsedTimer one_cycle_t;

SmartVM::SmartVM(QObject *parent)
{
    Q_UNUSED(parent);
    qDebug() << "main thread id: " << QThread::currentThreadId();
    connect(&https_manager_,
            SIGNAL(https_process(const QString &)),
            this,
            SLOT(on_https_processed(const QString &)),Qt::QueuedConnection);
    https_manager_.start();

#if 1
    connect(&weight_manager_,
            SIGNAL(weight_trigger_proc(const int &, const int &, const int &, const int &)),
            this,
            SLOT(on_weight_trigger_proc(const int &, const int &, const int &, const int &)));

    connect(&weight_manager_,
            SIGNAL(usb_port_opened(const int&)),
            this,
            SLOT(on_usb_port_opened(const int&)));

    connect(&weight_manager_,
            SIGNAL(weight_sensor_error(const QString&, const int &)),
            this,
            SLOT(on_device_error(const QString&, const int &)));
    connect(&weight_manager_,
            SIGNAL(door_opened()),
            this,
            SLOT(on_door_opened()));
    connect(&weight_manager_,
            SIGNAL(door_closed()),
            this,
            SLOT(on_door_closed()));

    connect(&weight_manager_,
            SIGNAL(loadcell_trigger(WeightChangedInfo)),
            this,
            SLOT(on_loadcell_trigger(WeightChangedInfo)));
    connect(&weight_manager_,
            SIGNAL(loadcell_trigger_4vision(WeightChangedInfo)),
            this,
            SLOT(on_loadcell_trigger_4vision(WeightChangedInfo)));
    connect(&weight_manager_,
            SIGNAL(lock_all_opened()),
            this,
            SLOT(on_lock_all_opened()));
    connect(&weight_manager_,
            SIGNAL(deadbolt_error()),
            this,
            SLOT(on_deadbolt_error()));
    connect(&weight_manager_,
            SIGNAL(reset_deadbolt()),
            this,
            SLOT(on_reset_deadbolt()));
    weight_manager_.start();
#endif

#if 1
    connect(this,
            SIGNAL(play_audio(const int &)),
            &audio_manager_,
            SLOT(on_play_audio(const int &)));
#endif
    QString ioboard_verion =  _CONF->ioboard_version_;
    if(ioboard_verion == "carrier"){
        QTimer::singleShot(3000, this, [this]() { weight_manager_.ioboard_usb_reset(); });
    }else{
        on_usb_port_opened(TOTAL_USB_NUM);
    }
}

SmartVM::~SmartVM()
{
    https_manager_.quit();
}

void SmartVM::on_finalize(int type)
{
    if (type == SIGSEGV)
        return;

    qDebug("SmartVM::finalize() -> %s !!!", qPrintable(getSignalString(type)));

    switch (type) {
    case SIGTERM:
    case SIGINT:
        //        case SIGSEGV:   // too many signals, just skipping.
        // todo: to avoid deadlock
        on_quit();
        break;
    }
}

void SmartVM::on_updated(int type)
{
    qDebug("SmartVM::on_updated -> %s !!!", qPrintable(getSignalString(type)));

    switch (type) {
    case SIGHUP: // update
        break;
    case SIGUSR1:
        break;
    case SIGUSR2:
        break;
    }
}

void SmartVM::on_https_processed(const QString &https_req)
{
    qDebug()<<https_req;
    QJsonObject obj;
    QJsonDocument doc = QJsonDocument::fromJson(https_req.toUtf8());
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            obj = doc.object();
            qDebug()<<obj;
        }
        else
        {
            qDebug() << "Document is not an object" << endl;
        }
    }else{
        qDebug()<<"doc is null";
    }
    QString  event_type = obj.value("event_type").toString();
    QString user_id = obj.value("user_id").toString();
    qDebug()<<"event "<<event_type;
    qDebug()<<"use id: "<<user_id;
    QString dateStamp = QDateTime::currentDateTime().toString("yyyyMMdd");
    QString logDir = QString(LOG_PATH+dateStamp+"/log/sys");
    CSingleTon<QtLogWrapper>::getInstancePtr()->SetPath(logDir);
    CSingleTon<QtLogWrapper>::getInstancePtr()->Start();

    switch (resolveEventType(event_type)) {
    case QUERY_PRODUCT:{
        qDebug()<<"query product";
        QString path =  obj.value("path").toString();
        _CONF->mdlist_path = path;
        break;
    }
    case DOOR_OPENED:{
        qDebug()<<"door opened";
        qDebug()<<"user id: "<<user_id;
        current_user = user_id;
        //        purchase_manager_.add_trigger_count(current_user);
        //        int shelf_num = _CONF->shelfCounts();
        //        for(int shelf_id = 1; shelf_id <= shelf_num; shelf_id++){
        //            if(_CONF->project_name_ != "cheaper")
        //                weight_manager_.multi_capture_start(shelf_id, mt_index);
        //        }
        weight_manager_.ioboard_lock_on();
        weight_manager_.init_weight_trigger();
        loadcell_weights_.clear();
        changedInfos.clear();
        changedInfosVsion.clear();
        _CONF->user_id = current_user;
        _CONF->time = QTime::currentTime().toString("hhmmss_zzz");
        QString dateStamp = QDateTime::currentDateTime().toString("yyyyMMdd");
        QString img_folder =  QString("../../debug/%1/origin/%2/%3").arg(dateStamp).arg(_CONF->user_id).arg(_CONF->time);
        _CONF->img_folder = img_folder;
        weight_manager_.init_error_prop();
        error_infos.clear();

        emit gpio_open_door();
        break;
    }
    case DOOR_CLOSED:{
        qDebug()<<"door closed";
        qDebug()<<"user id: "<<user_id;
        break;
    }
    case RELEASE_INFO:{
        qDebug()<<"release results";
        qDebug()<<"user id: "<<user_id;
        current_user = "null";
        loadcell_weights_.clear();
        https_manager_.set_info_released(user_id, true);
        changedInfos.clear();
        changedInfosVsion.clear();
        isTimeout = false;
        isCapture = false;
        break;
    }
    case CHECK_STATUS:{
        qDebug()<<"check system status";
        https_manager_.set_system_status(user_id, error_infos);
        break;
    }
    case DEVICE_OPEN:{
        qDebug()<<"device open event";
        qDebug()<<"current_user: "<<current_user;

        //        current_user = user_id;
        //        _CONF->user_id = current_user;
        //        _CONF->time = QTime::currentTime().toString("hhmmss_zzz");
        //        QString dateStamp = QDateTime::currentDateTime().toString("yyyyMMdd");
        //        QString img_folder =  QString("../../debug/%1/origin/%2/%3").arg(dateStamp).arg(_CONF->user_id).arg(_CONF->time);
        //        _CONF->img_folder = img_folder;
        if(current_user == "null"){
            current_user = user_id;
            device_opened = true;
            weight_manager_.set_device_opened(true);
        }else{
            device_id = user_id;
        }
        //        weight_manager_.ioboard_door_on();
        emit gpio_open_door();
        break;
    }
    case SET_TIMER:{
        qDebug()<<"set timer event";
        int duration =  obj.value("duration").toInt();
        qDebug()<<"duration: "<<duration;
        weight_manager_.set_timer(duration);
        setTimer(duration);
        https_manager_.set_timer_response(current_user, duration);
        break;
    }
    case LOCK_STATE:{
        qDebug()<<"lock state event";
        int lock_state = weight_manager_.get_lock_state();
        https_manager_.set_state_response(user_id, lock_state);
        break;
    }
    }
}

void SmartVM::on_shopping_finished_proc(const QString &user_id)
{
    qDebug()<<"shopping finished, user id: "<<user_id;
}

void SmartVM::on_device_error(const QString &dev_name, const int &shelf_id)
{
    qDebug()<<"on_device_error: "<<dev_name<<" , shelf id: "<<shelf_id;
    error_infos[dev_name] = shelf_id;
    QString sensor_type = get_sensor_type(dev_name);
    https_manager_.send_sensor_error(current_user, sensor_type, dev_name);
}

void SmartVM::on_transmit_image(const QString &path, const int &cam_id)
{
    qDebug()<<"path: "<<path<<" , cam id: "<<cam_id;
}

void SmartVM::on_transmit_image(int mt_index, const QString &path, const int &cam_id)
{
    qDebug()<<"mt_index: "<< mt_index <<" , path: "<< path <<" , cam id: "<< cam_id;
    QString user = current_user;
    if(!isCapture) isCapture = true;
    if(door_opened == true && device_opened == false){
        https_manager_.send_top_img(cam_id, path, user);
    }
}

void SmartVM::on_event_off_proc()
{
}

EVENT_TYPE SmartVM::resolveEventType(QString event)
{
    if( event == "openDoor" ) return DOOR_OPENED;
    if( event == "closeDoor" ) return DOOR_CLOSED;
    if( event == "releaseInfo" ) return RELEASE_INFO;
    if( event == "checkStatus" ) return CHECK_STATUS;
    if( event == "queryProduct" ) return QUERY_PRODUCT;
    if(event == "deviceOpen") return DEVICE_OPEN;
    if(event == "setTimer") return  SET_TIMER;
    if(event == "lockState") return LOCK_STATE;
    return DOOR_OPENED;
}

void SmartVM::on_audio_timeout()
{
    if(door_opened){
        if(play_audio_times < 4){
            play_audio_times++;
            qDebug()<<"send signal to play audio";
            audio_timer_.start(60000);
            emit play_audio(CLOSE_DOOR_NOTI);
        }else{
            if(play_audio_times < 6){
                qDebug()<<"change audio interval to 5mins";
                play_audio_times++;
                audio_timer_.start(300000);
                emit play_audio(CLOSE_DOOR_NOTI);
            }else {
                emit play_audio(CLOSE_DOOR_NOTI);
                play_audio_times = 0;
                audio_timer_.stop();
                qDebug()<<"stop audio timer";
                https_manager_.set_close_door_error(current_user);
            }
        }
    }
}

void SmartVM::on_loadcell_trigger(WeightChangedInfo changedInfo)
{
    qDebug()<<"start time: : "<< changedInfo.start_time;
    qDebug()<<"end  time: : "<< changedInfo.end_time;
    qDebug()<<"  shelf_id: : "<< changedInfo.shelf_id;
    qDebug()<<"  delta_weight: : "<< changedInfo.delta_weight;
    changedInfos.push_back(changedInfo);
}

void SmartVM::on_loadcell_trigger_4vision(WeightChangedInfo changedInfo)
{
    qDebug()<<"4vision start_time: : "<< changedInfo.start_time <<" , " << QDateTime::fromMSecsSinceEpoch(changedInfo.start_time);
    qDebug()<<"4vision end_time: : "<< changedInfo.end_time <<" , " << QDateTime::fromMSecsSinceEpoch(changedInfo.end_time);
    qDebug()<<"4vision weightId: : "<< changedInfo.weight_id;
    qDebug()<<"4vision deltaWeight: : "<< changedInfo.delta_weight;
    changedInfosVsion.push_back(changedInfo);
}

void SmartVM::on_lock_all_opened()
{
    int mt_index_ = mt_index;
    bool device_opened_ = device_opened;
    QTimer::singleShot(CAMERA_CAPTURE_DELAY, this, [this, mt_index_, device_opened_]() {
        camera_manager_.on_off(mt_index_, 1, true, device_opened_);
        //            camera_manager_.cam_timer.start(50);
    });

    qDebug()<<"lock all opened";
    if(!device_opened){
        if(current_user.size() > 0 && door_opened == true){
            https_manager_.set_lock_all_opened(current_user,vendor);
        }
    }
}

void SmartVM::on_deadbolt_error()
{
    https_manager_.send_deadbolt_error(current_user);
}

QString SmartVM::get_sensor_type(QString dev_name)
{
    if(dev_name.startsWith("/dev/ttyUSB")){
        return "ioboard";
    }else if(dev_name.startsWith("/dev/v4l")){
        return "camera";
    }else{
        qDebug()<<"cannot find sensor type";
        return "unknown";
    }
}

void SmartVM::on_reset_deadbolt()
{
    qDebug()<<"reset deadbolt: "<<device_id;
    https_manager_.set_device_open_res(device_id, true);
}

void SmartVM::loop_open_door()
{
    QString jsonTemplate = QString("{\"event_type\":\"openDoor\",\"user_id\":\"%1\",\"inventory_list\":{},\"token\":\"%1\",\"validity\":\"240413101100\",\"type\":\"openDoor\"}");
    int tIndex_ = tIndex;
    QTimer::singleShot(3000, this, [this, tIndex_, jsonTemplate]() {
        QString msg = jsonTemplate.arg(tIndex_);
        on_https_processed(msg);
    });
    tIndex++;
}

void SmartVM::on_button_pressed()
{
    // 1 second close lock
    qDebug("button pressed");
    emit gpio_close_door();
}

void SmartVM::on_button_released()
{
    qDebug("button released");
    if(door_opened){
        on_lock_all_opened();
    }
}

void SmartVM::on_gpio_opened(const bool isOpened)
{
    qDebug()<<"gpio is : "<<isOpened;
    if(isOpened){
        on_door_opened();
        opened_door_timer_.start(timeout_);
        qDebug()<<"start timer";
    }else{
        opened_door_timer_.stop();
        on_door_closed();
    }
}

void SmartVM::on_opened_door_timeout()
{
    qDebug()<<"on_opened_door_timeout";
    bool isPressed=  button_manager_.getBtnStatus();
    bool isOpened = door_manager_.getDoorStatus();
    if(isPressed && isOpened){
        if(!device_opened){
            if(!isCapture) isTimeout = true;
            qDebug()<<"set timeout is true";
        }
        emit gpio_close_door();
    }
}

void SmartVM::setTimer(int duration)
{
    opened_door_timer_.stop();
    timeout_ = duration;
    opened_door_timer_.start(duration);
    qDebug()<<"set timer: "<<duration;
}

void SmartVM::on_init()
{
    connect(&camera_manager_,
            SIGNAL(camera_error(const QString&, const int &)),
            this,
            SLOT(on_device_error(const QString&, const int &)));
    connect(&camera_manager_,
            SIGNAL(transmit_image(const QString&, const int &)),
            this,
            SLOT(on_transmit_image(const QString&, const int &)));
    connect(&camera_manager_,
            SIGNAL(transmit_image(int, const QString&, const int &)),
            this,
            SLOT(on_transmit_image(int, const QString&, const int &)));
    camera_manager_.start();
    connect(&audio_timer_, SIGNAL(timeout()), this, SLOT(on_audio_timeout()));
    params_monitor_.start();
    qDebug("SmartVM::on_init()");

    connect(&door_manager_,
            SIGNAL(gpio_opened(const bool)),
            this,
            SLOT(on_gpio_opened(const bool)),Qt::DirectConnection);

    connect(&opened_door_timer_, SIGNAL(timeout()), this, SLOT(on_opened_door_timeout()));

    connect(this,
            SIGNAL(gpio_open_door()),
            &door_manager_,
            SLOT(on_gpio_open_door()),Qt::DirectConnection);
    connect(this,
            SIGNAL(gpio_close_door()),
            &door_manager_,
            SLOT(on_gpio_close_door()),Qt::DirectConnection);
    door_manager_.start();

    connect(&button_manager_,
            SIGNAL(button_pressed()),
            this,
            SLOT(on_button_pressed()),Qt::DirectConnection);
    connect(&button_manager_,
            SIGNAL(button_released()),
            this,
            SLOT(on_button_released()),Qt::DirectConnection);
    button_manager_.start();
}

void SmartVM::on_quit()
{
    qApp->quit();
    exit(1);
}

void SmartVM::on_delayed_cam_run(int id)
{
    if (id < _CONF->totalCamCounts())
        QTimer::singleShot(_CONF->camera_recognition_delay, this, [this, id]() { on_delayed_cam_run(id + 1); });
}

void SmartVM::on_door_opened()
{
    qDebug()<<"door opened smartvm";
    door_opened = true;

    if(!device_opened){
        if(current_user.size() > 0 && door_opened == true){
            //            https_manager_.set_door_opened(current_user, true);
            https_manager_.set_door_opened(current_user, true, vendor);
        }else{
            //            https_manager_.set_door_opened(current_user, false);
            https_manager_.set_door_opened(current_user, false, vendor);
        }
    }else{
        qDebug()<<"C/S deviceOpen event, door is opened";
        https_manager_.set_device_open_res(current_user, true);
    }
    //    audio_timer_.start(60000);
}

void SmartVM::on_door_closed()
{
    qDebug()<<"door closed"<<current_user;
    https_manager_.timer_t.restart();
    qDebug()<<"start httpManager timer";
    door_opened = false;
    int current_index = mt_index;
    bool device_opened_ = device_opened;
    camera_manager_.on_off(current_index, 1, false, device_opened_);
    camera_manager_.cam_timer.stop();

    if(!device_opened){
        //decision error
        //topview stop capture images
        if(!isTimeout)
            https_manager_.topview_done(current_user, 0);

        QJsonObject json_obj;
        json_obj.insert("user_id", current_user);
        json_obj.insert("event_type", DOOR_CLOSED_);
        QJsonDocument doc(json_obj);
        QString strJson(doc.toJson(QJsonDocument::Compact));
        //        on_https_processed(strJson);

        //play audio
        audio_timer_.stop();
        play_audio_times = 0;
        emit play_audio(REMOVE_CARD_NOTI);
        QTimer::singleShot(50, this, [this, current_index,strJson]() {
            qDebug()<<"2000";

            if(_CONF->project_name_ == "cheaper"){
                QString img_folder = _CONF->img_folder;
                if(error_infos.size() > 0){
                    https_manager_.set_ioboard_error(this->current_user, img_folder, this->error_infos);
                }else{
                    https_manager_.set_weight_info(this->current_user, loadcell_weights_, img_folder,this-> changedInfos, this->changedInfosVsion, isTimeout);
                }
            }else{
                int shelf_num = _CONF->shelfCounts();
                for(int shelf_id = 1; shelf_id <= shelf_num; shelf_id++){
                    qDebug("shelf id : %d", shelf_id);
                    weight_manager_.multi_capture_end(shelf_id, current_index);
                }
            }
            on_https_processed(strJson);
        });
        qDebug()<<"\n\n start";
    }else{
        qDebug()<<"C/S deviceOpen event, door is closed";
        current_user = "null";
        device_opened = false;
        weight_manager_.set_device_opened(false);
        loadcell_weights_.clear();
        changedInfos.clear();
        changedInfosVsion.clear();
    }
}

void SmartVM::on_usb_port_opened(const int& port_id)
{
    if(port_id == TOTAL_USB_NUM){
        //    if(port_id == 2){
        qDebug()<<"usb ports are all opened";
        QTimer::singleShot(1000, this, [this]() { on_init(); });
    }
}

void SmartVM::on_aivison_processed(const vision_out_t &vout)
{
    for (auto v : vout.detected) {
        v.print();
    }
}

void SmartVM::on_weight_trigger_proc(const int &mt_index, const int &shelf_no, const int &weight_index, const int &delta_weight)
{
    qDebug("mt_index: %d , weight_trigger: shelf %d and weight index %d weight %d", mt_index, shelf_no, weight_index, delta_weight );
    loadcell_nums_++;

    if(abs(delta_weight ) > 0 ){
        int weight_id = 2*(shelf_no - 1) + weight_index;
        loadcell_weights_[weight_id] = delta_weight;
    }

    int shelf_num = _CONF->shelfCounts();
    if(loadcell_nums_ == (shelf_num*2)){
        loadcell_nums_ = 0;
        if(device_opened == true){
            device_opened = false;
            weight_manager_.set_device_opened(false);
            qDebug()<<"device open the door, weight sensors get stable";
        }else{
            qDebug()<<"door closed and weight sensors get stable";
            /* send weight info and image folder path*/
            QString img_folder = _CONF->img_folder;
            if(error_infos.size() > 0){
                https_manager_.set_ioboard_error(current_user, img_folder, error_infos);
            }else{
                https_manager_.set_weight_info(current_user, loadcell_weights_, img_folder, changedInfos, changedInfosVsion);
            }
        }
    }
}
