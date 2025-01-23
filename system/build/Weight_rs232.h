#ifndef WEIGHT_RS232_H
#define WEIGHT_RS232_H

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QByteArray>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QVector>
#include <QElapsedTimer>
#include <QElapsedTimer>
#include <QBitArray>

#define INITIAL_TIMER 10000

enum lock_status
{
    LOCK_OPENED = 0,
    LOCK_ALL_OPENED,
    LOCK_CLOSED,
    DEADBOLT_ERROR
};
class Weight_rs232 : public QObject
{
    Q_OBJECT

public:
    explicit Weight_rs232(const QString& device);
    ~Weight_rs232();

    void start();
    void stop();
    void stopPolling();
    void startPolling();
    void add_scale_id(int id);

    bool formatCheck(const QByteArray& rdata);
    bool bccCheck(const QByteArray& rdata);
    bool idCheck(const QByteArray& rdata);

    void sendCommandImpl(int id, const QByteArray& arr);
    void send_command_relay(char cmd);
    void send_sensor_command(int id);
    void send_command_rid(int id);    // Read Device ID
    void send_command_rnd(int id);    // Read Normalized Data
    void send_command_req(char cmd);

    void next_seq_id();
    unsigned char checkSum(char* buf, char len);

    QString find_serial_path(const char *cmd);

    void set_timer(int duration);
    int get_lock_state();
    int get_door_closed_state();
    int get_door_unlock_state();

signals:
    void sendData(const QByteArray &);
    void weight_value(int, int);
    void mcu_alive();
    void usb_on();
    void usb_off();
    void usb_reset();
    void usb_status(int);
    void door_opened();
    void door_closed();
    void usb_disconnected();
    void lock_all_opened();
    void deadbolt_error();
    void reset_deadbolt();
public slots:

    void writeData(const QByteArray &data);
    void readData();

    void handleError(QSerialPort::SerialPortError error);

    void on_polling();

    void  open_door_timeout();
    void close_door_timeout();
    void on_reset_error_prop();

protected:
    bool openSerialPort(QString);
    void closeSerialPort();
    QTimer open_door_timer;
    QTimer close_door_timer;

private:
    QTimer polling_timer_;
    QString device_;
    QSerialPort *serial_ = nullptr;
    QVector<int> id_;

    int seq_id_;

    QByteArray packets_;
    bool is_error = false;
    int lock_status_ = LOCK_CLOSED;
    QElapsedTimer opened_door_t;
};

#endif // WEIGHT_RS232_H
