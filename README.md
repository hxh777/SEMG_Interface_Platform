# SEMG_Interface_Platform
便携式肌电信号交互采集及云端平台，使用者移动端（平板、手机）操作，数据从采集器流入移动端动态显示，可进行边缘计算，上传云端。


硬件 ：

肌电信号放大器（自研）：放大semg信号，数字量输出

ble蓝牙5.0模块：便携无线无网快速传输

安卓9.0联想平板（手机也可）：交互软件，动态显示SEMG信号，可边缘计算和云端服务器上传数据。

stm32f103rdt6：DMA连续扫描adc采集信号、蓝牙发送信号、状态机按键去抖和oled状态显示。

软件：

keil，QT5.12.3，
kit组件
1.Android for Arm64-v8a
2.Desktop Qt 5.12.3 MinGW 64-bit

Android NDK ：android-ndk-r19c-windows-x86_64

Androif SDK ：installer_r24.4.1-windows

Android NDK版本和Android SDK版本必须匹配

本代码为整个项目部分实验代码。
实现移动端通过蓝牙获取sEMG便携采集器信号，图片展示了单通道数据传输，最高速度可达到15kb/s，代码为三通道数据传输

SEMG_Collector 为便携式嵌入端部分代码

sEMG_display 为安卓移动端交互软件部分代码
