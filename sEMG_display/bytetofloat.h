#ifndef BYTETOFLOAT_H
#define BYTETOFLOAT_H

#include <QObject>
#include <QVector>
class BytetoFloat : public QObject
{
    Q_OBJECT
public:
    explicit BytetoFloat(QObject *parent = nullptr);



signals:
    void sendArray(const float num);
public slots:
    void working(const QByteArray &value);
};

#endif // BYTETOFLOAT_H
