#include "Weight_rs232.h"

#include <QDebug>
#include <StlTemplate.h>
#include "SensorDAQImpl.h"
#include "Instance.h"

#define WEIGHT_SEND_ECHO_LEN 8 // len for ECHO packet
#define WEIGHT_SENSOR_ECHO_MODE

#define WEIGHT_PROTOCOL_MAX_LEN 17  // receive len for RND
#define WEIGHT_PROTOCOL_BODY_LEN 15 // excluding stx + etx

// command lists
/*
SFS 0x53 0x46 0x53 Set FIR Tab Size(0=80, 1=40, 2=20, 3=10) 0~3
SSR 0x53 0x53 0x52 Set Software Reset -
SWS 0x53 0x57 0x53 ADC Average Filter Window Size 1~50
SMF 0x53 0x4D 0x46 Set ADC Median Filter (0=OFF, 1=SET) 0, 1
SBR 0x53 0x42 0x52 Set BaudRate [BPS] (4=9600, 5=19200, 6=38400, 7=57600, 8=115200) 4~8
SAS 0x53 0x41 0x53 ADC converting speed Setting(0=10Hz, 1=80Hz) 0,1
SID 0x53 0x49 0x44 Device ID Setting or Answer 1~99
SPI 0x53 0x50 0x49 Set Parameter Init(1=Initial) 0,1
RMF 0x52 0x4D 0x46 Read ADC Median Filter Setting(0=OFF, 1=ON) 0,1
RWS 0x52 0x57 0x53 Read ADC Average Filter Window Size 1~50
RFS 0x52 0x46 0x53 Read FIR Tab Size(0=80, 1=40, 2=20, 3=10) 0~3
RRC 0x52 0x52 0x43 Read Diagnosis of Temperature Stauts
RRP 0x52 0x52 0x50 Read Diagnosis of Voltage Stauts(Ext and LC)
RRS 0x52 0x52 0x53 Read Diagnosis of Stable
RRL 0x52 0x52 0x4C Read Diagnosis of Load State
RNR 0x52 0x4E 0x52 Read Normalized Resolution
RVR 0x52 0x56 0x52 Read ROM Version
RMC 0x52 0x4D 0x43 Read Loadcell Max Capacity
RBR 0x52 0x42 0x52 Read BaudRate[BPS] (4=9600, 5=19200, 6=38400, 7=57600, 8=115200) 4~8
RSN 0x52 0x53 0x4E Read Serial Number
RLN 0x52 0x4C 0x4E Read Loadcell Name
RFD 0x52 0x46 0x44 Read Filtered Data
RND 0x52 0x4E 0x44 Read Normalized Data
RAD 0x52 0x41 0x44 Read raw AD Data
RID 0x52 0x49 0x44 Read Device ID (Device Address)
*/

// data format
/*
** input cmd : RND (8byte)
    stx[0]	cmd[1..3]	id[4] 	reserve[5]		bcc[6]
etx[7]
Tx:	2 		52 4e 44 	fe 		30
96 			3

** receive cmd : GND (17byte)
    stx[0]	cmd[1..3] 	id[4]	status[5..6]	sign[7]	value[8..14]
bcc[15]	etx[16]
Rx:	2 		47 4e 44 	fe 		c1 	80
2b 		30 30 30 30 33 34 39            7         	3

Tx:2 52 4e 44 fe 30 96 3
Rx:2 47 4e 44 fe c0 80 2d 30 30 31 33 37 33 30 e8 3
Weight changes: -2632 => -2288 (344g)
weight=344g: Sprite IN.
*/

// 96,000bps 8 Bits, No parity, Stop 1bit

Weight_rs232::Weight_rs232(const QString &device)
    : device_(device), serial_(new QSerialPort(this)), seq_id_(0)
{
    connect(serial_, SIGNAL(readyRead()), this, SLOT(readData()));
    connect(serial_, SIGNAL(error(QSerialPort::SerialPortError)), this,
            SLOT(handleError(QSerialPort::SerialPortError)));
    connect(this, SIGNAL(sendData(const QByteArray &)), this, SLOT(writeData(const QByteArray &)));

    polling_timer_.setSingleShot(true);
    connect(&polling_timer_, SIGNAL(timeout()), this, SLOT(on_polling()));
    connect(&open_door_timer, SIGNAL(timeout()), this, SLOT(open_door_timeout()));
    connect(&close_door_timer, SIGNAL(timeout()), this, SLOT(close_door_timeout()));
}

Weight_rs232::~Weight_rs232()
{
    closeSerialPort();
    polling_timer_.stop();
}

