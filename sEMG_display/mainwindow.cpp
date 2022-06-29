#include "mainwindow.h"
#include "ui_mainwindow.h"

static double dyn_x = 1;
static uint8_t dyn_cnt = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ble = new MyBLE;
    tcp = new MyTCP;
    btf = new BytetoFloat;

    t1 = new QThread;
    t2 = new QThread;
    t3 = new QThread;

    btf->moveToThread(t3);
    ble->moveToThread(t1);
    tcp->moveToThread(t2);


    connect(ble,&MyBLE::BLE_NOOPEN,[=](){
        QMessageBox::information(this,tr("提示"),tr("请连接蓝牙..."));
    });
    connect(ble,&MyBLE::ble_finshed,[=](){
           connectOK();
           ui->pushButton_openBLE->setEnabled(false);   //打开按钮
    });


    connect(ble,SIGNAL(read_signal(const QByteArray)),btf,SLOT(working(const QByteArray )));
    connect(btf,SIGNAL(sendArray(const float)),this,SLOT(bledata_proc(const float )));
 /*********************************************************************/

    connect(tcp,&MyTCP::tcp_finshed,[=](){

         ui->pushButton_openTCP->setEnabled(false);   //打开按钮


    });

    connect(tcp,&MyTCP::tcp_failed,[=](){

         ui->pushButton_openTCP->setEnabled(true);

    });
/*******************************************************************/

    connect(this, &MainWindow::destroyed, this, [=]()
    {


        t1->quit();
        t1->wait();
        t1->deleteLater();  // delete t1;


        t2->quit();
        t2->wait();
        t2->deleteLater();

        t3->quit();
        t3->wait();
        t3->deleteLater();

        btf->deleteLater();
        tcp->deleteLater();
        ble->deleteLater();
        qDebug()<<"ddddddddddddd"<<endl;

    });


   /*静态最大存储 maxstatic + 1 个数据，暂时未写*/
   maxstatic = 500;


   /* Y 轴上的最小值 */
   minY = -1;
   /* Y 轴上的最大值 */
   maxY = 13;




   /* 设置为图表视图为中心部件 */
//   setCentralWidget(chartView);
//   QGridLayout *baseLayout = new QGridLayout(this); //便于显示，创建网格布局

//   baseLayout->addWidget(chartView, 1, 0);
//   ui->widget_show->setLayout(baseLayout); //显示到QWidget控件

//   /* 设置随机种子，随机数初始化 */
//   qsrand(time(NULL));
   customPlot = ui->customPlot;
    x.resize(1500);
    y.resize(1500);
    data.resize(20);


    QLinearGradient plotGradient;
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));



   customPlot->axisRect()->setBackground(plotGradient);//背景黑色
   customPlot->xAxis->grid()->setPen(QPen(QColor(180, 180, 180), 1, Qt::PenStyle::DashLine));//网格白色虚线
   customPlot->yAxis->grid()->setPen(QPen(QColor(180, 180, 180), 1, Qt::PenStyle::DashLine));//网格白色虚线


   customPlot->yAxis->ticker()->setTickCount(7);//11个主刻度

   //每条曲线都会独占一个graph()

   pGraph1_sta = customPlot->addGraph();
   pGraph1_sta->setPen(QPen(Qt::blue)); // 曲线的颜色
   pGraph1_sta->setBrush(QBrush(QColor(0, 0, 255, 20))); // 曲线与X轴包围区的颜色

   // 边框右侧和上侧均显示刻度线，但不显示刻度值:
   // (QCPAxisRect::setupFullAxesBox for a quicker method to do this)
   customPlot->xAxis2->setVisible(true);
   customPlot->xAxis2->setTickLabels(false);
   customPlot->yAxis2->setVisible(true);
   customPlot->yAxis2->setTickLabels(false);
   // 使上下两个X轴的范围总是相等，使左右两个Y轴的范围总是相等
   connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
   connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));


   timer_plot = new QTimer(this);
   // 初始化图表1
   QPlot_init(customPlot);

}

MainWindow::~MainWindow()
{
    delete ui;
}
//连接成功
void MainWindow::connectOK()
{
    QMessageBox::information(this, tr("成功"), tr("连接成功!"));
}
void MainWindow::on_pushButton_openBLE_clicked()
{

    t1->start();
    ble->open_findBLE();
    t3->start();

}

void MainWindow::on_pushButton_upDateBLE_clicked()
{
    QString s = "hello hello";
    ble->ble_write(s.toUtf8());
}

void MainWindow::on_pushButton_closeBLE_clicked()
{

    ble->stopBLE("关闭蓝牙");
    ui->pushButton_openBLE->setEnabled(true);   //打开按钮
}

void MainWindow::on_pushButton_senddateTCP_clicked()
{
    QString s = "ahxh0308";
    tcp->tcp_write(s.toUtf8());
}

void MainWindow::on_pushButton_closeTCP_clicked()
{
  tcp->tcp_disconnected();
}

void MainWindow::on_pushButton_openTCP_clicked()
{
    t2->start();
    tcp->open_findTCP();
}

void MainWindow::on_pushButton_clearall_clicked()
{
    data.clear();
    pGraph1_dyn->data().data()->clear();

    pGraph1_CH2->data().data()->clear();
    pGraph1_CH3->data().data()->clear();

    customPlot->graph(0)->data().data()->clear(); //ppGraph1_sta
    customPlot->replot(QCustomPlot::rpQueuedReplot);
    dyn_x = 0;
    dyn_cnt = 0;
    ble->cleararrdata();

    if(!timer_plot->isActive())
        ui->pushButton_static->setEnabled(true);
    ui->pushButton_dynamic->setEnabled(true);

}



