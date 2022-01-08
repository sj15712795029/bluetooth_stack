# 蓝牙协议栈进度



## Common TODO

| 序号 | 标题                                             | 归属 | 进度 | 解决方案 |
| ---- | :----------------------------------------------- | ---- | ---- | -------- |
|      | 底层rx需要处理，避免多层数据copy                 |      | 0%   |          |
|      | 底层tx需要处理，避免多层数据copy                 |      | 0%   |          |
|      | 增加OS macro定义，支持多线程                     |      | 0%   |          |
|      | UART 架构调整（文档整理 概念/平台/常见问题）     |      |      |          |
|      | API支持多线程                                    |      |      |          |
|      | bt disable功能                                   |      |      |          |
|      | 增加BT inquiry扫描到EIR的UUID                    |      |      |          |
|      | BT wrapper的返回值                               |      |      |          |
|      | 增加bond设备的保存HAL接口                        |      |      |          |
|      | 把pincode request拉到上层处理_hci_pin_req_handle |      |      |          |
|      | 把SSP拉到上层处理_hci_usr_confim_req_evt_process |      |      |          |

## HFP HF TODO

| 序号 | 标题                          | 归属 | 进度 | 解决方案 |
| ---- | ----------------------------- | ---- | ---- | -------- |
|      | SDP版本                       |      | 100% |          |
|      | HF的BRSF的feature             |      | 0%   |          |
|      | HFP BATT indicator的增加/验证 |      | 100% |          |
|      | 主动连接                      |      |      |          |
|      | 主动断开                      |      |      |          |
|      | HFP多连接                     |      |      |          |



## GATT 功能表实现表

| 标题                                | Sub Feature                                         | GATTC API | GATTC Callback | GATTS API | GATTS Callcak | 备注 |
| ----------------------------------- | --------------------------------------------------- | --------- | -------------- | --------- | ------------- | ---- |
| 版本                                | GATTC: core 5.3<br />GATTS: core 5.3                | /         | /              | /         | /             |      |
| GATTS Register                      |                                                     | /         | /              | 50%       | /             |      |
| GATT Connect                        |                                                     | 0%        | 100%           | 0%        | 100%          |      |
| GATT Disconnect                     |                                                     | 0%        | 100%           | 0%        | 100%          |      |
| SERVER CONFIGURATION                | Exchange MTU                                        | 100%      | 100%           | 100%      | 100%          |      |
| PRIMARY SERVICE DISCOVERY           | Discover All Primary Services                       | 100%      | 100%           | 100%      | 100%          |      |
|                                     | Discover Primary Service by Service UUID            | 100%      | 100%           | /         | 0%            |      |
| RELATIONSHIP DISCOVERY              | Find Included Services                              | 100%      | 0%             | 0%        | 0%            |      |
| CHARACTERISTIC DISCOVERY            | Discover All Characteristics of a Service           | 100%      | 100%           | 0%        | 0%            |      |
|                                     | Discover Characteristics by UUID                    | 0%        | 0%             | 0%        | 0%            |      |
| CHARACTERISTIC DESCRIPTOR DISCOVERY | Discover All Characteristic Descriptors             |           |                |           |               |      |
| CHARACTERISTIC VALUE READ           | Read Characteristic Value                           |           |                |           |               |      |
|                                     | Read Using Characteristic UUID                      |           |                |           |               |      |
|                                     | Read Long Characteristic Values                     |           |                |           |               |      |
|                                     | Read Multiple Characteristic Values                 |           |                |           |               |      |
|                                     | Read Multiple Variable Length Characteristic Values |           |                |           |               |      |
| CHARACTERISTIC VALUE WRITE          | Write Without Response                              |           |                |           |               |      |
|                                     | Signed Write Without Response                       |           |                |           |               |      |
|                                     | Write Characteristic Value                          |           |                |           |               |      |
|                                     | Write Long Characteristic Values                    |           |                |           |               |      |
|                                     | Reliable Writes                                     |           |                |           |               |      |
| CHARACTERISTIC VALUE NOTIFICATION   | Notifications                                       |           |                |           |               |      |
|                                     | Multiple Variable Length Notifications              |           |                |           |               |      |
| CHARACTERISTIC VALUE INDICATIONS    | Indications                                         |           |                |           |               |      |
| CHARACTERISTIC DESCRIPTORS          | Read Characteristic Descriptors                     |           |                |           |               |      |
|                                     | Read Long Characteristic Descriptors                |           |                |           |               |      |
|                                     | Write Characteristic Descriptors                    |           |                |           |               |      |
|                                     | Write Long Characteristic Descriptors               |           |                |           |               |      |



## SMP TODO

| 标题               | Sub Feature                                                  | SMP API | SMP Callback | 备注 |
| ------------------ | ------------------------------------------------------------ | ------- | ------------ | ---- |
| 版本               | SMP core 5.3                                                 | /       | /            |      |
| SMP legecy pairing | SMP交互地址不能写死                                          |         |              |      |
|                    | 写代码的时候补充excel算法的数据类型，比如random是16byte的值  |         |              |      |
|                    | 使用的HCI command要根据是否支持来走HCI还是软件算法(public key/dhkey) |         |              |      |
|                    | Initiator just work                                          |         |              |      |
|                    | Initiator passkey entry                                      |         |              |      |
|                    | Initiator passkey oob                                        |         |              |      |
|                    | response passkey oob                                         |         |              |      |
|                    |                                                              |         |              |      |
|                    |                                                              |         |              |      |
|                    |                                                              |         |              |      |
|                    |                                                              |         |              |      |
|                    |                                                              |         |              |      |