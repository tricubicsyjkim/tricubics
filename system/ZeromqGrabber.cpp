#include "ZeromqGrabber.h"


ZeromqGrabber::ZeromqGrabber(const QString pole, int port, int level_id, int group_id)
    :QThread(nullptr), pole_(pole), port_(port), level_id_(level_id), group_id_(group_id)
{
    context = zmq_ctx_new ();
    if(pole == CONFIG_SENDER){
        sender = zmq_socket (context, ZMQ_PUSH);
        QString sender_addr = get_ip_addr(port_);
        if(sender_addr != NULL){
            int send_hwm = 50000;
            zmq_setsockopt (sender, ZMQ_SNDHWM, &send_hwm, sizeof (send_hwm));
            zmq_bind(sender, sender_addr.toStdString().data());
        }
#ifdef test
        if(level_id == 1){
            zmq_bind(sender, "tcp://0.0.0.0:5550");
        }else{
            zmq_bind(sender, "tcp://0.0.0.0:5598");
        }
        qDebug()<<"bind sender";
#endif

    }else if(pole == CONFIG_RECEIVER){
        receiver = zmq_socket (context, ZMQ_PULL);
        QString receiver_addr = get_ip_addr(port_);
        if(receiver_addr != NULL){
            int receive_hwm = 50000;
            zmq_setsockopt (receiver, ZMQ_SNDHWM, &receive_hwm, sizeof (receive_hwm));
            zmq_bind(receiver, receiver_addr.toStdString().data());
        }
#ifdef test
        if(level_id == 1){
            zmq_bind(receiver, "tcp://0.0.0.0:5551");
        }else{
            zmq_bind(receiver, "tcp://0.0.0.0:5599");
        }
        qDebug()<<"bin receiver: "<<group_id_;
#endif
        quit_ = true;
    }
}

ZeromqGrabber::~ZeromqGrabber()
{
    if(sender != nullptr){
        zmq_close(sender);
    }
    if(receiver != nullptr){
        zmq_close (receiver);
    }
    zmq_ctx_destroy (context);
}

QSharedPointer<ZeromqGrabber> ZeromqGrabber::create(const QString pole, int port, int level_id, int group_id)
{
    qDebug()<<"create ZeromqGrabber";
    QSharedPointer<ZeromqGrabber> zeromq_grabber;
    zeromq_grabber = QSharedPointer<ZeromqGrabber>::create(pole, port, level_id, group_id);
    return zeromq_grabber;
}

QString ZeromqGrabber::get_ip_addr(int port)
{
    QString addr = "tcp://0.0.0.0:" + QString::number(port);
    qDebug()<<"addr: "<<addr;
    return addr;
}

bool ZeromqGrabber::proc()
{
    qDebug()<<"receiver proc";
    qDebug()<<"level id: "<<level_id_<<" ,group: "<<group_id_<<" , port: "<<port_<<"pole: "<<pole_;
    while(quit_){
        memset( res_buffer, 0, 1024);
        zmq_recv (receiver, res_buffer, sizeof(res_buffer), 0);
        QString res_msg= QString(reinterpret_cast<const char*>(res_buffer));
        parse_res(res_msg);
    }
    return true;
}

void ZeromqGrabber::parse_res(QString &res_msg)
{
    if(level_id_ == 1){
#ifdef SNACK
        emit level2_bbox(res_msg);
#else
        emit level1_bbox(res_msg);
#endif
    }else if(level_id_ == 2){
        emit level2_bbox(res_msg);
    }
}

void ZeromqGrabber::on_req_level1(const QString &req_msg, const int &cam_id)
{
    qDebug()<<"req msg: "<<req_msg;
    QJsonObject json_obj;
    json_obj.insert("cam_id", cam_id);
    json_obj.insert("img_path", req_msg);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( req_buffer, 0, 1024);
    memcpy( req_buffer, strJson.toStdString().c_str(), strJson.size());
    zmq_send (sender, req_buffer, 1024, 0);
}

void ZeromqGrabber::on_req_level1(int mt_index, const QString &req_msg, const int &cam_id)
{
    qDebug()<<"req msg: "<<req_msg;
    QJsonObject json_obj;
    json_obj.insert("cam_id", cam_id);
    json_obj.insert("img_path", req_msg);
    //mt index
    json_obj.insert("mt_index", mt_index);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( req_buffer, 0, 1024);
    memcpy( req_buffer, strJson.toStdString().c_str(), strJson.size());
    zmq_send (sender, req_buffer, 1024, 0);
}

void ZeromqGrabber::on_req_level2(const QString &bbox_data)
{
    qDebug()<<"on_req_level2, group id: "<<group_id_;
    memset( req_buffer, 0, 1024);
    memcpy( req_buffer, bbox_data.toStdString().c_str(), bbox_data.size());
    zmq_send (sender, req_buffer, 1024, 0);
    qDebug()<<"msg send to level2: "<<bbox_data;
}

void ZeromqGrabber::run()
{
    qDebug()<<"ZeromqGrabber id :" <<this->currentThreadId() << "is running";
    if(pole_ == CONFIG_RECEIVER){
        this->proc();
    }
}
