#include "AudioThread.h"

AudioThread::AudioThread(QObject *parent, QString file_name)
{

    qDebug()<<parent<<file_name;
    file_name_ = file_name;
}

AudioThread::~AudioThread()
{

}

bool AudioThread::proc()
{
    qDebug()<<"run audioThread";
    QString cmd = "sh play_audio.sh "+file_name_;
    exec_shell(cmd.toLocal8Bit().data());
    return  true;
}

void AudioThread::exec_shell(const char *cmd)
{
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
}

void AudioThread::run()
{
    proc();
}
