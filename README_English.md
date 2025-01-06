# Bluetooth stack introduce
This is a protocol stack that includes traditional Bluetooth and low-power Bluetooth. It belongs to the ultra-lightweight Bluetooth protocol stack and is used by people who learn the Bluetooth protocol stack<br>I have also built a development environment based on this part as an additional income. I hope you can give me your support.If there is insufficient support, I hope you can correct it。

这是一个包含传统蓝牙跟低功耗蓝牙的协议栈，属于超轻量级蓝牙协议栈，用作学习蓝牙协议栈的人使用
<br>

|               | CSR8311                                                      | CYW54591 | BCM43430A<br />(AP6212A) | BCM4345C5<br />(AP6256) | ESP32                                                        |
| ------------- | ------------------------------------------------------------ | -------- | ------------------------ | ----------------------- | ------------------------------------------------------------ |
| STM32F103     | √                                                            | ×        | ×                        | ×                       |                                                              |
| STM32F407     | √                                                            | ×        | ×                        | ×                       |                                                              |
| STM32F412     | √                                                            | ×        | ×                        | ×                       |                                                              |
| STM32H750     | √                                                            | ×        | ×                        | ×                       |                                                              |
| Linux         | √                                                            | √        | √                        | √                       |                                                              |
| Windows       | ×                                                            | ×        | ×                        | ×                       |                                                              |
| Purchase page | [**Buy (click on me)**](https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-22329603914.14.6d987dbeJT61MT&id=622837949775) | 暂未上线 | 暂未上线                 | 暂未上线                | [**Buy (click on me)**](https://item.taobao.com/item.htm?id=672388903856&pisk=gN0nfLc5PDrIBPMApAzBcWFeZrtOd6a7EYQ8ezeybRy1JYULylraNWPUT8SF_ccjEJdWAyngEAkgLYcWO7yrUY2-vhLxOXa7zKUkkEhB3-0L8vbzL5Rzi7jy6kJG6VIYzKpvrGCaApUrpeEL9GSaGRPUUw7eslPbgzPUYuRgQ7Vf49kzzCAgwWjPTwPP_1PuMzSP8uyN_WN3Uw7zTGRgNRyzzzlG_wyqzVgwI8Zmv12wZqV3x-lULXhK7EZCAfyGBOgijl22HRbPzVVn1k4YUZx83mwLDJkwWw4o_7DgDDvc-YcrG0rir9RK3AooU5i6Eg2m4XnjWlCyg-uI_qaaXERm_VMKOVcRaUPtHRlI8zY2Gy4ULPVsyd_UEcorJka5LL2SSfuEqgJPbiWR3aNwwVS5VkP_s-KBG9rWZgZzpCAGczZU1WpvsCj5VkP_9KdMsiMQY5NpH&spm=a21xtw.29178619.product_shelf.7.475441f9RoWgEV) |



## 源码目录结构

| 目录文件夹  | 子目录文件夹                    | 说明                                   |
| ----------- | ------------------------------- | -------------------------------------- |
| board       | stm32f10x                       | STM32F103 BSP                          |
|             | stm32f407                       | STM32F407 BSP                          |
|             | stm32f412                       | STM32F412 BSP                          |
| bsp         | cortex-m3                       | Cortex M3 Driver(The standard library) |
|             | cortex-m4                       | Cortex M4 Driver(The HAL library)      |
| component   | bluetooth                       | Bluetooth stack source code            |
|             | cjson                           | json source code                       |
|             | fs                              | file system source code                |
|             | memory_manager                  | memoey manager source code             |
|             | ringbuffer                      | Ring buffer source code                |
| mcu_bt_tool |                                 | PC app source code                     |
| project     | stm32f10x_bb_bt                 | STM32F103 bare board bluetooth project |
|             | stm32f407_bb_bt                 | STM32F407 bare board bluetooth project |
|             | stm32f412_bb_bt                 | STM32F412 bare board bluetooth project |
|             | stm32h750_bb_bt                 | STM32H750 bare board bluetooth project |
|             | linux_bt                        | Linux bluetooth project                |
|             | windows_bt                      | Windows bluetooth project              |
| python_tool | convert_binayr_file_to_array.py |                                        |
|             | corvert_opcode_to_ogf_ocf.py    |                                        |

## Use of source code

### General configuration

Configure the functionality you want in bt_config.h, for example:debug/profile enable/paring mode/chipset select/spec profile config...<br>

### How to use STM32

Prerequisites: 1)Keil installation 2) Pack corresponding to STM32F1/STM32F4 3)Download tool (we selected ST-Link)<br>

Open the corresponding STM32F10X_BB_BT/STM32F10X_BB_BT/STM32F412_BB_BT under the project to compile and download<br>

### How to use Linux

Prerequisites:<br>

1)USB to UART tool (H4 Transport requires hardware flow control) and wiring is correct, according to the screen printing TX to TX,RX to RX,CTS to CTS,RTS to RTS,VCC5V,GND<br>

2)Sudo chmod 777 /dev/ttyusb0 (if not the path you need to change the code)<br>

Make linux_bt under project, create bt_demo and run it directly<br>


## Bluetooth protocol stack architecture:
![](https://img-blog.csdnimg.cn/20200720164649531.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L1hpYW9YaWFvUGVuZ0Jv,size_16,color_FFFFFF,t_70)

