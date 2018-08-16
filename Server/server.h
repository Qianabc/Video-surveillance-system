#ifndef SERVER_H
#define SERVER_H

#include <QWidget>
#include "mycamera.h"

class QTcpServer;
class QTcpSocket;

namespace Ui {
class Server;
}

class Server : public QWidget
{
    Q_OBJECT
    
public:
    explicit Server(QWidget *parent = 0);
    ~Server();
    
private:
    Ui::Server *ui;
    QImage *frame;
    QTcpServer *tcpServer;     //tcp server variable
    QTcpSocket *tcpConnection; //tcp connection variable
    myCamera *camera;          //video device variable
    unsigned char * pointer_yuv_buffer; //pointer to yuv buffer (mapped from kernel)
    size_t len;
    unsigned char rgb_buffer[IMG_WIDTH*IMG_HEIGTH*3];//rgb buffer for one image frame
    bool state;
    int convert_yuv_to_rgb_buffer();

private slots:
    void sendImage();
};

#endif // SERVER_H
