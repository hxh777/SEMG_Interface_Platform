#include "myble.h"
#include <QDebug>

static const QLatin1String serviceUuid("{0000FFE0-0000-1000-8000-00805F9B34FB}");
MyBLE::MyBLE(QObject *parent) : QObject(parent)
{
    //蓝牙连接
    {
        discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
        timer = new QTimer();
        localDevice = new QBluetoothLocalDevice();
        //给socket分配内存，限定套接字协议  蓝牙2.0版本  ble蓝牙不用
        //socket1 = new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol);
        //发现设备时会触发deviceDiscovered信号，转到槽显示蓝牙设备
        connect(discoveryAgent,&QBluetoothDeviceDiscoveryAgent::deviceDiscovered,this,&MyBLE::addBlueToothDevicesToList);

        //蓝牙连接设备成功后会停止搜索设备，显示连接成功



        m_control=NULL; //初始值
        m_service=NULL;  //初始值
        SendModeSelect=0;
        SendMaxMode=0;

        /*. 检查蓝牙的状态，用于设置按钮的初始状态*/

        if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff)
        {
//                //如果蓝牙处于关闭状态
//                ui->pushButton_openBLE->setEnabled(true);   //打开按钮
//                ui->pushButton_closeBLE->setEnabled(false); //关闭按钮
                    localDevice->powerOn();//调用打开本地的蓝牙设备

        }
        else    //如果蓝牙处于开启状态
        {
//                ui->pushButton_openBLE->setEnabled(false);//打开按钮
//                ui->pushButton_closeBLE->setEnabled(true);//关闭按钮
//                ui->pushButton_upDateBLE->setEnabled(true); //设置扫描按钮可用
        }


    }
}

MyBLE::~MyBLE()
{
    delete discoveryAgent;
    delete localDevice;
}

void MyBLE::open_findBLE()
{
    if(localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff)
    {
        emit BLE_NOOPEN();
     }
    else
    {
        discoveryAgent->start();
    }
    qDebug()<<"成功打开蓝牙"<<endl;
}