void Weight_rs232::handleError(QSerialPort::SerialPortError error)
{
    //    if(error != 0){
    //        qDebug()<<"ERROR: "<<error;
    //        if(!is_error){
    //            is_error = true;
    //            emit usb_disconnected();
    //        }
    //    }
    if (error == QSerialPort::ResourceError || error == QSerialPort::ReadError) {
        qDebug("* %s Critical Error : %s", qPrintable(device_), qPrintable(serial_->errorString()));
        closeSerialPort();
        QString serial_path =  find_serial_path("bash ./find_serial.sh ioboard");
        int error_index = 0;
        while(serial_path.length() <=0 && error_index < 500){
            qDebug()<<"search serial port again";
            error_index++;
            serial_path = find_serial_path("bash ./find_serial.sh ioboard");
        }
        error_index = 0;
        qDebug()<<"found tty: "<<serial_path;
        if(serial_path != ""){
            device_ = serial_path;
            start();
        } else{
            emit usb_disconnected();
            qDebug()<<"send usb disconnected signal to main thread";
        }
    }else{
        qDebug()<<"cannot catch error: "<<qPrintable(serial_->errorString());
    }
}

bool Weight_rs232::openSerialPort(QString dev)
{
    // newtio.c_cflag = B9600 | CS8 | CLOCAL | CREAD /*  | CSTOPB */;
    serial_->setPortName(dev);
    serial_->setBaudRate(QSerialPort::Baud9600);  //Baud9600  Baud115200
    serial_->setDataBits(QSerialPort::Data8);
    serial_->setParity(QSerialPort::NoParity);
    serial_->setStopBits(QSerialPort::OneStop);
    serial_->setFlowControl(QSerialPort::NoFlowControl);

    if (serial_->open(QIODevice::ReadWrite)) {
        qDebug("* %s connected...", qPrintable(dev));
        device_ = dev;
        if(_CONF->project_name_ != "cheaper")
            polling_timer_.start();
    } else {
        qDebug("* %s Open error...", qPrintable(dev));
        return false;
    }
    return true;
}

void Weight_rs232::closeSerialPort()
{
    if (serial_->isOpen())
        serial_->close();
    qDebug("* %s Disconnected...", qPrintable(device_));
}

void Weight_rs232::add_scale_id(int id)
{
    id_.push_back(id);
}

void Weight_rs232::next_seq_id()
{
    if (id_.size() <= 1)
        return;
    seq_id_ = (seq_id_ + 1) % id_.size();
    //qDebug()<<"next_seq_id: "<<seq_id_;
}

unsigned char Weight_rs232::checkSum(char *buf, char len)
{

    int i, sum = 0;
    for(i = 0; i < len; i++)
    {
        sum += buf[i];
    }
    return (unsigned char)(sum%256);
}

QString Weight_rs232::find_serial_path(const char *cmd)
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
    QString event_id = QString::fromStdString(result);
    return event_id;
}

void Weight_rs232::set_timer(int duration)
{
    qDebug()<<"setTImer in Weight_rs232: "<<duration;
    close_door_timer.stop();
    close_door_timer.start(duration);
}

int Weight_rs232::get_lock_state()
{
    qDebug()<<"lock_status_: "<<lock_status_;
    return lock_status_;
}

int Weight_rs232::get_door_closed_state()
{
    QString ioboard_verion =  _CONF->ioboard_version_;
    if(ioboard_verion == "newCBox"){
        return 0x0f;
    }else{
        return 0x03;
    }
}

int Weight_rs232::get_door_unlock_state()
{
    QString ioboard_verion =  _CONF->ioboard_version_;
    if(ioboard_verion == "newCBox"){
        return 0x0a;
    }else{
        return 0x02;
    }
}

void Weight_rs232::sendCommandImpl(int id, const QByteArray &arr)
{
    char sendbuf[8];
    sendbuf[0] = 0x02;
    sendbuf[1] = arr.at(0);
    sendbuf[2] = arr.at(1);
    sendbuf[3] = arr.at(2);
    sendbuf[5] = 0x30; // Reserved.
    if(arr == "REQ"){
        sendbuf[4] = (static_cast<char>(id));
        sendbuf[5] = 0x05;
        sendbuf[6] = 0x01;
    }else if(arr == "RND"){
        sendbuf[4] = (0xFF - static_cast<char>(id));
        sendbuf[6] = sendbuf[1] ^ sendbuf[2] ^ sendbuf[3] ^ sendbuf[4] ^ sendbuf[5];
    }
    sendbuf[7] = 0x03;
    emit sendData(QByteArray(sendbuf, 8));
}

void Weight_rs232::send_command_relay(char cmd)
{
    emit sendData(QByteArray(&cmd, 1));
}

void Weight_rs232::send_sensor_command(int id)
{
    send_command_rnd(id);
}

void Weight_rs232::send_command_rid(int id) // Read Device ID
{
    sendCommandImpl(id, "RID");
}

