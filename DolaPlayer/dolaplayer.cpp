#include "dolaplayer.h"
#include "qobject.h"
#include <Windows.h>

typedef bool(*apiInit)(int, const char *, const char *);
typedef int(*apiComsumeFlvData)(char *, int);
typedef int(*apiSeekTo)(int milslec);
typedef int64_t(*apiGetFileSize)();

apiInit init = NULL;
apiSeekTo seekTo = NULL;
apiComsumeFlvData consumeData = NULL;
apiGetFileSize getFileSize = NULL;

FILE *file = NULL;
int libvlc_media_open(void *opaque, void **datap,uint64_t *sizep){
    DolaPlayer *player = (DolaPlayer*)opaque;
    if (player == NULL){
        return 0;
    } else {
        if (player->mediaOpen()){
             *sizep = getFileSize();
        } else {
            *sizep = 0;
        }
    }
    return 0;
}
ssize_t libvlc_media_read(void *opaque, unsigned char *buf,size_t len){
    int res = consumeData((char*)buf, len);
    return res;
//     DolaPlayer *player = (DolaPlayer*)opaque;
//     if (player == NULL){
//         return 0;
//     } else {
//         return player->mediaRead(buf, len);
//     }
}
int libvlc_media_seek(void *opaque, uint64_t offset){
//     if (fseek(file, offset, SEEK_SET) < 0){
//         return -1;
//     }
//     return 0;

    DolaPlayer *player = (DolaPlayer*)opaque;
    if (player == NULL){
        return 0;
    } else {
        return player->mediaSeek(offset);
    }
}
void libvlc_media_close(void *opaque){
//     if (file != NULL){
//         fclose(file);
//     }
    DolaPlayer *player = (DolaPlayer*)opaque;
    if (player != NULL){
        return player->mediaClose();
    }
}

DolaPlayer::DolaPlayer(QWidget *parent)
    : QMainWindow(parent), mIsPlaying(false){
    ui.setupUi(this);
    connect(ui.btnPlay, SIGNAL(clicked()), this, SLOT(slotPlay()));
    connect(ui.slider,SIGNAL(actionTriggered(int)), this, SLOT(slotProgressChanged(int)));


    mWnd = (HWND)ui.RenderWidget->winId();
    mInstance = libvlc_new(0, nullptr);

    mMedia = libvlc_media_new_callbacks(mInstance, libvlc_media_open, libvlc_media_read, libvlc_media_seek, libvlc_media_close, this);
    mMediaPlayer = libvlc_media_player_new_from_media(mMedia);
    libvlc_media_player_set_hwnd(mMediaPlayer, mWnd);

    initPlayerEngine();
}

DolaPlayer::~DolaPlayer(){
    libvlc_media_release(mMedia);
}

bool DolaPlayer::initPlayerEngine(){
    HINSTANCE dll = LoadLibrary(L"FlvProcess.dll");
    if (dll == NULL){
        return 0;
    }

    init = (apiInit)GetProcAddress(dll, "init");
    consumeData = (apiComsumeFlvData)GetProcAddress(dll, "comsumeFlvData");
    seekTo = (apiSeekTo)GetProcAddress(dll, "seekTo");
    getFileSize = (apiGetFileSize)GetProcAddress(dll, "getFileSize");

    if (init == NULL || consumeData == NULL || seekTo == NULL || getFileSize == NULL){
        return false;
    }

    return true;
}

bool DolaPlayer::mediaOpen(){
    return init(2, "enc.flv", "enc_to_dec.flv");
}

ssize_t DolaPlayer::mediaRead(unsigned char *buffer, size_t len){
    return consumeData((char*)buffer, len);
}
int DolaPlayer::mediaSeek(uint64_t offset){
    return 0;
}
void DolaPlayer::mediaClose(){

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