void MyBLE::connectBLE()
{
    discoveryAgent->stop();  //停止搜索设备
    //QString text = item->text();
//  QString text = "48:70:1E:20:AE:55 ";
    QString text = "DC:4B:EA:7A:64:F3 ";
    qDebug()<<"连接的号码为"+text<<endl;
    int index = text.indexOf(' ');
    if (index == -1)
        return;
    QBluetoothAddress address(text.left(index));
    QString name(text.mid(index + 1));
    //QMessageBox::information(this,tr("提示"),tr("设备正在连接中..."));



    /*低功耗蓝牙设备*/

       if(m_control!=NULL)

       {

           m_control->disconnectFromDevice(); //断开远程设备

           delete m_control;

           m_control= NULL;

       }

      // ui->comboBox_UUID->clear();         //清除显示UUID服务的列表框

       QList<QBluetoothDeviceInfo> info_list=discoveryAgent->discoveredDevices(); //得到扫描的所有设备信息

       for(int i=0;i<info_list.count();i++)

       {

           if(info_list.at(i).address().toString()==text.left(index))

           {

                remoteDevice=info_list.at(i);

//                ui->textBrowser_receive->insertPlainText("连接设备:");

//          ui->textBrowser_receive->insertPlainText(remoteDevice.name());

//                ui->textBrowser_receive->insertPlainText("\n");      ui->textBrowser_receive->insertPlainText(remoteDevice.address().toString());

//                ui->textBrowser_receive->insertPlainText("\n");

               break;

           }

      }

       //创建中央角色设备

       m_control = new QLowEnergyController(remoteDevice, this);

       //m_control=QLowEnergyController::createCentral(remoteDevice,this);

       if(m_control==0)

       {

          // ui->textBrowser_receive->insertPlainText("创建中央角色设备失败!\n");

       }

       else

       {

          // ui->textBrowser_receive->insertPlainText("创建中央角色设备成功!\n");

       }

       //每次发现新的服务就会发送此信号

       connect(m_control, SIGNAL(serviceDiscovered(QBluetoothUuid)),this, SLOT(BlueServiceDiscovered(QBluetoothUuid)));

       //正在运行的服务发现完成时发出此信号。

       connect(m_control, SIGNAL(discoveryFinished()),this, SLOT(BlueServiceScanDone()));

       //当控制器成功连接到远程Low Energy设备时，会发出此信号。

        connect(m_control, SIGNAL(connected()),this, SLOT(BlueDeviceConnected()));

       //当控制器从远程低功耗设备断开时发出此信号。

        connect(m_control, SIGNAL(disconnected()),this, SLOT(BlueDeviceDisconnected()));

        //该信号在发生错误时发出。

        connect(m_control, static_cast<void(QLowEnergyController::*)(QLowEnergyController::Error)>(&QLowEnergyController::error),

              [=](QLowEnergyController::Error error){

            if(error==QLowEnergyController::NoError)

            {

               // ui->textBrowser_receive->insertPlainText("没有发生错误\n");

            }

            else if(error==QLowEnergyController::UnknownError)

            {

               // ui->textBrowser_receive->insertPlainText("出现未知错误。\n");

            }

             else if(error==QLowEnergyController::UnknownRemoteDeviceError)

            {

               // ui->textBrowser_receive->insertPlainText("无法找到传递给此类构造函数的远程Bluetooth Low Energy设备。\n");

            }

            else if(error==QLowEnergyController::NetworkError)

            {

               // ui->textBrowser_receive->insertPlainText("尝试读取或写入远程设备失败\n");

            }

             else if(error==QLowEnergyController::InvalidBluetoothAdapterError)

            {

               // ui->textBrowser_receive->insertPlainText("传递给此类构造函数的本地蓝牙设备无法找到，或者没有本地蓝牙设备\n");

            }

             else if(error==QLowEnergyController::InvalidBluetoothAdapterError)

            {

              //  ui->textBrowser_receive->insertPlainText("尝试连接到远程设备失败。\n");

            }

            else

           //  ui->textBrowser_receive->insertPlainText("*****未知错误!******\n");
                qDebug()<<"111111"<<endl;

        });

        //连接到远程蓝牙低功耗设备。

        m_control->connectToDevice();
}

void MyBLE::addBlueToothDevicesToList(const QBluetoothDeviceInfo &info)
{
    qDebug()<<"查找子项"<<endl;
    QString label = QString("%1 ").arg(info.address().toString());


    if (label == "DC:4B:EA:7A:64:F3 ")
    {
        connectBLE();
    }
}

void MyBLE::BlueServiceDiscovered(const QBluetoothUuid &gatt)
{
    qDebug()<<gatt.toString()<<endl;
}

