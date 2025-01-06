# 蓝牙协议栈 Bluetooth stack                             [（English）](https://github.com/sj15712795029/bluetooth_stack/blob/master/README_English.md)
这是一个包含传统蓝牙跟低功耗蓝牙的协议栈，属于超轻量级蓝牙协议栈，用作学习蓝牙协议栈的人使用
<br>

|     |  CSR8311 | CYW54591 | BCM43430A<br />(AP6212A) | BCM4345C5<br />(AP6256) | ESP32 |
|  ----  | ----  | ---- | ---- |  ----  |  ----  |
| STM32F103  | √ | × | × | × | x |
| STM32F407  | √ | × | × | × | x |
| STM32F412  | √ | × | × | × | x |
| STM32H750 | √ | × | × | × | x |
| Linux | √ | √ | √ | √ | √ |
| Windows | × | × | × | × | x |
| 购买连接： | [**购买开发板连接(点击我)**](https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22329603896.18.5aeb41f9OvIVgA&id=622836061708) | 暂未上线 | 暂未上线 | 暂未上线 | [**购买开发板连接(点击我)**](https://item.taobao.com/item.htm?id=672388903856&pisk=gBsjfyVNPjcjhWclnSeyOVKJPVx_U1ZUlA9OKOnqBnKv6A1RBdp9XcKJ2pvj6Fdao1T1__YO0nWVNxj5Oco4irXtfh-TTWrUYt2DjhFU2R_gNjvDdqEwHE3JmdPisrvuYtXDshFUTkrE15BiBV3961KReppHXdd968tJIpttDFnxybdkBhK9kq382ppE6fL9B8gJpdox6c3AeQpDdVK9X1d8Fdc_4Y9TlIXbhxocZjipptdSXcIYjEOCbVosf9vvlGBDNa7PVKTX9U8TURjAaOIVmU4t5hXPR1_O97u2Mw9CNe7bOm1CidB6BsZndQIfCgTczvoCFT6O8UszpkxXDIQwmOkQgTsfgtj5QA3Jcn7WRitLxjdNz9s6CGVgV66OKNL5fjszADRCxz3sFem6FBy7FV0MUFvqTeHY4bYvEKaUF8GASEpkFBy7FjQMkLvba8wSMNf..&spm=a21xtw.29178619.product_shelf.7.475441f9RoWgEV) |



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
| project     | stm32f10x_bb_bt                 | STM32F103裸机(BB:bare board)蓝牙工程                   |
|             | stm32f407_bb_bt                 | STM32F407裸机(BB:bare board)蓝牙工程                   |
|             | stm32f412_bb_bt                 | STM32F412裸机(BB:bare board)蓝牙工程                   |
|             | stm32h750_bb_bt                 | STM32H750裸机(BB:bare board)蓝牙工程                   |
|             | linux_bt                        | Linux 蓝牙工程                                         |
|             | windows_bt                      | Windows蓝牙工程                                        |
| python_tool | convert_binayr_file_to_array.py | 把二进制文档转换为数据的python工具                     |
|             | corvert_opcode_to_ogf_ocf.py    | 把HCI opcode转换为OGF,OCF                              |





## 工程使用方式

### 通用

在bt_config.h中配置你想要的功能，比如debug/profile enable/paring mode/chipset select/spec profile config...<br>

### STM32下使用

前提条件：1)Keil安装  2)STM32F1/STM32F4对应的pack包  3)Download工具(我们选择的是ST-LINK)<br>

打开工程project下的对应的stm32f10x_bb_bt/stm32f10x_bb_bt/stm32f412_bb_bt直接编译下载即可<br>

### Linux下使用

前提条件:<br>

1)安装alsa工具:sudo apt install alsa

1)USB转UART工具(H4 transport需要硬件流控)并且接线正确，根据丝印TX接TX,RX接RX,CTS接CTS,RTS接RTS,VCC5V,GND<br>

2)一般需要给串口权限，比如sudo chmod 777 /dev/ttyUSB0(如果不是这个路径要修改code)<br>

打开工程project下的linux_bt make一下，生成bt_demo后直接运行即可<br>

## 交流社区：
[**1. CSDN技术文章专栏（点击我）**](https://blog.csdn.net/xiaoxiaopengbo/category_10215137.html)

[**2. 论坛（点击我）**](http://efunbbs.cn/)


## 视频教程：
[**蓝牙协议栈教程（点击我）**](https://item.taobao.com/item.htm?id=693788592796&pisk=g3OoXWYQOLWSO_xT2n5SCaQr-1MYNz1CxHFdv6IE3iSbwHCR9tWVRa7PzMyzuKY6xg3IF_pcxn-c4HYIPwSeYHjpexh9PU1CTcCn6fLSbGARaHXPYs82lNeP8xBPjrpYIcnt6XDzu6moX3EReiyVSN5Fz6Pz0qbh7W5FUT8qow7L4JPyYEuccw4zL8Pz3r75JJ5F88S40a_zUzWUajyVcw5FYH5EssVPbBRXgmjCILUDO8dlrTjwzg8JyIolF8T5mfh6iKXcbcI0TWRcrE6l6MPipGJ12OIy0jFltE7HmOpoj7xH8F9FnBqryh-2KH7XhD2hjp8JCIBY8VSwNER1gthZSERX2UODJylFkhI2PQ5n_V1lag5clBnbaMv2LQB5OuohAIY2apjzU-yavlPCu2d4dJ6PlZjtFVZPd22CLg3moR1CUZ_-Xq00dJ6Plantoq26VT7fycC..&spm=a21xtw.29178619.product_shelf.9.475441f9RoWgEV&sku_properties=14829532%3A72110507)

## 平台介绍：
**目前我们的蓝牙协议栈可以在STM32F103以及在Linux跑起来,蓝牙芯片是CSR8311,支持<br>PROTOCOL：HCI L2CAP RFCOMM SDP AVDTP AVCTP ATT SMP<br>PROFILE: HFP A2DP AVRCP PBAP SPP GATT<br>
希望会有更多的人来加入进来一起协作开发profile以及蓝牙芯片的工作。**


## 蓝牙协议栈架构：
![](https://img-blog.csdnimg.cn/20200720164649531.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1hpYW9YaWFvUGVuZ0Jv,size_16,color_FFFFFF,t_70)

