#include "Purchase.h"
#include "SmartVM.h"
#include <QDebug>
#include <QElapsedTimer>
#include "AIVsionDefines.h"
#include<algorithm>
#include "Instance.h"
#include <functional>
#include <numeric>  //std::accumulate
#include<stdlib.h>
#include <cmath>
using namespace std;

PurchaseManager::PurchaseManager()
{
    if(_CONF->mdlist_path != ""){
        qDebug()<<"init objs info";
        //init_objs_info(_CONF->mdlist_path);
    }else{
        qDebug()<<"didnot init production list";
    }
}

PurchaseManager::~PurchaseManager() {
}



void PurchaseManager::release_proc(int mt_index)
{
    running_shelves[mt_index].clear();
    //add every shelf weight value
    shelf_obj_num.clear();
    start_decision_[mt_index] = false;
    set_delta_weight[mt_index] = false;
    mt_obj_num[mt_index] = 0;
    img_counter[mt_index] = 0;
    //only loadcell
    weight_result_vec.clear();
    multi_delta_weight_[mt_index].clear();
}


void PurchaseManager::add_trigger_count(QString user_id)
{
    int count = trigger_counter.count(user_id);
    if(count == 0){
        trigger_counter[user_id] = 1;
    }else{
        trigger_counter[user_id] += 1;
    }
    qDebug()<<"userid: "<<user_id<<" , trigger_counter: "<<trigger_counter[user_id];
}

std::map<QString, int> PurchaseManager::get_total_results(QString user_id)
{
    return total_results[user_id];
}

void PurchaseManager::release_userInfo(QString user_id)
{
    total_results[user_id].clear();
    user_ids.erase(user_ids.begin());
    shopping_finished.erase(user_id);
    invetory_results[user_id].clear();
    qDebug()<<"total results size: "<<total_results[user_id].size();
    //decision error
    is_decision_error = false;
    error_loadcell_ids.clear();
}

void PurchaseManager::insert_running_shelves(int shelf_id, int mt_index)
{
    int num = std::count(running_shelves[mt_index].begin(), running_shelves[mt_index].end(), shelf_id);
    if(num == 0){
        running_shelves[mt_index].push_back(shelf_id);
    }
}

void PurchaseManager::update_obj_num(const int &mt_index, const int &shelf_id, const int &weight_index, const int &delta_weight)
{
    multi_delta_weight_[mt_index][shelf_id][weight_index] = delta_weight;
    qDebug()<<"update_obj_num, delta_weight: "<<delta_weight;

    int sensor_size = 0;
    sensor_size = _CONF->weight_ids.size();
    qDebug()<<"sensor_size: "<<sensor_size;
    int current_sensor_size = 0;
    for(auto multi_weight_pair : multi_delta_weight_[mt_index]){
        current_sensor_size += multi_weight_pair.second.size();
    }
    qDebug()<<"current_sensor_size: "<<current_sensor_size;
    if(sensor_size == current_sensor_size){
        set_delta_weight[mt_index] = true;
    }

    qDebug()<<"delta_weight: "<<delta_weight;

    if(abs(delta_weight) > 10){
        int obj_num = 0;
        int totoal_weight = 0;
        qDebug()<<"shelf_id: "<<shelf_id<<" , weight_index"<<weight_index;
        bool loadcell_error = true;
        WeightDetect info;
        float deviation = 1;
        info.obj_id = "";
        for (int i = 0; i < carrier_objs.size(); ++i){
            int weight_value = carrier_objs[i].weight;
            totoal_weight += weight_value;
            qDebug()<<"in while: "<<weight_value;
            float obj_num_f =    static_cast<float>(abs(delta_weight))/static_cast<float>(weight_value);
            obj_num = std::round(obj_num_f);
            qDebug()<<"obj_num: "<<obj_num;
            obj_num_f -= obj_num;
            qDebug()<<"obj_num_f: "<<obj_num_f;
            if(abs(obj_num_f) < 0.15 && obj_num == 1){
                int shelf_id =carrier_objs[i].shelf_id;
                qDebug()<<"shelf_id: "<<shelf_id;
                QString obj_name = carrier_objs[i].obj_name;
                qDebug()<<"obj_name: "<<obj_name;
                QString obj_id =  carrier_objs[i].obj_id;
                qDebug()<<"obj_id: "<<obj_id;
                if(abs(obj_num_f) < deviation){
                    info.clsName = obj_name;
                    info.obj_id = obj_id;
                    info.shelf_id = shelf_id;
                    info.weight = delta_weight;
                    info.objNum = obj_num;
                    loadcell_error = false;
                    deviation = abs(obj_num_f);
                }
            }
        }
        if(info.obj_id != -1){
            weight_result_vec.push_back(info);
        }
        if(loadcell_error){
            // something wrong
            qDebug()<<"something wrong";
            is_decision_error = true;
            QString shelf_str = "shelf_"+ QString::number(shelf_id);
            int weight_id = _CONF->shelf_configs_[shelf_str].weight_ids[weight_index];
            error_loadcell_ids.push_back(weight_id);
            qDebug()<<"error weight id: "<<weight_id;
        }
    }

    if((set_delta_weight[mt_index] == true)){
        start_weight_decision(mt_index);
    }else{
        qDebug()<<"not ready, weight finished";
    }
}

