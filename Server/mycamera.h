#ifndef MYCAMERA_H
#define MYCAMERA_H

#include <QObject>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/videodev2.h>
#include <linux/types.h>
#include <asm/types.h>


#define FILE_VIDEO 	"/dev/video0"
#define IMG_WIDTH 320
#define IMG_HEIGTH 240

class myCamera: public QObject
{
public:
    myCamera(QString camera_name);
    ~myCamera();
    int get_frame(unsigned char ** pointer_yuv_buffer, size_t* len);
    int unget_frame();

private:
    struct buffer
    {
        void * start;
        size_t length;
    };

    QString camera_name;
    int fd;          //video0 file ID
    buffer* buffers;
    unsigned int n_buffers;
    int index;

    int open_camera();
    int init_camera();
    int start_capture();
    int init_mmap();
    int stop_capture();
    int uninit_camera();
    int close_camera();



};

#endif // MYCAMERA_H