void MyBLE::BlueServiceScanDone()
{
    // ui->plainTextEdit_BluetoothInfiShow->insertPlainText("正在运行的服务发现完成\n");

 //    QMessageBox::information(this,tr("帮助提示"),"服务发现完成\n"

 //                                             "请选择上方列表中的服务\n"

 //                                             "进行连接BLE低功耗蓝牙设备\n");

     /*判断之前有没有连接过*/

     if(m_service!=NULL)

     {

         delete m_service;

         m_service=NULL;

     }

//     ui->textBrowser_receive->insertPlainText("\n选中的服务:");

//     ui->textBrowser_receive->insertPlainText(serviceUuid);

//     ui->textBrowser_receive->insertPlainText("\n");

     /*与设备之间建立服务*/

     m_service=m_control->createServiceObject(QBluetoothUuid(serviceUuid),this);

     if(m_service==NULL)

     {

         //ui->textBrowser_receive->insertPlainText("服务建立失败!\n");

         return;

     }

     else

     {

        // ui->textBrowser_receive->insertPlainText("服务建立成功!\n");

     }

     /*服务状态改变时发出此信号。newState也可以通过state（）。*/

     connect(m_service, SIGNAL(stateChanged(QLowEnergyService::ServiceState)),

                 this, SLOT(BleServiceServiceStateChanged(QLowEnergyService::ServiceState)));

     /*特性值由事件改变时发出此信号在外设上。 newValue参数包含更新后的值特性*/

     connect(m_service, SIGNAL(characteristicChanged(QLowEnergyCharacteristic,QByteArray)),

                 this, SLOT(BleServiceCharacteristicChanged(QLowEnergyCharacteristic,QByteArray)));

     /*当特征读取请求成功返回其值时，发出此信号。*/

      connect(m_service, SIGNAL(characteristicRead(QLowEnergyCharacteristic,QByteArray)),

                 this, SLOT(BleServiceCharacteristicRead(QLowEnergyCharacteristic,QByteArray)));

     /*当特性值成功更改为newValue时，会发出此信号。*/

     connect(m_service, SIGNAL(characteristicWritten(QLowEnergyCharacteristic,QByteArray)),

                 this, SLOT(BleServiceCharacteristicWrite(QLowEnergyCharacteristic,QByteArray)));

     /*错误信号*/

     connect(m_service, static_cast<void(QLowEnergyService::*)(QLowEnergyService::ServiceError)>(&QLowEnergyService::error),

          [=](QLowEnergyService::ServiceError newErrorr)

     {

         if(QLowEnergyService::NoError == newErrorr)

         {

             // ui->textBrowser_receive->insertPlainText("没有发生错误。\n");

         }

         if(QLowEnergyService::OperationError==newErrorr)

         {

             // ui->textBrowser_receive->insertPlainText("错误: 当服务没有准备好时尝试进行操作!\n");

         }

         if(QLowEnergyService::CharacteristicReadError==newErrorr)

         {

             // ui->textBrowser_receive->insertPlainText("尝试读取特征值失败!\n");

         }

         if(QLowEnergyService::CharacteristicWriteError==newErrorr)

         {

             // ui->textBrowser_receive->insertPlainText("尝试为特性写入新值失败!\n");

         }

         if(QLowEnergyService::DescriptorReadError==newErrorr)

         {

             // ui->textBrowser_receive->insertPlainText("尝试读取描述符值失败!\n");

         }

         if(QLowEnergyService::DescriptorWriteError==newErrorr)

         {

            //  ui->textBrowser_receive->insertPlainText(" 尝试向描述符写入新值失败!\n");

         }

         if(QLowEnergyService::UnknownError==newErrorr)

         {

             // ui->textBrowser_receive->insertPlainText("与服务交互时发生未知错误!\n");

         }

     });

     if(m_service->state() == QLowEnergyService::DiscoveryRequired)

     {

         m_service->discoverDetails(); //启动服务发现扫描

     }

     else

         searchCharacteristic();
}

