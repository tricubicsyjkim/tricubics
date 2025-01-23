#ifndef AUDIOTHREAD_H
#define AUDIOTHREAD_H
#include<QThread>
#include<QObject>
#include<QString>
#include<QDebug>
#include <memory>

class AudioThread: public QThread
{
    Q_OBJECT
public:
    explicit AudioThread(QObject *parent = nullptr, QString file_name = "");
    ~AudioThread() override;
    bool proc();
    void exec_shell(const char *cmd);

public slots:

private:
    void run() override;
    QString file_name_;
public:
};

#endif // AUDIOTHREAD_H
