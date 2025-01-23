#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include <QMap>
#include <QObject>
#include <QVector>
#include <QMap>
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QTimer>

#include "V4LGrabber.h"
#include "AIVsionDefines.h"
extern QElapsedTimer ai_proc_t;
class CameraManager : public QObject 
{
    Q_OBJECT
public:
    explicit CameraManager();
    ~CameraManager();

    void start();

    void start(uint32_t shelf_no, uint32_t cam_no);
    void stop(uint32_t shelf_no, uint32_t cam_no);

    void on_off(int mt_index, int shelf_no, bool is_on);
    void on_off(int mt_index, int shelf_no, bool camera_is_on, bool device_opened);
    void set_img_path_file(QString file_name);
    QString find_device(const char *cmd);

signals:
    void py_ai_proc (const vision_input_t &);

    //camera has something wrong
    void camera_error(const QString&, const int &);
    //zeromq
    void transmit_image(const QString&, const int &);
    //mt index
    void transmit_image(int, const QString&, const int &);

public slots:
    void on_captured(const QString &, int);
    void on_captured(int, const QString &, int);
    void  cam_timeout();

    //camera has something wrong
    void on_usb_disconnected(const QString &, int);
    void on_usb_disconnected(const QString &, int, int);
protected:
    unsigned int getShelfId(const QString &shelf);
    unsigned int getCamId(const QString &cam_device);

private:
    QMap<QString, QVector<QSharedPointer<V4LGrabber>>> devices_;

public:
    QMap<int, QFile*> img_txt_file;
    QMap<int, QTextStream*> imgs_text_stream;
    QString img_path_filename;
    QTimer cam_timer;
    bool is_opened = false;
};

#endif // CAMERAMANAGER_H
