#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "myble.h"
#include "mytcp.h"
#include "qcustomplot.h"
#include "BytetoFloat.h"
#include <QDebug>
#include <QThread>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>

#include <QPen>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    MyBLE *ble;
    QThread* t1;
    QThread *t2;
    QThread *t3;

    MyTCP *tcp;
    BytetoFloat *btf;
  /*QCustomPlot绘制初始化*/
  void QPlot_init(QCustomPlot *customPlot);
  void Show_Plot(QCustomPlot *customPlot, float num);
  /* 绘图控件的指针*/
  QCustomPlot *customPlot;

  /* 绘图控件中曲线的指针*/
  QCPGraph *pGraph1_dyn;
  QCPGraph *pGraph1_CH2;
  QCPGraph *pGraph1_CH3;
  QCPGraph *pGraph1_sta;


  QTimer *timer_plot;
 /* 接收数据接口 */
 void receivedData_dyn(const float);
 void receivedData_sta(const float);
 /* 两种模式数据最大个数 */
 int maxstatic;
 /* y 轴上的最大值 */
 int maxY;
 /* y 轴上的最小值 */
 int minY;



 /* QList int 类型容器 */
 QVector<float> data;
 QVector<double>x,y;

private slots:

     void connectOK();
     void on_pushButton_openBLE_clicked();
     void on_pushButton_upDateBLE_clicked();
     void on_pushButton_closeBLE_clicked();
     void on_pushButton_senddateTCP_clicked();
     void on_pushButton_closeTCP_clicked();
     void on_pushButton_openTCP_clicked();
     void timerTimeOut_dyn();
     void timerTimeOut_sta();
     void on_pushButton_dynamic_clicked();
     void on_pushButton_static_clicked();
     void on_pushButton_clearall_clicked();

public slots:
    void TimeData_Update(void);
    void bledata_proc(const float data);
};

#endif // MAINWINDOW_H
