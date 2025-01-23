#include "PyAIManager.h"
#include <QMetaType>
#include <QString>
#include "Instance.h"

PyAIManager::PyAIManager()
{
    init_label();
    ai_attr.mq_flags = 0;
    ai_attr.mq_maxmsg = 10;
    ai_attr.mq_msgsize = 1024;
    ai_attr.mq_curmsgs = 0;

    //mq_unlink("/ai_result");
    ai_mqd = mq_open("/ai_result", O_CREAT|O_RDONLY|O_NONBLOCK, S_IWUSR|S_IRUSR, &ai_attr);
    if ( ai_mqd == (mqd_t) -1 ){
        perror("ai_result");
        printf("[ERROR] ai_mqd open failed!\n");
    }else{
        qDebug("ai result queue opended");
    }

    sys_attr.mq_flags = 0;
    sys_attr.mq_maxmsg = 10;
    sys_attr.mq_msgsize = 1024;
    sys_attr.mq_curmsgs = 0;

    //mq_unlink("/sys_status");
    sys_mqd = mq_open("/sys_status",  O_CREAT|O_WRONLY, S_IWUSR|S_IRUSR, &sys_attr);
    if ( sys_mqd == (mqd_t) -1 ){
        perror("sys_mqd");
        printf("[ERROR] sys_mqd open failed!\n");
    }else{
        qDebug("sys queue opended");
    }

    qDebug("end init");
}


PyAIManager::~PyAIManager()
{
    mq_close(sys_mqd);
    mq_close(ai_mqd);
}

bool PyAIManager::proc()
{
    while ( 1 ) {
        memset(ai_buffer, 0, sizeof(ai_buffer));
        ai_result_num = mq_receive(ai_mqd, (char *)ai_buffer, 1024, NULL);
        if ( ai_result_num > 0 ) {
            vision_out_t out;
            QString ai_msg= QString(reinterpret_cast<const char*>(ai_buffer));
            qDebug()<<ai_msg;
            QJsonDocument doc = QJsonDocument::fromJson(ai_msg.toUtf8());
            QJsonObject obj;
            if(!doc.isNull())
            {
                if(doc.isObject())
                {
                    obj = doc.object();
                }
                else
                {
                    qDebug() << "Document is not an object" << endl;
                }
            }
            int cam_id =  obj.value("cam_id").toInt();
            out.cam_id = cam_id;
            bool start_decision = obj.value("start_decision").toBool();
            out.start_decision = start_decision;
            qDebug()<<"cam id: "<<cam_id<<"  , start decision: "<< start_decision;
            if(obj.contains("objs")){
                QVariant objs = obj.value("objs").toVariant();
                qDebug()<<objs;
                QJsonDocument objs_doc = QJsonDocument::fromVariant(objs);
                QVariantList list = objs_doc.toVariant().toList();
                for(int i = 0; i<list.count(); i++)
                {
                    DetectInfo di;
                    QVariantMap map = list[i].toMap();
                    di.clsId = map["obj_id"].toInt();
                    di.x_c = map["x_c"].toInt();
                    di.y_c = map["y_c"].toInt() + (map["height"].toInt())/2;
                    di.width = map["width"].toInt();
                    di.height = map["height"].toInt();
                    di.score = map["score"].toInt();
                    di.clsName = get_name(map["obj_id"].toInt());
                    out.detected.push_back(di);
                    qDebug()<<map["obj_id"].toInt();
                    qDebug()<<map["x_c"].toInt();
                    qDebug()<<map["y_c"].toInt();
                    qDebug()<<map["width"].toInt();
                    qDebug()<<map["height"].toInt();
                    qDebug()<<map["score"].toInt();
                }
            }
            emit detector_process(out);
            memset(ai_buffer, 0, sizeof(ai_buffer));
        }
    }
}

void PyAIManager::init_label()
{
    QString base_dir = _CONF->ai_basedir_;
    QString namefile =  _CONF->ai_namefile_;
    QString labelPath = QString(base_dir + namefile);
    QFile file(labelPath.toLatin1().data());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QTextStream in(&file);
    while (!in.atEnd())
        class_names_.append(in.readLine());
}

const QString PyAIManager::get_name(int obj_id)
{
    if (obj_id < 0 || obj_id > class_names_.size() - 1)
        return "unknown";

    return class_names_.at(obj_id);
}

void PyAIManager::on_open_cam_processed(const QString &file_name)
{
    QJsonObject json_obj;
    json_obj.insert("mt_status", 1);
    json_obj.insert("path", file_name);
    QJsonDocument doc(json_obj);
    QString json_str(doc.toJson(QJsonDocument::Compact));
    qDebug()<< json_str.toStdString().c_str() << " , size: "<<json_str.size();
    memset( sys_buffer, 0, 1024);
    memcpy( sys_buffer, json_str.toStdString().c_str(), json_str.size());
    int ret = mq_send(sys_mqd, (char*)&sys_buffer, 1024, 1);
    if(ret == -1){
        perror("sys_mqd send msg error");
    }else{
        qDebug("sys_mqd sent msg end\n");
    }
}

void PyAIManager::run()
{
    qDebug()<<"pyai run";
    this->proc();
}
