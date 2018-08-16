#include "mycamera.h"


myCamera::myCamera(QString camera_name)
{
    this->camera_name = camera_name;
    this->fd = -1;
    this->buffers = NULL;
    this->n_buffers = 0;
    this->index = -1;

    if(open_camera() == false)
    {
        close_camera();
    }

    if(init_camera() == false)
    {
        close_camera();
    }

    if(start_capture() == false)
    {
        stop_capture();
        close_camera();
    }
}

myCamera::~myCamera()
{
    stop_capture();
    uninit_camera();
    close_camera();
}


int myCamera::open_camera()
{
    fd = open(FILE_VIDEO,O_RDWR);
    if(fd == -1)
    {
        return false;
    }
    return true;
}

int myCamera::init_camera()
{
    v4l2_capability cap;
    v4l2_format fmt;
    v4l2_streamparm setfps;

    if(ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1)
    {
        return false;
    }


    //set fmt
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.height = IMG_HEIGTH;
    fmt.fmt.pix.width = IMG_WIDTH;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
    {
            return false;
    }


    //set fps
    setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    setfps.parm.capture.timeperframe.numerator = 1;
    setfps.parm.capture.timeperframe.denominator = 20;

    if(ioctl(fd, VIDIOC_S_PARM, &setfps) == -1)
    {
        return false;
    }


    //mmap
    if(init_mmap() == false )
    {
        return false;
    }

    return true;
}


int myCamera::init_mmap()
{
    v4l2_requestbuffers req;

    req.count = 2;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_REQBUFS, &req) == -1)
    {
        return false;
    }

    if(req.count < 2)
    {
        return false;
    }

    buffers = (buffer*)calloc(req.count, sizeof(*buffers));

    if(!buffers)
    {
        return false;
    }

    for(n_buffers = 0; n_buffers < req.count; ++n_buffers)
    {
        v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if(ioctl(fd, VIDIOC_QUERYBUF, &buf) == -1)
        {
            return false;
        }

        buffers[n_buffers].length = buf.length;
        buffers[n_buffers].start =
                mmap(NULL, // start anywhere// allocate RAM*4
                     buf.length,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     fd, buf.m.offset);

        if(MAP_FAILED == buffers[n_buffers].start)
        {
            return false;
        }
    }

    return true;
}

int myCamera::stop_capture()
{
    v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(fd, VIDIOC_STREAMOFF, &type) == -1)
    {
        return false;
    }
    return true;
    
}

int myCamera::start_capture()
{
    unsigned int i;
    for(i = 0; i < n_buffers; ++i)
    {
        v4l2_buffer buf;
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory =V4L2_MEMORY_MMAP;
        buf.index = i;
        if(-1 == ioctl(fd, VIDIOC_QBUF, &buf))
        {
            return false;
        }
    }

    v4l2_buf_type type;
    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if(ioctl(fd, VIDIOC_STREAMON, &type)==-1)
    {
        return false;
    }
    return true;
}

int myCamera::uninit_camera()
{
    unsigned int i;

    for(i = 0; i < n_buffers; ++i)
    {
        if(-1 == munmap(buffers[i].start, buffers[i].length))
        {
            return false;
        }

    }

    delete buffers;
    return true;
}

int myCamera::close_camera()
{
    int flag;
    flag = close(fd);
    if( flag == -1)
    {
        return false;
    }
    return true;
}

int myCamera::get_frame(unsigned char ** pointer_yuv_buffer, size_t* len)
{
    v4l2_buffer queue_buf;

    queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    queue_buf.memory = V4L2_MEMORY_MMAP;

    if(ioctl(fd, VIDIOC_DQBUF, &queue_buf) == -1)
    {
        return false;
    }

    *pointer_yuv_buffer = (unsigned char *)buffers[queue_buf.index].start;
    *len = buffers[queue_buf.index].length;
    index = queue_buf.index;

    return true;

}

int myCamera::unget_frame()
{
    if(index != -1)
    {
        v4l2_buffer queue_buf;
        queue_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        queue_buf.memory = V4L2_MEMORY_MMAP;
        queue_buf.index = index;

        if(ioctl(fd, VIDIOC_QBUF, &queue_buf) == -1)
        {
            return false;
        }
        return true;
    }
    return false;
}