void Weight_rs232::send_command_rnd(int id) // Read Normalized Data
{
    sendCommandImpl(id, "RND");
}

void Weight_rs232::send_command_req(char cmd)
{
    int id = static_cast<int>(cmd);
    sendCommandImpl(id, "REQ");
}

void Weight_rs232::start()
{
    if (device_ == "") {
        qDebug("* [%s] null device ???", qPrintable(device_));
        return;
    }
    qDebug()<<"serial device_:"<<device_;
    openSerialPort(device_);
}

void Weight_rs232::stop()
{
    if (!serial_->isOpen())
        return;

    if (device_ == "") {
        qDebug("* [%s] null device ???", qPrintable(device_));
        return;
    }
    closeSerialPort();
}

void Weight_rs232::stopPolling()
{
    polling_timer_.stop();
}

void Weight_rs232::startPolling()
{
    polling_timer_.start();
}

void Weight_rs232::writeData(const QByteArray &data)
{
    if (!serial_->isOpen())
        return;

#if 0
    qDebug("***** Weight_rs232 rs232 Packet (Len:%d)", data.count());
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Writting Raw Data to Log file
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        std::string sRawData = "raw [";
        for(int i = 0; i< data.size(); i++) {
            sRawData += (ToZeroHexaString(static_cast<uint8_t>(data.at(i)), 2) + " ");
        }
        sRawData += "] ";
        qDebug() << sRawData.c_str();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

    serial_->write(data);
}

bool Weight_rs232::formatCheck(const QByteArray &rdata)
{
    return (rdata.at(0) == CHAR_STX &&
            rdata.at(WEIGHT_PROTOCOL_MAX_LEN - 1) == CHAR_ETX);
}

bool Weight_rs232::bccCheck(const QByteArray &rdata)
{
    char bcc1 = rdata.at(WEIGHT_PROTOCOL_BODY_LEN);
    char bcc2 = 0;
    for (int i = 1; i < WEIGHT_PROTOCOL_BODY_LEN; i++) {
        bcc2 ^= rdata.at(i);
    }

    bool valid = (bcc1 == bcc2);

    if (!valid) {
        //qDebug() << "bcc1: " << bcc1 << " bcc2: " << bcc2 << " => " << ((bcc1 == bcc2) ? "OK" : "BAD");
    }

    return valid;
}

