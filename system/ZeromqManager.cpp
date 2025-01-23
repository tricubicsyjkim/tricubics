#include "ZeromqManager.h"

ZeromqManager::ZeromqManager()
{

    init_label();
    for(auto sender : _CONF->sender_url_list){
        sender_grabbers[sender.level_id][sender.group_id] = (ZeromqGrabber::create(CONFIG_SENDER, sender.port, sender.level_id, sender.group_id));
    }

    for(auto receiver : _CONF->receiver_url_list){
        receiver_grabbers[receiver.level_id][receiver.group_id] = (ZeromqGrabber::create(CONFIG_RECEIVER, receiver.port, receiver.level_id, receiver.group_id));

        connect(receiver_grabbers[receiver.level_id].last().data(),
                SIGNAL(level1_bbox(const QString &)),
                this,
                SLOT(on_level1_bbox(const QString &)),
                Qt::QueuedConnection);

                connect(receiver_grabbers[receiver.level_id].last().data(),
                SIGNAL(level2_bbox(const QString &)),
                this,
                SLOT(on_level2_bbox(const QString &)),
                Qt::QueuedConnection);
    }

    for(auto level_sender_pair : sender_grabbers.toStdMap()){
        for(auto sender_pair : level_sender_pair.second.toStdMap()){
            auto  grabber =  sender_pair.second;
            if(!grabber.isNull()){
                qDebug()<<"start sender grabber";
                grabber->start();
            }else{
                qDebug()<<"sender grabber is null";
            }
        }
    }


    for(auto level_receiver_pair : receiver_grabbers.toStdMap()){
        for(auto receiver_pair : level_receiver_pair.second.toStdMap()){
            auto  grabber =  receiver_pair.second;
            if(!grabber.isNull()){
                qDebug()<<"start receiver grabber";
                grabber->start();
            }else{
                qDebug()<<"receiver grabber is null";
            }
        }
    }
}

ZeromqManager::~ZeromqManager()
{

}

void ZeromqManager::init_label()
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

const QString ZeromqManager::get_name(int obj_id)
{
    if (obj_id < 0 || obj_id > class_names_.size() - 1)
        return "unknown";

    return class_names_.at(obj_id);
}

void ZeromqManager::on_transmit_image(const QString &path, const int &cam_id)
{
    sender_grabbers[1][1].data()->on_req_level1(path, cam_id);
}

void ZeromqManager::on_transmit_image(int mt_index, const QString &path, const int &cam_id)
{
    sender_grabbers[1][1].data()->on_req_level1(mt_index, path, cam_id);
}

void ZeromqManager::on_level1_bbox(const QString &bbox_data)
{
    qDebug()<<"from level1: "<<bbox_data;
    qDebug()<<"level1 to level2";
    for(auto sender_pair : sender_grabbers[2].toStdMap()){
        sender_pair.second.data()->on_req_level2(bbox_data);
    }
}

void ZeromqManager::on_level2_bbox(const QString &bbox_data)
{
    vision_out_t out;
    qDebug()<<"from level2: "<<bbox_data;
    QJsonDocument doc = QJsonDocument::fromJson(bbox_data.toUtf8());
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
    int group_id = obj.value("group_id").toInt();
    int mt_index = obj.value("mt_index").toInt();
    out.level2_group_id = group_id;
    out.cam_id = cam_id;
    out.mt_index = mt_index;
    if(obj.contains("results")){
        QVariant objs = obj.value("results").toVariant();
        QJsonDocument objs_doc = QJsonDocument::fromVariant(objs);
        QVariantList list = objs_doc.toVariant().toList();
        qDebug()<<"list size: "<<list.count();
        for(int i = 0; i<list.count(); i++)
        {
            DetectInfo di;
            QVariantMap map = list[i].toMap();
            int x1 = map["x1"].toInt();
            int x2 = map["x2"].toInt();
            int y1 = map["y1"].toInt();
            int y2 = map["y2"].toInt();
            di. x1 = map["x1"].toInt();
            di.x2 = map["x2"].toInt();
            di.y1 = map["y1"].toInt();
            di.y2 = map["y2"].toInt();
            di.cam_id = cam_id;
            di.level2_id = group_id;
            di.clsId  = map["obj_id"].toInt();
            di.x_c = x1 + (x2 - x1)/2;
            di.y_c = y2;
            di.width = (x2 - x1);
            di.height = (y2 - y1);
            di.score = map["score"].toInt();
            qDebug()<<"di.clsid: "<<di.clsId;
            di.clsName = get_name(di.clsId);
            qDebug()<<"di.clsName: "<<di.clsName;
            out.detected.push_back(di);
        }
    }
    emit detector_process(out);
}
