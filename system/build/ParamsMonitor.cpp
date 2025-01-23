#include "ParamsMonitor.h"

ParamsMonitor::ParamsMonitor(QObject *parent)
{
    connect(&polling_timer_, SIGNAL(timeout()), this, SLOT(on_polling()));
}

ParamsMonitor::~ParamsMonitor()
{

}

bool ParamsMonitor::proc()
{
    //    on_polling();

    while(1){
        on_polling();
        sleep(10);
    }
    return  true;
}

int ParamsMonitor::get_param_value(QString device, QString param)
{
    QString command = get_command(device, param, "get");
    QString result = run_command(command);
    if(result == ""){
        qDebug()<<"cannot get parameter : "<<param;
        return -1;
    }else{
        QStringList resultList = result.split(":");
        resultList[1] = resultList[1].trimmed();
        if(resultList.size() == 2){
            return resultList[1].toInt();
        }else{
            qDebug()<<"command result split error: "<<result;
            return -1;
        }
    }
}

void ParamsMonitor::set_param_value(QString device, QString param, int param_value)
{
    QString command = get_command(device, param, "set");
    command = QString("%1=%2").arg(command).arg(param_value);
//    qDebug()<<"set command is : "<<command;
    QString set_result = run_command(command);
    if(set_result == ""){
        qDebug()<<"reset "<<param<<" succesfull";
    }else{
        qDebug()<<"reset "<<param<<" failed";
    }
}

QString ParamsMonitor::get_command(QString device, QString param, QString type)
{
    QStringList arg_list;
    arg_list << "v4l2-ctl" << "-d" << device;
    if(type == "get"){
        arg_list << "-C";
    }else{
        arg_list << "-c";
    }
    arg_list << param;
    QString command = arg_list.join(" ");
    return command;
}

QString ParamsMonitor::run_command(QString command)
{
    QString output;
    QByteArray ba =  command.toLocal8Bit();
    const char *listStr = ba.data();
    char buffer[128];
    FILE *stream = popen(listStr, "r");
    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        output.append(buffer);
    }
    pclose(stream);
    //    qDebug()<<output;
    return output;
}

void ParamsMonitor::on_polling()
{
    for(auto cam_params : (_CONF->all_cams_params).toStdMap()){
        QString device_name = cam_params.first;
        //        qDebug()<<device_name<<"  ---" << cam_params.second;
        for(auto param : cam_params.second.toStdMap()){
            int param_value =  get_param_value(device_name, param.first);
            if(param_value == -1){
                continue;
            }else if(param_value != param.second){
                qDebug()<<"parameter: "<<param.first << " changed,  from "<<param.second<<" to "<<param_value;
                set_param_value(device_name, param.first, param.second);
            }
        }
    }
    //    polling_timer_.start(10000);
}

void ParamsMonitor::run()
{
    this->proc();
}

