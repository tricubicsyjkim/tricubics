#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H
#include <QObject>
#include <QMediaPlayer>
#include <QMap>
#include <QString>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>
#include <memory>
#include "AudioThread.h"

enum NOTI_TYPE
{
    CLOSE_DOOR_NOTI=0,
    REMOVE_CARD_NOTI
};

class AudioManager: public QObject
{
    Q_OBJECT
public:
    explicit AudioManager();
    ~AudioManager() override;

    void addPlayer(int event, QString filePath);

signals:
public slots:
    void on_play_audio(const int &);

private:
    QMap<int,  QMediaPlayer *> players;
    AudioThread* audio_thread;
    QMap<int, QString> file_names;
};



#endif // AUDIOMANAGER_H