void Weight_rs232::readData()
{
    packets_.append(serial_->readAll());
    //////////////////////////////////////////////////////////////////////////////////////////////
#if 0
    {
        qDebug()<<"read data";
        std::string sRawData = "raw [";
        for(int i = 0; i< packets_.size(); i++) {
            sRawData += (ToZeroHexaString(static_cast<uint8_t>(packets_.at(i)), 2) + " ");
        }
        sRawData += "] ";
        qDebug() << sRawData.c_str();
    }
#endif
    //////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WEIGHT_SENSOR_ECHO_MODE
    // todo: rs485 echo suppression is needed through hardware design.
    int s_pos = packets_.indexOf(CHAR_STX);

    if (s_pos > 0) {
        packets_.remove(0, s_pos);
    } else if (s_pos < 0) {
        packets_.clear();
        return;
    }

    int e_pos = packets_.indexOf(CHAR_ETX);
    switch (e_pos) {
    case -1:
        return;
    case WEIGHT_PROTOCOL_MAX_LEN - 1:
        break;
    default: {
        packets_.remove(0, e_pos);
        break;
    }
    }
#else
    int s_pos = packets_.indexOf(CHAR_STX);
    if (s_pos > 0) {
        packets_.remove(0, s_pos);
    } else if (s_pos < 0) {
        packets_.clear();
        return;
    }
#endif

    if (packets_.at(0) != CHAR_STX) {
        return;
    }

    if (packets_.size() < WEIGHT_PROTOCOL_MAX_LEN) {
        return;
    }

    while(packets_.size() != 0){
        QByteArray rdata = packets_.mid(0, WEIGHT_PROTOCOL_MAX_LEN);
        packets_.remove(0, WEIGHT_PROTOCOL_MAX_LEN);
        QByteArray event_type = rdata.mid(1,3);
        if(event_type == "RSP"){
            qDebug()<<rdata.toHex();
            qDebug()<<"ioboard response"<<rdata.at(4) ;
            char c = rdata.at(7); // or any other character
            QBitArray bits(8); // assuming a char is 8 bits in size
            for (int i = 0; i < 8; i++) {
                bits.setBit(i, (c & (1 << i)) != 0);
            }
            qDebug()<<"bits: "<<bits;

            if(rdata.at(4) == 0x01){
                emit mcu_alive();
            }else  if(rdata.at(4) == 0x10){
                emit usb_on();
            }else  if(rdata.at(4) == 0x11){
                emit usb_off();
            }else  if(rdata.at(4) == 0x12){
                emit usb_reset();
            }else  if(rdata.at(4) == 0x13){
                int port_id = rdata.at(7);
                qDebug()<<"port_id: "<<port_id;
                emit usb_status(port_id);
            }else if(rdata.at(4) == 0x20){
                int closed_state = get_door_closed_state();
                qDebug()<<"closed_state: "<<closed_state;
                if((bits[0] == true && bits[1] == true) || (bits[2] == true && bits[3] == true)){
                    qDebug()<<"door closed in 0x20";
                    if(lock_status_ != LOCK_CLOSED){
                        int interval = (int)(opened_door_t.elapsed());
                        qDebug()<<"open --- close door: "<<interval;
                        if(interval > INITIAL_TIMER || lock_status_ == LOCK_ALL_OPENED ){
                            lock_status_ = LOCK_CLOSED;
                            qDebug()<<"stop lock_timer";
                            close_door_timer.stop();
                            emit door_closed();
                        }
                    }
                }
            }else  if(rdata.at(4) == 0x21 || (rdata.at(4) == 0x22)){
                int unlock_state = get_door_unlock_state();
                qDebug()<<"unlock_state: "<<unlock_state;
                if(rdata.at(7) == 0x00 || rdata.at(7) == unlock_state){
                    qDebug()<<"door opended in rs232";
                    qDebug()<<rdata.toHex();
                    close_door_timer.start(INITIAL_TIMER);
                    opened_door_t.restart();
                    if(lock_status_ == LOCK_CLOSED){
                        lock_status_ = LOCK_OPENED;
                        qDebug()<<"opened_door_t start";
                        open_door_timer.stop();
                        qDebug()<<"stop open door timer";
                        emit door_opened();

                        if(rdata.at(7) == 0x00){
                            qDebug()<<"door all opened in rs232";
                            lock_status_ = LOCK_ALL_OPENED;
                            close_door_timer.stop();
                            emit lock_all_opened();
                        }
                    }else if(lock_status_ == LOCK_OPENED){
                        if(rdata.at(7) == 0x00){
                            qDebug()<<"door all opened in rs232";
                            lock_status_ = LOCK_ALL_OPENED;
                            close_door_timer.stop();
                            emit lock_all_opened();
                        }
                    }else if(lock_status_ == DEADBOLT_ERROR){
                        if(rdata.at(7) == 0x00) lock_status_ = LOCK_ALL_OPENED;
                        if(rdata.at(7) == unlock_state)  lock_status_ = LOCK_OPENED;
                        emit reset_deadbolt();
                    }
                }else if( (bits[0] == true && bits[1] == true) || (bits[2] == true && bits[3] == true)){
                    if(lock_status_ != LOCK_CLOSED){
                        int interval = (int)(opened_door_t.elapsed());
                        qDebug()<<"open --- close door: "<<interval;
                        if(interval > INITIAL_TIMER || lock_status_ == LOCK_ALL_OPENED ){
                            lock_status_ = LOCK_CLOSED;
                            qDebug()<<"stop lock_timer";
                            close_door_timer.stop();
                            emit door_closed();
                        }
                    }
                }else{
                    if((bits[0] == true && bits[1] == false) || (bits[2] == true && bits[3] == false)){
                        if(lock_status_ != DEADBOLT_ERROR && lock_status_ != LOCK_CLOSED){
                            lock_status_ = DEADBOLT_ERROR;
                            emit deadbolt_error();
                        }
                    }else{
                        qDebug()<<"uncatch lock status: "<<rdata.at(7);
                    }
                }
            }
        }else if(event_type == "GND"){
            int scale_id = uint8_t(0xff - rdata.at(4));
            int sign = uint8_t(0xff - rdata.at(7));
            sign =  (sign == 0 ? 1 : -1);
            QString val = rdata.mid(8, 7);
            int ret_value = sign*(val.toInt());
            next_seq_id();
            emit weight_value(scale_id, ret_value);
        }
    }
}

void Weight_rs232::on_polling()
{
    send_sensor_command(id_[seq_id_]);
    //next_seq_id();
    polling_timer_.start(static_cast<int>(_CONF->weight_polling_interval_));
}

void Weight_rs232::open_door_timeout()
{
    if(lock_status_ == LOCK_CLOSED){
        send_command_req(0x21);
        qDebug()<<"open the door again";
        open_door_timer.start(500);
    }
}

void Weight_rs232::close_door_timeout()
{
    if(lock_status_ == LOCK_OPENED){
        send_command_req(0x20);
        qDebug()<<"clsoe the door auto";
        close_door_timer.start(500);
    }
}

void Weight_rs232::on_reset_error_prop()
{
    qDebug()<<"reset error prop";
    is_error = false;
}
