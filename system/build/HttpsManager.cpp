#include "HttpsManager.h"

HttpsManager::HttpsManager()
{
    context = zmq_ctx_new ();
    sender = zmq_socket (context, ZMQ_PUSH);
    QString sender_addr = "tcp://0.0.0.0:3001";
    if(sender_addr != NULL){
        zmq_connect(sender, sender_addr.toStdString().data());
    }

    receiver = zmq_socket (context, ZMQ_PULL);
    QString receiver_addr = "tcp://0.0.0.0:3000";
    if(receiver_addr != NULL){
        zmq_connect(receiver, receiver_addr.toStdString().data());
    }

    qDebug("end init");
}

HttpsManager::~HttpsManager()
{
    if(sender != nullptr){
        zmq_close(sender);
    }
    if(receiver != nullptr){
        zmq_close (receiver);
    }
    zmq_ctx_destroy (context);
}

bool HttpsManager::proc()
{
    qDebug()<<"https manager started";
    while ( 1 ) {
        usleep(1);
        memset( req_buffer, 0, 51200);
        qDebug()<<sizeof(req_buffer);
        zmq_recv (receiver, req_buffer, 51200, 0);
        qDebug()<<"rec data from server";
        QString testMsg(req_buffer);
        qDebug()<<"test msg: "<<testMsg;
        QString req_msg= QString(reinterpret_cast<const char*>(req_buffer));
        qDebug()<<"req_msg: "<<req_msg;
        qDebug()<<req_msg.size();
        emit https_process(req_msg);
        memset(req_buffer, 0, sizeof(req_buffer));
    }
    return true;
}

void HttpsManager::set_system_status(QString user_id, std::map<QString, int> error_infos)
{
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "CHECK_STATUS");
    json_obj.insert("check_status", true);
    QJsonArray error_array;
    for(auto error_info_pair : error_infos){
        QJsonObject error_info;
        error_info.insert("deviceType", error_info_pair.first);
        error_info.insert("shelfId", error_info_pair.second);
        error_array.append(error_info);
    }
    qDebug()<<"size: "<<error_array.size();
    if(error_array.size() > 0){
        json_obj.insert("errorList", error_array);
    }
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, 0, 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::set_door_opened(QString user_id, bool is_opened)
{
    qDebug()<<"user id: "<<user_id;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "DOOR_OPENED");
    json_obj.insert("is_opened", is_opened);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::set_door_opened(QString user_id, bool is_opened, QString vendor)
{
    qDebug()<<"user id: "<<user_id;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "DOOR_OPENED");
    json_obj.insert("is_opened", is_opened);
    json_obj.insert("vendor", vendor);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::set_info_released(QString user_id, bool is_released)
{
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "RELEASE_INFO");
    json_obj.insert("is_released", is_released);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, 0, 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::set_weight_info(QString user_id, std::map<int, int> weight_info, QString img_folder, std::vector<WeightChangedInfo> changedInfos, std::vector<WeightChangedInfo> infosVision, bool isTimeout)
{
    qDebug()<<"set set_weight_info results";
    qDebug()<<"weight_info: "<< weight_info;
    qDebug()<<"img_folder: "<< img_folder;
    //send mq to server
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "DOOR_CLOSED");
    json_obj.insert("is_closed", true);
    json_obj.insert("folderPath", get_abs_path(img_folder));
    json_obj.insert("isTimeout", isTimeout);
    QJsonArray weight_array;
    for(auto result_pair : weight_info){
        QJsonObject weight;
        weight.insert("WeightValue", result_pair.second);
        weight.insert("WeightID", result_pair.first);
        if(result_pair.second != 0){
            weight_array.append(weight);
        }
    }

    qDebug()<<"size: "<<weight_array.size();
    if(weight_array.size() > 0){
        json_obj.insert("weightInfo", weight_array);
    }

    QJsonArray changed_info_array;
    for(auto result_pair : changedInfos){
        QJsonObject info;
        info.insert("start_time", result_pair.start_time);
        info.insert("end_time", result_pair.end_time);
        info.insert("delta_weight", result_pair.delta_weight);
        info.insert("shelf_id", result_pair.shelf_id);
        changed_info_array.append(info);
    }

    qDebug()<<"size: "<<changed_info_array.size();
    if(changed_info_array.size() > 0){
        json_obj.insert("changedInfo", changed_info_array);
    }

    QJsonArray info_vision_array;
    for(auto result_pair : infosVision){
        QJsonObject info;
        info.insert("startChange", result_pair.start_time);
        info.insert("stopChange", result_pair.end_time);
        info.insert("deltaWeight", result_pair.delta_weight);
        info.insert("WeightID", result_pair.weight_id);
        info_vision_array.append(info);
    }

    qDebug()<<"size: "<<changed_info_array.size();
    if(info_vision_array.size() > 0){
        json_obj.insert("changedWeightInfo", info_vision_array);
    }

    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.toStdString().size();
    memset( res_buffer, 0, 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    qDebug("send to server: %s \n",res_buffer );
    zmq_send (sender, res_buffer, 51200, 0);
    qDebug()<<"send weight info to L/S: "<<timer_t.elapsed();
}