void PurchaseManager::start_weight_decision(int mt_index)
{
    QString weight_result;
    WeightDetect info;
    if(weight_result_vec.size() == 0){
        qDebug()<<"nothing changed";
    }else{
        for(auto weight_detect : weight_result_vec){
            info = weight_detect;
            QString cls_name = info.clsName;
            QString obj_id = info.obj_id;
            //add gui message data
            QString status;
            int temp_weight = info.weight;
            qDebug()<<"temp_weight:"<<temp_weight;
            weight_result.append(cls_name + " ");
            if(temp_weight > 0){
                weight_result.append("in");
                status.append("in");
            }else{
                weight_result.append("out");
                status.append("out");
            }
            qDebug()<<"loadcell decision: "<<weight_result;
            //https server request
            QString user_id = user_ids.front();
            int count = total_results[user_id].count(obj_id);

            if(count == 0){
                total_results[user_id][obj_id] = 0;
            }
            if(temp_weight < 0){
                total_results[user_id][obj_id] += info.objNum;
            }else{
                total_results[user_id][obj_id] -= info.objNum;
            }
        }
    }
    qDebug()<<"release_proc";
    release_proc(mt_index);

    QString user_id = user_ids.front();
    qDebug()<<"user id: "<<user_id;
    trigger_counter[user_id] -= 1;
    qDebug()<<"user id: "<<user_id<< " , trigger count: "<<trigger_counter[user_id];

    if(shopping_finished[user_id] == true){
        if(trigger_counter[user_id] == 0){
            qDebug()<<"emit shopping finished signal";
            emit shopping_finished_proc(user_id);
        }
    }
    emit event_off_proc();
    qDebug()<<"event off";
}

void PurchaseManager::init_objs_info(QString path)
{
    QString val;
    QFile file;
    file.setFileName(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject rootObj;
    if(!doc.isNull())
    {
        if(doc.isObject())
        {
            rootObj = doc.object();
            qDebug()<<"root obj";
        }
        else
        {
            qDebug() << "Document is not an object" << endl;
        }
    }else{
        qDebug()<<"doc is null";
    }

    QJsonArray objsArray = rootObj.value("objs").toArray();
    qDebug() << "There are " << objsArray.size() << "sets of objs in the json file";
    foreach(const QJsonValue & val, objsArray){
        ObjInfo obj;
        QString obj_name = val.toObject().value("productName").toString();
        QString obj_id = val.toObject().value("productID").toString();
        int weight = (int)(val.toObject().value("weight").toDouble());
        obj.obj_name = obj_name;
        obj.obj_id = obj_id;
        obj.weight = weight;
        carrier_objs.append(obj);
    }

    qDebug() << "Finished parsing, heres the data";
    for(auto pt: carrier_objs) {
        qDebug() << pt.obj_name <<"  , "<< pt.weight;
    }
}
