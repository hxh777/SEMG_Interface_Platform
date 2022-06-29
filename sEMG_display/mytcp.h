#ifndef MYTCP_H
#define MYTCP_H

#include <QObject>
#include <QTcpSocket>
class MyTCP : public QObject
{
    Q_OBJECT
public:
    explicit MyTCP(QObject *parent = nullptr);
    ~MyTCP();
    void tcp_write(const QByteArray &data);
    void tcp_disconnected();
    void open_findTCP();
private:

    QTcpSocket *socket;

signals:
    void tcp_finshed();
    void tcp_failed();

public slots:


};

#endif // MYTCP_H
