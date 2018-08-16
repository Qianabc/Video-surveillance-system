#include "client.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QtNetwork>
#include <QTime>

Client::Client(QWidget *parent)
    : QWidget(parent)
{
    QHBoxLayout *line = new QHBoxLayout(this);
    QVBoxLayout *virth =new QVBoxLayout();
    label = new QLabel();
//    QLabel *label1 = new QLabel();
    line->addWidget(label,5,Qt::AlignTop);
    line->addLayout(virth,1);
    connects = new QPushButton("connect",this);
    connect(connects,SIGNAL(clicked()),this,SLOT(on_pushButton_link_clicked()));

    disconnects = new QPushButton("disconnect",this);
    connect(disconnects,SIGNAL(clicked()),this,SLOT(on_pushButton_unlink_clicked()));
    closes = new QPushButton("exit",this);
    connect(closes,SIGNAL(clicked()),this,SLOT(on_pushButton_closes_clicked()));
//    virth->addStretch(1);
    virth->addWidget(connects,0,Qt::AlignLeft | Qt::AlignVCenter);
    virth->addWidget(disconnects,0,Qt::AlignLeft | Qt::AlignVCenter);
    virth->addWidget(closes,0,Qt::AlignLeft | Qt::AlignVCenter);
//    virth->addWidget(lineedit,0,Qt::AlignLeft | Qt::AlignVCenter);
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
               this, SLOT(displayError(QAbstractSocket::SocketError)));
    connects->setEnabled(true);

}


Client::~Client()
{
    delete tcpSocket;
    tcpSocket = NULL;
}

void Client::newConnect()
{

    // 取消已有的连接
    tcpSocket->abort();
    tcpSocket->connectToHost("10.11.55.247",6666);
}

void Client::readMessage()
{
    QDataStream in(tcpSocket);
    // 设置数据流版本，这里要和服务器端相同
    in.setVersion(QDataStream::Qt_4_6);

    // 如果没有得到全部的数据，则返回，继续接收数据
    if(tcpSocket->bytesAvailable() < IMG_WIDTH*IMG_HEIGTH*3) return;
    // 将接收到的数据存放到变量中
    in.readRawData((char *)rgb_buffer,IMG_WIDTH*IMG_HEIGTH*3);
    // 显示接收到的数据
    frame = new QImage(rgb_buffer,IMG_WIDTH,IMG_HEIGTH,QImage::Format_RGB888);
    label->setPixmap(QPixmap::fromImage(*frame,Qt::AutoColor));
    label->setFixedSize(1680,960);
//    label->setFixedSize(640,480);
    label->setScaledContents(true);
}



void Client::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}



void Client::timerEvent(QTimerEvent *)
{
    newConnect();
}

void Client::on_pushButton_link_clicked()
{
    delay_time=true;
    connects->setEnabled(false);
    disconnects->setEnabled(true);

    while(delay_time)
    {
        QTime reachTime =QTime::currentTime().addMSecs(400);
        while(QTime::currentTime()<reachTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
        newConnect();

    }

}

void Client::on_pushButton_unlink_clicked()
{
//    killTimer(id);
    delay_time=false;
    connects->setEnabled(true);
}
void Client::on_pushButton_closes_clicked()
{
    delay_time=false;
    close();
}
