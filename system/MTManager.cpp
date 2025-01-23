#include "MTManager.h"
#include <QDebug>

MTManager::MTManager(QObject *parent) : QObject(parent)
{
}

MTManager::~MTManager()
{
    delete touch_dev;
}

void MTManager::start()
{
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(check_device()));
    timer->start();
}

void MTManager::start(QString &mtdev_path)
{
    mtdev_path_ = mtdev_path;
    touch_dev= new MTDevice(mtdev_path);
    connect(touch_dev,
            SIGNAL(hand_in_process(const int &)),
            this,
            SLOT(on_hand_in_process(const int &)), Qt::QueuedConnection);
    connect(touch_dev,
            SIGNAL(hands_out_process()),
            this,
            SLOT(on_hands_out_process()), Qt::QueuedConnection);
    touch_dev->start();

    //check mt has something wrong
    timer = new QTimer();
    timer->setInterval(100);
    connect(timer,SIGNAL(timeout()),this,SLOT(check_device()));
    timer->start();
    no_res_timer_.setSingleShot(true);
    connect(&no_res_timer_, SIGNAL(timeout()), this, SLOT(on_no_res_proc()));
}

int MTManager::get_shelf_id(const QString &shelf)
{
    return shelf.mid(sizeof(CONFIG_SHELF_NAME) - 1).toInt();
}

QString  MTManager::exec_shell(const char *cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return QString::fromStdString(result);
}

void MTManager::on_hand_in_process(const int &shelf_id)
{
    emit mtdev_triggered(shelf_id, true);
#ifdef DEBUG
    qDebug()<<"mtdev trigger: "<<shelf_id ;
#endif
    last_shelf_id = shelf_id;
    no_res_timer_.start(35);
}

void MTManager::hand_is_out(const int &shelf_id)
{
    devices_state[shelf_id] = false;
    emit mtdev_triggered(shelf_id, false);
}

void MTManager::on_hands_out_process()
{
#ifdef DEBUG
    qDebug("on_hands_out_process");
#endif
    devices_state.clear();
    emit mtdev_triggered(-1, false);
    no_res_timer_.stop();
}

//check mt has something wrong
void MTManager::check_device()
{
#if 1
    int fd = open(_CONF->mt_device_.toLocal8Bit().data(), O_RDONLY | O_NONBLOCK);
#else
    int fd = open(mtdev_path_.toLocal8Bit().data(), O_RDONLY | O_NONBLOCK);
#endif
    if(fd < 0){
        qDebug()<<"mt device has something wrong";
        if(!is_error){
           // emit mt_device_error(_CONF->mt_device_, 0);
            timer->stop();
        }
    }
    close(fd);
}



void MTManager::on_no_res_proc()
{
    qDebug()<<"mt no response proc";
   // emit mtdev_triggered(last_shelf_id, true);
    no_res_timer_.start(35);
}
