#include "mytcp.h"
#include <QDebug>
MyTCP::MyTCP(QObject *parent) : QObject(parent)
{
    socket = new QTcpSocket();

    //连接信号槽
    QObject::connect(socket, &QTcpSocket::readyRead, [=](){


        QByteArray buffer;
        //读取缓冲区数据
        buffer = socket->readAll();
        if(!buffer.isEmpty())
        {
//            QString str = ui->textEdit_Recv->toPlainText();
//            //str+=tr(buffer);
//            str+=QString::fromUtf8(buffer);
//            //刷新显示
//            ui->textEdit_Recv->setText(str);
            qDebug()<< "tcp接收:";
            qDebug()<<QString::fromUtf8(buffer)<<endl;
        }

    });
    QObject::connect(socket, &QTcpSocket::disconnected,[=](){



        qDebug() << "Disconnected!";
    } );
}

MyTCP::~MyTCP()
{
    delete this->socket;
}

void MyTCP::tcp_write(const QByteArray &data)
{
    qDebug()<<"tcp发送"<<endl;
    socket->write(data);
    socket->flush();
}

void MyTCP::tcp_disconnected()
{

        socket->disconnectFromHost();
        emit tcp_failed();

}

void MyTCP::open_findTCP()
{
    QString IP;
    quint16 port;

    //获取IP地址
    IP = "47.95.253.244";
    //获取端口号
    port = 6666;
    //取消已有的连接
    socket->abort();
    //连接服务器
    socket->connectToHost(IP, port);

    //等待连接成功
    if(!socket->waitForConnected(30000))
    {
        qDebug() << "Connection failed!";
        return;
    }
    emit tcp_finshed();
    qDebug() << "Connect successfully!";
}
