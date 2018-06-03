#include "dolaplayer.h"
#include "qobject.h"
#include <Windows.h>
FILE *file = NULL;
int libvlc_media_open(void *opaque, void **datap,uint64_t *sizep){
    fopen_s(&file, "test.f4v", "rb");
    if (file != NULL){
        fseek(file, 0, SEEK_END);
        *sizep = ftell(file);
        rewind(file);
        return 0;
    }
    return 0;
}
ssize_t libvlc_media_read(void *opaque, unsigned char *buf,size_t len){
    auto res = fread(buf, 1, len, file);
    if (res == 0){
        return feof(file) != 0 ? 0 : -1;
    }
    return res;
}
int libvlc_media_seek(void *opaque, uint64_t offset){
    if (fseek(file, offset, SEEK_SET) < 0){
        return -1;
    }
    return 0;
}
void libvlc_media_close(void *opaque){
    if (file != NULL){
        fclose(file);
    }
}

DolaPlayer::DolaPlayer(QWidget *parent)
    : QMainWindow(parent), mIsPlaying(false){
    ui.setupUi(this);
    connect(ui.btnPlay, SIGNAL(clicked()), this, SLOT(slotPlay()));
    connect(ui.slider,SIGNAL(actionTriggered(int)), this, SLOT(slotProgressChanged(int)));


    mWnd = (HWND)ui.RenderWidget->winId();
    mInstance = libvlc_new(0, nullptr);

    mMedia = libvlc_media_new_callbacks(mInstance, libvlc_media_open, libvlc_media_read, libvlc_media_seek, libvlc_media_close, NULL);
    mMediaPlayer = libvlc_media_player_new_from_media(mMedia);
    libvlc_media_player_set_hwnd(mMediaPlayer, mWnd);
}

DolaPlayer::~DolaPlayer(){
    libvlc_media_release(mMedia);
}

void DolaPlayer::slotPlay(){
    if (libvlc_media_player_is_playing(mMediaPlayer)) {
        libvlc_media_player_pause(mMediaPlayer);
        ui.btnPlay->setText("play");
    } else {
        ui.btnPlay->setText("pause");
        libvlc_media_player_play(mMediaPlayer);
        //Sleep(10000);
        //qint64 duration = libvlc_media_player_get_time(mMediaPlayer);
        ui.slider->setRange(0, 30 * 1000);
    }
}

void DolaPlayer::slotProgressChanged(int value){
    Q_UNUSED(value);
    qint64 time = ui.slider->value();

    if (libvlc_media_player_is_playing(mMediaPlayer)){
         libvlc_media_player_set_time(mMediaPlayer, time);
    }
}