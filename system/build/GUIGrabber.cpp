#include "GUIGrabber.h"


GUIGrabber::GUIGrabber()
{
    context = zmq_ctx_new ();
    requester = zmq_socket (context, ZMQ_PUSH);
    zmq_bind (requester, "tcp://0.0.0.0:6999");
}

GUIGrabber::~GUIGrabber()
{
    if(requester != nullptr){
        zmq_close (requester);
    }
    zmq_ctx_destroy (context);
}

void GUIGrabber::send_gui_msg(int cls_id, const QString status)
{
    qDebug()<<"req cls_id: "<<cls_id<<" , status: "<<status;
    QJsonObject json_obj;
    json_obj.insert("cls_id", cls_id);
    json_obj.insert("status", status);
    QJsonDocument doc(json_obj);
    QString strJson(doc.toJson(QJsonDocument::Compact));
    qDebug()<< strJson.toStdString().c_str() << " , size: "<<strJson.size();
    memset( req_buffer, 0, 1024);
    memcpy( req_buffer, strJson.toStdString().c_str(), strJson.size());
    zmq_send (requester, req_buffer, 1024, 0);
}
