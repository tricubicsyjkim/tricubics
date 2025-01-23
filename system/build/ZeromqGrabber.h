#ifndef ZEROMQGRABBER_H
#define ZEROMQGRABBER_H
#include <QThread>
#include <QSharedPointer>
#include <QString>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QString>
#include <QProcessEnvironment>
#include <zmq.h>
#include "Instance.h"
#include "Config.h"
#include "AIVsionDefines.h"
//#define test

#define SNACK

class ZeromqGrabber : public QThread
{
    Q_OBJECT
public:
    explicit ZeromqGrabber(const QString pole, int port, int level_id, int group_id);
    ~ZeromqGrabber() override;
    static QSharedPointer<ZeromqGrabber> create(const QString pole, int port, int level_id, int group_id);
    QString  get_ip_addr(int port);
    bool proc();
    void parse_res(QString &res_msg);

signals:
    void level1_bbox(const QString &);
    void level2_bbox(const QString &);

public slots:
    void on_req_level1(const QString&, const int &);
    //mt index
    void on_req_level1(int, const QString&, const int &);
    void on_req_level2(const QString&);
private:
    ZeromqGrabber(const ZeromqGrabber &);
    ZeromqGrabber &operator=(const ZeromqGrabber &);

    void run() override;



protected:
    QString pole_;
    int port_;
    int level_id_;
    int group_id_;
    QString ip_addr_;
    bool quit_= false;

public:
    void *context;
    void *sender;
    void *receiver;
    char req_buffer[1024];
    char res_buffer[1024];
};

#endif // ZEROMQGRABBER_H