void MainWindow::receivedData_dyn(const float value)
{
   /* 将数据添加到 data 中 */
   //data.append(value);
    Show_Plot(customPlot, value);

}

void MainWindow::receivedData_sta(const float value)
{
//   static int i = 1;
//   x[i] = i;
//   y[i] = value;
//   i++;
//   if(i == maxstatic){
////       timer_sta->stop();


//       // 把已存在的数据填充进graph的数据区
//       customPlot->graph(0)->setData(x, y);

//        //自动调整XY轴的范围，以便显示出graph(0)中所有的点
//       customPlot->graph(0)->rescaleAxes();
//       ui->customPlot->replot();
//       i = 1;
//       ui->pushButton_static->setEnabled(true);
//   }



}



void MainWindow::on_pushButton_dynamic_clicked()
{
   ui->pushButton_static->setEnabled(false);
   customPlot->yAxis->setRange(maxY,minY);
   static uint32_t count = 0;
   if(count%2 == 0)
   {
       //timer_dyn->start(10);
       timer_plot->start(20);
   }
   else {
       //timer_dyn->stop();
       timer_plot->stop();
   }
   count ++;
}

void MainWindow::on_pushButton_static_clicked()
{
   //静态存储功能未写完，但不影响动态功能
//   timer_plot->stop();
//   ui->pushButton_dynamic->setEnabled(false);
//   ui->pushButton_static->setEnabled(false);
//   timer_sta->start(1);
}

//动态存储需要的初始化
void MainWindow::QPlot_init(QCustomPlot *customPlot)
{

   // 创建定时器，用于定时生成曲线坐标点数据

//   timer->start(10);
   connect(timer_plot,SIGNAL(timeout()),this,SLOT(TimeData_Update()));

   // 图表添加两条曲线
   pGraph1_dyn = customPlot->addGraph();
   pGraph1_CH2 = customPlot->addGraph();
   pGraph1_CH3 = customPlot->addGraph();

   // 设置曲线颜色
    QPen pen1;
    pen1.setColor(Qt::red);
    pen1.setWidth(3);
    QPen pen2;
    pen2.setColor(Qt::cyan);
    pen2.setWidth(3);
    QPen pen3;
    pen3.setColor(Qt::yellow);
    pen3.setWidth(3);

    pGraph1_dyn->setPen(pen1);
    pGraph1_CH2->setPen(pen2);
    pGraph1_CH3->setPen(pen3);

   // 设置坐标轴名称
   customPlot->xAxis->setLabel("X");
   customPlot->yAxis->setLabel("Y");

   // 设置y坐标轴显示范围
   customPlot->yAxis->setRange(maxY,minY);

   // 显示图表的图例
   customPlot->legend->setVisible(false);
   // 添加曲线名称
   //pGraph1_1->setName("波形1");
   // 允许用户用鼠标拖动轴范围，用鼠标滚轮缩放，点击选择图形:
   //customPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);
}



// 定时器溢出处理槽函数。用来更新绘图。
void MainWindow::TimeData_Update(void)
{
    customPlot->replot(QCustomPlot::rpQueuedReplot);
    customPlot->xAxis->setRange((pGraph1_dyn->dataCount()>1000)?(pGraph1_dyn->dataCount()-1000):0, pGraph1_dyn->dataCount());
}


// 曲线更新绘图
void MainWindow::Show_Plot(QCustomPlot *customPlot, float num)
{

   data.append(num);
   dyn_x++;
   // 给曲线添加数据
   pGraph1_dyn->addData(dyn_x, num);


   // 设置x坐标轴显示范围，使其自适应缩放x轴，x轴最大显示1000个点
   //customPlot->xAxis->setRange((pGraph1_dyn->dataCount()>1000)?(pGraph1_dyn->dataCount()-1000):0, pGraph1_dyn->dataCount());
   // 更新绘图，这种方式在高填充下太浪费资源。有另一种方式rpQueuedReplot，可避免重复绘图。

   //customPlot->replot();
   //customPlot->replot(QCustomPlot::rpQueuedReplot);

   static QTime time(QTime::currentTime());
   double key = time.elapsed()/1000.0; // 开始到现在的时间，单位秒
   //计算帧数
   static double lastFpsKey;
   static int frameCount;
   ++frameCount;

   if (key-lastFpsKey > 1) // 每1秒求一次平均值
   {
       qDebug()<<QString("%1 FPS,Total Data Points: %2").arg(frameCount/(key-lastFpsKey),0,'f',0).arg(customPlot->graph(1)->data()->size()
                             , 0)<<endl;
       lastFpsKey = key;
       frameCount = 0;
   }


}

void MainWindow::bledata_proc(const float data){

    switch(dyn_cnt%3){
    case 0 :      //曲线1
        dyn_cnt++;
        pGraph1_dyn->addData(dyn_x, data);
        break;
    case 1 :      //曲线2
        dyn_cnt++;
        pGraph1_CH2->addData(dyn_x, data + 4);
        break;
    case 2 :        //曲线3
        dyn_cnt = 0;
        dyn_x++;
        pGraph1_CH3->addData(dyn_x, data + 8);
        break;
    default:
        qDebug()<<"data process error!"<<endl;

    }

}


