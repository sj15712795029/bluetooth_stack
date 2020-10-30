# 蓝牙协议栈 Bluetooth stack
这是一个包含传统蓝牙跟低功耗蓝牙的协议栈，属于超轻量级蓝牙协议栈，用作学习蓝牙协议栈的人使用
<br>我根据此部分也搭建了一个开发环境，来作为额外收入，希望大家多多支持。有不足支持也希望大家指正。

|     |  CSR8311 | .... | 开发板购买链接 |
|  ----  | ----  | ---- | ---- |
| STM32F103  | √ | x | [**购买开发板连接（点击我）**](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22329603896.18.5aeb41f9OvIVgA&id=622836061708) |
| STM32F407  | √ | x | x |
| STM32F412  | √ | x | [**购买开发板连接（点击我）**](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22329603896.30.5aeb41f9DVuET8&id=628752502770) |
|  | [**购买模组连接（点击我）**](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22329603896.18.5aeb41f9OvIVgA&id=622836061708) |  |  |
## 源码目录结构
| 目录文件夹  | 子目录文件夹                    | 说明                                                   |
| ----------- | ------------------------------- | ------------------------------------------------------ |
| board       | stm32f10x                       | STM32F103板载外设驱动                                  |
|             | stm32f407                       | STM32F407板载外设驱动                                  |
|             | stm32f412                       | STM32F412板载外设驱动                                  |
| bsp         | cortex-m3                       | Cortex M3 Driver库（M3默认用标准库）                   |
|             | cortex-m4                       | Cortex M4 Driver库（M3默认用HAL库）                    |
| component   | bluetooth                       | 蓝牙协议栈源码                                         |
|             | cjson                           | json源码（目前用于跟上位机交互）                       |
|             | fs                              | file system文件系统，目前用于管理spi flash             |
|             | memory_manager                  | 内存管理，目前用于管理STM32F407的SRAM                  |
|             | ringbuffer                      | 环形数组，用于蓝牙接收串口数据                         |
| mcu_bt_tool |                                 | PC上位机，用于控制STM32开发板，以及接受STM32的状态回传 |
| project     | stm32f10x_bb_csr8x11_bt         | STM32F103裸机蓝牙工程                                  |
|             | stm32f407_bb_csr8x11_bt         | STM32F407裸机蓝牙工程                                  |
|             | stm32f412_bb_csr8x11_bt         | STM32F412裸机蓝牙工程                                  |
|             | ubuntu_csr8x11_bt               | Linux ubuntu蓝牙工程                                   |
| python_tool | convert_binayr_file_to_array.py | 把二进制文档转换为数据的python工具                     |



## 技术文章专栏：
[**CSDN技术文章专栏（点击我）**](https://blog.csdn.net/xiaoxiaopengbo/category_10215137.html)


## 视频教程：
[**CSDN学院教程-蓝牙协议栈教程（点击我）**](https://edu.csdn.net/lecturer/5352?spm=1002.2001.3001.4144)

## 平台介绍：
**目前我们的蓝牙协议栈可以在STM32F103以及在Linux跑起来,蓝牙芯片是CSR8311,支持
<br>PROTOCOL：HCI L2CAP RFCOMM SDP
<br>PROFILE: HFP A2DP AVRCP PBAP SPP
希望会有更多的人来加入进来一起协作开发profile以及蓝牙芯片的工作。**


## 蓝牙协议栈架构：
![](https://img.alicdn.com/imgextra/i3/4013866915/O1CN01NSxsSk20x7PDPT25x_!!4013866915.png)


