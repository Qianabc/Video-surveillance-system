#include "server.h"
#include "ui_server.h"
#include <QtNetwork>

Server::Server(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Server)
{
    ui->setupUi(this);
    //启动摄像头设备
    camera = new myCamera(tr("/dev/video0"));
//    frame = new QImage(rgb_buffer,640,480,QImage::Format_RGB888);

    //启动监听套接字
    tcpServer = new QTcpServer(this);
    //设置可以监听的IP范围和端口地址
    if (!tcpServer->listen(QHostAddress::Any, 6666)) {
        qDebug() << tcpServer->errorString();
        close();
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(sendImage()));
}

Server::~Server()
{
    delete camera;
    camera = NULL;
    delete tcpServer;
    tcpServer = NULL;
    delete ui;
}

void Server::sendImage()
{
    //从摄像头获取数据
    this->len=0;
    this->pointer_yuv_buffer = NULL;
    state  = camera->get_frame(&this->pointer_yuv_buffer,&this->len);
    if(state)
    {
        convert_yuv_to_rgb_buffer();
        // 用于暂存我们要发送的数据
        frame = new QImage(rgb_buffer,320,240,QImage::Format_RGB888);
        frame->loadFromData(rgb_buffer,320 * 240 * 3);
        ui->label_video->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));
        QByteArray block;
        QDataStream out(&block, QIODevice::WriteOnly);
        // 设置数据流的版本，客户端和服务器端使用的版本要相同
        out.setVersion(QDataStream::Qt_4_0);
        int bytesWritten =  out.writeRawData((const char*)rgb_buffer,IMG_WIDTH*IMG_HEIGTH*3);
        if(!(bytesWritten == IMG_WIDTH*IMG_HEIGTH*3))
        {
            qDebug()<<"error: writing data to  socket!\n";
        }

        // 获取已经建立的连接的套接字
        QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
        connect(clientConnection, SIGNAL(disconnected()),
                clientConnection, SLOT(deleteLater()));
        clientConnection->write(block);
        clientConnection->disconnectFromHost();
        // 发送数据成功后
        camera->unget_frame();



    }
    else
    {
        qDebug()<<"error: cannot get frame from camera!\n";
    }

}

int Server::convert_yuv_to_rgb_buffer()
{


    unsigned char * pointer;
    int  i,j;
    unsigned char y1,y2,u,v;
    int r1,g1,b1,r2,g2,b2;
    pointer = pointer_yuv_buffer;
    for(i=0;i<IMG_HEIGTH;i++)
     {
         for(j=0;j<(IMG_WIDTH/2);j++)
         {
             y1 = *( pointer + (i*(IMG_WIDTH/2)+j)*4);
             u  = *( pointer + (i*(IMG_WIDTH/2)+j)*4 + 1);
             y2 = *( pointer + (i*(IMG_WIDTH/2)+j)*4 + 2);
             v  = *( pointer + (i*(IMG_WIDTH/2)+j)*4 + 3);

             r1 = y1 + 1.042*(v-128);
             g1 = y1 - 0.34414*(u-128) - 0.71414*(v-128);
             b1 = y1 + 1.772*(u-128);

             r2 = y2 + 1.042*(v-128);
             g2 = y2 - 0.34414*(u-128) - 0.71414*(v-128);
             b2 = y2 + 1.772*(u-128);

             if(r1>255)
                 r1 = 255;
             else if(r1<0)
                 r1 = 0;

             if(b1>255)
                 b1 = 255;
             else if(b1<0)
                 b1 = 0;

             if(g1>255)
                 g1 = 255;
             else if(g1<0)
                 g1 = 0;

             if(r2>255)
                 r2 = 255;
             else if(r2<0)
                 r2 = 0;

             if(b2>255)
                 b2 = 255;
             else if(b2<0)
                 b2 = 0;

             if(g2>255)
                 g2 = 255;
             else if(g2<0)
                 g2 = 0;

             *(rgb_buffer + (i*(IMG_WIDTH/2)+j)*6    ) = (unsigned char)r1;
             *(rgb_buffer + (i*(IMG_WIDTH/2)+j)*6 + 1) = (unsigned char)g1;
             *(rgb_buffer + (i*(IMG_WIDTH/2)+j)*6 + 2) = (unsigned char)b1;
             *(rgb_buffer + (i*(IMG_WIDTH/2)+j)*6 + 3) = (unsigned char)r2;
             *(rgb_buffer + (i*(IMG_WIDTH/2)+j)*6 + 4) = (unsigned char)g2;
             *(rgb_buffer + (i*(IMG_WIDTH/2)+j)*6 + 5) = (unsigned char)b2;
         }

     }


    return 0;
}
