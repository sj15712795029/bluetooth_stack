这是STM32H750的蓝牙工程



BSP完成进度

| 标题              | 完成度 | 备注                                                         |
| ----------------- | ------ | ------------------------------------------------------------ |
| UART 1 debug      | 100%   |                                                              |
| UART1 DMA接收数据 | 90%    | 需要判断数据格式是否为Json,牵扯到所有的STM32的工程           |
| Systick           | 100%   | 因为我们设置了systick为100hz，所以systick delay的误差会有10ms |
| LED               | 100%   |                                                              |
| BUTTON            | 100%   |                                                              |
| EEPROM            |        |                                                              |
| Reset蓝牙pin      |        |                                                              |
| 驱动起来蓝牙      |        |                                                              |
| WIFI              |        |                                                              |
| Codec IC驱动起来  |        |                                                              |
| SPI LCD驱动起来   |        |                                                              |
| LVGL移植          |        |                                                              |
| USB FS            |        |                                                              |
| IR                |        |                                                              |

