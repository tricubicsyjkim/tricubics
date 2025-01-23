#ifndef PURCHASE_H
#define PURCHASE_H

#include <QElapsedTimer>
#include <QObject>
#include <cstdio>
#include <map>
#include <vector>
#include <QString>
#include <mqueue.h>
#include "AIVsionDefines.h"
#include "WeightManager.h"
#include <QVector>
#include <algorithm>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QDebug>
#include <QFile>
#include <QJsonObject>

#define ONLY_LOADCELL
#define SHELF_NUM 5

class PurchaseManager : public QObject {
    Q_OBJECT
public:
    explicit PurchaseManager();
    ~PurchaseManager();

    void release_proc(int mt_index);

    // https server
    void add_trigger_count(QString user_id);
    std::map<QString, int> get_total_results(QString user_id);
    void release_userInfo(QString user_id);

    //add camera_id to run cameras
    void insert_running_shelves(int shelf_id, int mt_index);

    //add weight value into decision
    void update_obj_num(const int &mt_index, const int &shelf_id, const int &weight_index, const int &delta_weight);
    //only loadcell
    void start_weight_decision(int mt_index);

    //read carriers objs info from json file
    void init_objs_info(QString path);
signals:
    void shopping_finished_proc(const QString &);
    void event_off_proc();
public slots:

private:
    std::vector<WeightDetect> weight_result_vec;

public:
    bool is_first = true;

    //https server
    std::vector<QString> user_ids;
    std::map<QString, int> trigger_counter;
    std::map<QString, bool> shopping_finished;  //user id, bool
    std::map<QString, std::map<QString, int>> total_results;
    //add camera_id to run cameras
    std::map<int, std::vector<int>> running_shelves;
    //add weight value into decision, std::map<int(shelf_id), int(obj num)>
    std::map<int, int> shelf_obj_num;
    //only loadcell
    std::map<int, std::map<int, std::map<int, int>>> loadcell_obj_num;  //mt_index, shelf_id, weight_index, obj_num
    std::map<int, int> mt_obj_num;  //mt_index, obj_num
    //add every shelf weight value
    std::map<int, std::map<int, std::map<int, int>>> multi_delta_weight_; //mt_index, shelf_id, weight_index, delta weight
    std::map<int, bool> start_decision_;    //mt_index
    std::map<int, bool> set_delta_weight;   //mt_index, bool
    std::map<int, int> img_counter;    // mt_index, image count
    //inventory list
    std::map<QString, std::map<QString, int>> invetory_results; //user id, obj_name, obj_num
    int update_times = 0;
    // decision error
    bool is_decision_error = false;
    std::vector<int> error_loadcell_ids;
    QVector<ObjInfo> carrier_objs;
};

#endif

