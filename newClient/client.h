#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QAbstractSocket>
#include <QLabel>
#include <QPushButton>

#define IMG_WIDTH 320
#define IMG_HEIGTH 240

class QTcpSocket;

namespace Ui {
class Client;
}

class Client : public QWidget
{
    Q_OBJECT

public:
    explicit Client(QWidget *parent = 0);
    ~Client();
//    QPushButton *label;

private:
    Ui::Client *ui;
    QTcpSocket *tcpSocket;
    QLabel *label;
    QPushButton *connects;
    QPushButton *disconnects;
    QPushButton *closes;
    unsigned char rgb_buffer[IMG_WIDTH*IMG_HEIGTH*3];
    QImage *frame;
    int id; //timer ID
    bool delay_time;


private slots:
    void newConnect();
    void readMessage();
    void displayError(QAbstractSocket::SocketError);

    void on_pushButton_link_clicked();

    void on_pushButton_unlink_clicked();
    void on_pushButton_closes_clicked();

protected:
    void timerEvent(QTimerEvent *);

};

#endif // CLIENT_H