void HttpsManager::set_weight_info(QString user_id, std::map<int, int> weight_info, QString img_folder, std::vector<WeightChangedInfo> changedInfos, std::vector<WeightChangedInfo> infosVision)
{
    qDebug()<<"set set_weight_info results";
    qDebug()<<"weight_info: "<< weight_info;
    qDebug()<<"img_folder: "<< img_folder;
    //send mq to server
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "DOOR_CLOSED");
    json_obj.insert("is_closed", true);
    json_obj.insert("folderPath", get_abs_path(img_folder));
    QJsonArray weight_array;
    for(auto result_pair : weight_info){
        QJsonObject weight;
        weight.insert("WeightValue", result_pair.second);
        weight.insert("WeightID", result_pair.first);
        if(result_pair.second != 0){
            weight_array.append(weight);
        }
    }

    qDebug()<<"size: "<<weight_array.size();
    if(weight_array.size() > 0){
        json_obj.insert("weightInfo", weight_array);
    }

    QJsonArray changed_info_array;
    for(auto result_pair : changedInfos){
        QJsonObject info;
        info.insert("start_time", result_pair.start_time);
        info.insert("end_time", result_pair.end_time);
        info.insert("delta_weight", result_pair.delta_weight);
        info.insert("shelf_id", result_pair.shelf_id);
        changed_info_array.append(info);
    }

    qDebug()<<"size: "<<changed_info_array.size();
    if(changed_info_array.size() > 0){
        json_obj.insert("changedInfo", changed_info_array);
    }

    QJsonArray info_vision_array;
    for(auto result_pair : infosVision){
        QJsonObject info;
        info.insert("startChange", result_pair.start_time);
        info.insert("stopChange", result_pair.end_time);
        info.insert("deltaWeight", result_pair.delta_weight);
        info.insert("WeightID", result_pair.weight_id);
        info_vision_array.append(info);
    }

    qDebug()<<"size: "<<changed_info_array.size();
    if(info_vision_array.size() > 0){
        json_obj.insert("changedWeightInfo", info_vision_array);
    }

    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.toStdString().size();
    memset( res_buffer, 0, 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    qDebug("send to server: %s \n",res_buffer );
    zmq_send (sender, res_buffer, 51200, 0);
    qDebug()<<"send weight info to L/S: "<<timer_t.elapsed();
}

void HttpsManager::set_ioboard_error(QString user_id, QString img_folder, std::map<QString, int> error_infos)
{
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "DOOR_CLOSED");
    json_obj.insert("is_closed", true);
    json_obj.insert("folderPath", get_abs_path(img_folder));
    QJsonArray error_array;
    for(auto error_info_pair : error_infos){
        QJsonObject error_info;
        error_info.insert("deviceType", error_info_pair.first);
        error_info.insert("shelfId", error_info_pair.second);
        error_array.append(error_info);
    }
    qDebug()<<"size: "<<error_array.size();
    if(error_array.size() > 0){
        json_obj.insert("errorList", error_array);
    }

    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.toStdString().size();
    memset( res_buffer, 0, 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    qDebug("send to server: %s \n",res_buffer );
    zmq_send (sender, res_buffer, 51200, 0);
    qDebug()<<"send io board error to L/S: "<<timer_t.elapsed();
}

void HttpsManager::set_device_open_res(QString deviceID, bool is_opened)
{
    qDebug()<<"device id: "<<deviceID;
    QJsonObject json_obj;
    json_obj.insert("deviceID", deviceID);
    json_obj.insert("event_type", "deviceOpen");
    json_obj.insert("is_opened", is_opened);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::send_top_img(int cam_id, QString path, QString user_id)
{
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "sendImage");
    json_obj.insert("path", path);
    json_obj.insert("cam_id", cam_id);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    //qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::topview_done(QString user_id, int cam_id)
{
    qDebug()<<"topviewDone: "<<cam_id;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "topviewDone");
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::set_lock_all_opened(QString user_id, QString vendor)
{
    qDebug()<<"set_lock_all_opened: "<<user_id <<" , "<<vendor;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "lockAllOpened");
    json_obj.insert("vendor", vendor);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::set_timer_response(QString user_id, int duration)
{
    qDebug()<<"set_timer_response: "<<user_id;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "setTimer");
    json_obj.insert("duration", duration);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::set_state_response(QString user_id, int state)
{
    qDebug()<<"set_state_response: "<<user_id;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "lockState");
    json_obj.insert("status", state);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::set_close_door_error(QString user_id)
{
    qDebug()<<"set_close_door_error: "<<user_id;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "closeDoorError");
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

QString HttpsManager::get_abs_path(QString file_path)
{
    QFileInfo fileinfo;
    fileinfo = QFileInfo(file_path);
    return fileinfo.absoluteFilePath();
}

void HttpsManager::send_deadbolt_error(QString user_id)
{
    qDebug()<<"send_deadbolt_error: "<<user_id;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "deadboltError");
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::send_sensor_error(QString user_id, QString sensor_type, QString dev_path)
{
    qDebug()<<"send_sensor_error: "<<user_id;
    QJsonObject json_obj;
    json_obj.insert("user_id", user_id);
    json_obj.insert("event_type", "sensorError");
    json_obj.insert("sensor_type", sensor_type);
    json_obj.insert("dev_path", dev_path);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( res_buffer, '\0', 51200);
    memcpy( res_buffer, strJson.toStdString().c_str(), strJson.toStdString().size());
    //zmq_send (sender, res_buffer, 51200, 0);
}

void HttpsManager::run()
{
    qDebug()<<"httpsmanager run";
    proc();
}
