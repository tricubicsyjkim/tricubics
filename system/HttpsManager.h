#ifndef HTTPSMANAGER_H
#define HTTPSMANAGER_H
#include <QThread>
#include <QByteArray>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QString>
#include <mqueue.h>
#include <zmq.h>
#include <QTime>
#include <QFileInfo>
#include <QElapsedTimer>
#include "Instance.h"
#include "AIVsionDefines.h"

enum EVENT_TYPE
{
    DOOR_OPENED=0,
    DOOR_CLOSED,
    RELEASE_INFO,
    CHECK_STATUS,
    QUERY_PRODUCT,
    DEVICE_OPEN,
    SET_TIMER,
    LOCK_STATE
};

class HttpsManager: public QThread
{
    Q_OBJECT
public:
    explicit HttpsManager();
    ~HttpsManager() override;
    bool proc();
    void set_system_status(QString user_id, std::map<QString, int>error_infos);
    void set_door_opened(QString user_id, bool is_opened);
    void set_door_opened(QString user_id, bool is_opened, QString vendor);
    void set_info_released(QString user_id, bool is_released);
    void set_weight_info(QString user_id, std::map<int,int> weight_info, QString img_folder, std::vector<WeightChangedInfo> changedInfos, std::vector<WeightChangedInfo> infosVision);
    void set_weight_info(QString user_id, std::map<int,int> weight_info, QString img_folder, std::vector<WeightChangedInfo> changedInfos, std::vector<WeightChangedInfo> infosVision, bool isTimeout);
    void set_ioboard_error(QString user_id, QString img_folder, std::map<QString, int> error_infos);
    void set_device_open_res(QString deviceID, bool is_opened);
    void send_top_img(int cam_id, QString path, QString user_id);
    void topview_done(QString user_id, int cam_id);
    void set_lock_all_opened(QString user_id, QString vendor);
    void set_timer_response(QString user_id, int duration);
    void set_state_response(QString user_id, int state);
    void set_close_door_error(QString user_id);
    QString get_abs_path(QString file_path);
    void send_deadbolt_error(QString user_id);
    void send_sensor_error(QString user_id, QString sensor_type, QString dev_path);

signals:
    void https_process(const QString &);
public slots:

private:
    void run() override;

public:
    void *context;
    void *sender;
    void *receiver;
    char req_buffer[4096];
    char res_buffer[51200];
    QElapsedTimer timer_t;
};

#endif // HTTPSMANAGER_H