void MyBLE::searchCharacteristic()
{
    if(m_service)

    {

        QList<QLowEnergyCharacteristic> list=m_service->characteristics();

        qDebug()<<"list.count()="<<list.count();

        //characteristics 获取详细特性

        SendMaxMode=list.count();  //设置模式选择上限

        for(int i=0;i<list.count();i++)

        {

            QLowEnergyCharacteristic c=list.at(i);

            /*如果QLowEnergyCharacteristic对象有效，则返回true，否则返回false*/

            if(c.isValid())

            {

//                返回特征的属性。

//                这些属性定义了特征的访问权限。

               if(c.properties() & QLowEnergyCharacteristic::WriteNoResponse || c.properties() & QLowEnergyCharacteristic::Write)

               // if(c.properties() & QLowEnergyCharacteristic::Write)

                {

                    //ui->textBrowser_receive->insertPlainText("具有写权限!\n");

                    m_writeCharacteristic[i] = c;  //保存写权限特性

                    if(c.properties() & QLowEnergyCharacteristic::WriteNoResponse)

//                        如果使用此模式写入特性，则远程外设不应发送写入确认。

//                        无法确定操作的成功，并且有效负载不得超过20个字节。

//                        一个特性必须设置QLowEnergyCharacteristic :: WriteNoResponse属性来支持这种写模式。

//                         它的优点是更快的写入操作，因为它可能发生在其他设备交互之间。

                        m_writeMode = QLowEnergyService::WriteWithoutResponse;

                    else

                        m_writeMode = QLowEnergyService::WriteWithResponse;

                    //如果使用此模式写入特性，则外设应发送写入确认。

                    //如果操作成功，则通过characteristicWritten（）信号发出确认。

                    //否则，发出CharacteristicWriteError。

                    //一个特性必须设置QLowEnergyCharacteristic :: Write属性来支持这种写模式。

                }

                if(c.properties() & QLowEnergyCharacteristic::Read)

                {

                    m_readCharacteristic = c; //保存读权限特性

                }

                //描述符定义特征如何由特定客户端配置。

                m_notificationDesc = c.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);

                //值为真

                if(m_notificationDesc.isValid())

                {

                    //写描述符

                    m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("0100"));

                  //   m_service->writeDescriptor(m_notificationDesc, QByteArray::fromHex("FEE1"));

                    //ui->textBrowser_receive->insertPlainText("写描述符!\n");

                }

            }

        }

    }
}

void MyBLE::BlueDeviceConnected()
{
    //ui->textBrowser_receive->insertPlainText("成功连接设备!\n");
    qDebug()<<"成功连接设备！\n"<<endl;
    //启动发现服务Services
    emit ble_finshed();
    m_control->discoverServices();
}

void MyBLE::on_comboBox_UUID_currentIndexChanged(const QString &arg1)
{
    qDebug()<<"服务改变"<<endl;
}

void MyBLE::stopBLE(const QString ss)
{

    localDevice->setHostMode(QBluetoothLocalDevice::HostPoweredOff);
    qDebug()<<ss+"断开连接"<<endl;
}

void MyBLE::ble_write(const QByteArray &newValue)
{
    m_service->writeCharacteristic(m_writeCharacteristic[SendModeSelect],newValue, m_writeMode);
    //Qstring Qbytearray .toUTf8()
}

void MyBLE::BlueDeviceDisconnected()
{
    qDebug()<<"ble断开"<<endl;
}

void MyBLE::BleServiceServiceStateChanged(QLowEnergyService::ServiceState s)
{
    if(s == QLowEnergyService::ServiceDiscovered)  //所有细节都已同步

    {

        qDebug()<<"所有细节都已发现!\n"<<endl;

        searchCharacteristic();

    }
}

void MyBLE::BleServiceCharacteristicChanged(const QLowEnergyCharacteristic &c, const QByteArray &value)
{

            //qDebug()<<"接收到数值为16进制"+QString(value.toHex())<<endl;

            arr.append(value);
            if(arr.size() > 400)  //
            {
                qDebug()<<arr.size()<<endl;
                if(arr.size()%4 != 0)
                {
                    newarr.append(arr);
                    if(newarr.size()%4 == 0)
                    {
                        //qDebug()<<newarr.size()<<endl;
                        emit read_signal(newarr);
                        newarr.clear();
                    }
                }
                else{

                emit read_signal(arr);

                }
                arr.clear();
            }

}

void MyBLE::BleServiceCharacteristicRead(const QLowEnergyCharacteristic &c, const QByteArray &value)
{

}

void MyBLE::BleServiceCharacteristicWrite(const QLowEnergyCharacteristic &c, const QByteArray &value)
{
   qDebug()<<"发送"+QString(value)<<endl;
}

void MyBLE::cleararrdata()
{
    arr.clear();
    qDebug()<<"clear arr data "<<endl;
}
