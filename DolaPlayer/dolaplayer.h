#ifndef DOLAPLAYER_H
#define DOLAPLAYER_H

#include <QtWidgets/QMainWindow>
#include "ui_dolaplayer.h"
#define ssize_t int
#include <vlc/libvlc.h>
#include <vlc/libvlc_media.h>
#include <vlc/libvlc_events.h>
#include <vlc/libvlc_media_player.h>


class DolaPlayer : public QMainWindow
{
    Q_OBJECT

public:
    DolaPlayer(QWidget *parent = 0);
    ~DolaPlayer();

    bool initPlayerEngine();

    bool mediaOpen();
    ssize_t mediaRead(unsigned char *buffer, size_t len);
    int mediaSeek(uint64_t offset);
    void mediaClose();
private Q_SLOTS:
    void slotPlay();
    void slotProgressChanged(int);
private:
    Ui::DolaPlayerClass ui;

    libvlc_instance_t *mInstance;
    libvlc_media_t *mMedia;
    libvlc_media_player_t *mMediaPlayer;
    HWND mWnd;

    bool mIsPlaying;
};

#endif // DOLAPLAYER_H
