#include "AudioManager.h"

AudioManager::AudioManager()
{
    file_names[CLOSE_DOOR_NOTI] = "./close_door.mp3";
    file_names[REMOVE_CARD_NOTI] = "./remove_card.mp3";
}

AudioManager::~AudioManager()
{
    qDebug("cameraManager delete");
}

void AudioManager::addPlayer(int event, QString filePath)
{
    QMediaPlayer* player = new QMediaPlayer;
    player->setMedia(QUrl::fromLocalFile(QFileInfo(filePath).absoluteFilePath()));
    player->setVolume(80);
    players[event] = player;
}

void AudioManager::on_play_audio(const int & event_type)
{
    qDebug()<<"on_play_audio: "<<event_type;
    audio_thread = new AudioThread(nullptr, file_names[event_type]);
    audio_thread->start();
}
