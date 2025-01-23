#ifndef AIVISIONDEFINES_H
#define AIVISIONDEFINES_H

#include <QDebug>
#include <QMetaType>
#include <map>
#include <string>
#include <vector>
#include <QDateTime>


#pragma pack(push, 1)

struct DetectInfo {
    int x1;
    int y1;
    int x2;
    int y2;
    int w;
    int h;
    int x_c;
    int y_c;
    int width;
    int height;

    double confidence;
    int score;
    int clsId;
    QString clsName;
    int cam_id;
    int level2_id;
    DetectInfo() : x1(0.f), y1(0.f), x2(0.f), y2(0.f), w(0.f), h(0.f), x_c(0.f), y_c(0.f),width(0.f), height(0.f),
        confidence(0.f),score(0.f), clsId(-1), clsName(""), cam_id(0.f), level2_id(0.f)
    {}
    void print() {
        //qDebug("DetectInfo [%d:%s] L1(%d x %d) L2(%d x %d) Center ( %d x %d ) w:%d h:%d prob: %.2lf",
        //clsId, qPrintable(clsName),
        //x1,
        //y1,
        //x2,
        //y2,
        //x_c,
        //y_c,
        //w,
        //h,
        //confidence);
    }
};

typedef struct {
    int shelf_id;
    int cam_id;
    QString spath;
    QString tpath;
    float thresh;
    bool debug;
    /* if is_purchase if false, we can start to purchase decision, if it is true,
 * just to add data */
    void print() {
        qDebug("vision_inp_t: shelf_id[%d], cam_id[%d] spath(%s) tpath(%s) thresh(%.2f) debug:%d",
               shelf_id, cam_id, qPrintable(spath), qPrintable(tpath), thresh, debug);
    }
} vision_inp_t;

typedef struct {
    int shelf_id;
    int cam_id;
    int level2_group_id;
    //mt index
    int mt_index;
    std::vector<DetectInfo> detected;
    bool start_decision;
} vision_out_t;

typedef struct {
    char mt_status;
    char  cam_id;
    char path[256];
} vision_input_t;

//only loadcell
struct WeightDetect {
    int clsId;
    QString clsName;
    QString obj_id;
    int shelf_id;
    int weight;
    int objNum;
    WeightDetect() : clsId(-1), clsName(""), obj_id(""), shelf_id(0.f), weight(0.f), objNum(0)
    {}

};

struct  ObjInfo{
    QString code;
    QString obj_name;
    QString obj_id;
    int shelf_id;
    int weight;
    ObjInfo():code(""), obj_name(""), obj_id(""), shelf_id(1), weight(0)
    {}
} ;

struct WeightChangedInfo{
    qint64 start_time;
    qint64 end_time;
    int shelf_id;
    int weight_id;
    int delta_weight;
    int first_changed_value;
    WeightChangedInfo():start_time(0), end_time(0),shelf_id(1),delta_weight(0),first_changed_value(0)
    {}
};

#pragma pack(pop)

Q_DECLARE_METATYPE(vision_inp_t);
Q_DECLARE_METATYPE(vision_out_t);
Q_DECLARE_METATYPE(vision_input_t);
Q_DECLARE_METATYPE(WeightChangedInfo);

#endif // AIVISIONDEFINES_H
