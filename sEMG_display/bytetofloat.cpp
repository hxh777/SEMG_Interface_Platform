#include "bytetofloat.h"
#include <QDebug>
#include <QThread>
BytetoFloat::BytetoFloat(QObject *parent) : QObject(parent)
{

}


void BytetoFloat::working(const QByteArray &value)
{

    //qDebug() << "线程地址: " << QThread::currentThread();


    int tmp = value.size() >> 2;

    //qDebug()<<"16进制:"+QString(value.toHex())<<endl;
    float outdata[tmp];
    memcpy(outdata,value.data(),value.size());
    for(int i = 0;i<tmp;i++){
        //qDebug()<<outdata[i]<<endl;
        emit sendArray(outdata[i]);
    }
}
