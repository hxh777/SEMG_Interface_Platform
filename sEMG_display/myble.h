#ifndef MYBLE_H
#define MYBLE_H

#include <QObject>
#include <QBluetoothSocket>
#include <QtBluetooth/qtbluetoothglobal.h>
#include <QtBluetooth/qbluetoothlocaldevice.h>
#include <qbluetoothaddress.h>
#include <qbluetoothdevicediscoveryagent.h>
#include <qbluetoothlocaldevice.h>
#include <QTextCodec>
#include <QTimer>//定时器
#include <QTime>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothLocalDevice>
#include <QBluetoothSocket>
#include <QBluetoothUuid>
#include <QLowEnergyController>
#include <QLowEnergyService>

class MyBLE : public QObject
{
    Q_OBJECT
public:
    explicit MyBLE(QObject *parent = nullptr);
    ~MyBLE();

    void open_findBLE();
    void connectBLE();
    void addBlueToothDevicesToList(const QBluetoothDeviceInfo &info);
    void searchCharacteristic();

    void on_comboBox_UUID_currentIndexChanged(const QString &arg1); //选中的UUID服务

    void stopBLE(const QString ss);

    void ble_write(const QByteArray &newValue);
    void blueStates();
    void cleararrdata();
private:
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;//用来对周围蓝牙进行搜寻
    QBluetoothLocalDevice *localDevice;//对本地设备进行操作，比如进行设备的打开，设备的关闭等等
    QBluetoothSocket *socket1;//就是用来进行蓝牙配对链接和数据传输的


    QLowEnergyController *m_control;
    QLowEnergyService *m_service;
    QBluetoothDeviceInfo remoteDevice;
    QLowEnergyCharacteristic m_readCharacteristic;
    QLowEnergyCharacteristic m_writeCharacteristic[5];
    QLowEnergyService::WriteMode m_writeMode;
    QLowEnergyDescriptor m_notificationDesc;

    int SendMaxMode;
    int SendModeSelect;
    QTimer *timer;
    QByteArray arr;
    QByteArray newarr;
signals:
    void activateBLE();
    void BLE_NOOPEN();
    void ble_finshed();
    void read_signal(const QByteArray &value);
public slots:

    void BlueDeviceDisconnected();//当控制器从远程低功耗设备断开时发出此信号。

    void BlueServiceScanDone();   //正在运行的服务发现完成时发出此信号。

    void BlueServiceDiscovered(const QBluetoothUuid &gatt);


    void BlueDeviceConnected();   //当控制器成功连接到远程Low Energy设备时，会发出此信号。


    void BleServiceServiceStateChanged(QLowEnergyService::ServiceState s);//服务状态改变时发出此信号
    void BleServiceCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value); //特性值由事件改变时发出此信号在外设上
    void BleServiceCharacteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value);   //当特征读取请求成功返回其值时
    void BleServiceCharacteristicWrite(const QLowEnergyCharacteristic &c, const QByteArray &value);//当特性值成功更改为newValue时

};

#endif // MYBLE_H
