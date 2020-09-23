#define CONSOLE_DEBUG
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using Newtonsoft.Json;
using System.Text.RegularExpressions;
using System.Runtime.InteropServices;


namespace mcu_bt_tool
{
    public partial class Form1 : Form
    {
        /* 用于串口插拔检测 */
        public const int WM_DEVICE_CHANGE = 0x219;
        public const int DBT_DEVICEARRIVAL = 0x8000;
        public const int DBT_DEVICE_REMOVE_COMPLETE = 0x8004;

        /* JSON字符串解析 */
        const int PRINTFING_LOG = 0;
        const int PARSEING_JSON = 1;
        int serial_recv_status = PRINTFING_LOG;
        string recv_json_str;


        /* JSON command以及response的响应 */
        string json_bt_cmd_func = "BT";
        string json_bt_cmd_bt_on = "BT_START";
        string json_bt_cmd_bt_off = "BT_STOP";
        string json_bt_cmd_bt_start_inquiry = "BT_INQUIRY";
        string json_bt_cmd_bt_stop_inquiry = "BT_CANCEL_INQUIRY";

        public Form1()
        {
            InitializeComponent();
            System.Windows.Forms.Control.CheckForIllegalCrossThreadCalls = false;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            search_add_serial_port();
            ui_init();
        }

        /* 开启串口button的响应 */
        private void b_serial_open_Click(object sender, EventArgs e)
        {
            if (!serialPort1.IsOpen)//串口处于关闭状态
            {
                try
                {
                    if (cb_serial_port.SelectedIndex == -1)
                    {
                        MessageBox.Show("Error: 无效的端口,请重新选择", "Error");
                        return;
                    }
                    string strSerialName = cb_serial_port.SelectedItem.ToString();

                    serialPort1.PortName = strSerialName;//串口号
                    serialPort1.BaudRate = Convert.ToInt32(cb_serial_baudrate.SelectedItem.ToString());//波特率
                    serialPort1.DataBits = 8;//数据位
                    serialPort1.StopBits = StopBits.One;
                    serialPort1.Parity = Parity.None;

                    //打开串口
                    serialPort1.Open();
                    b_serial_open.Enabled = false;
                    b_serial_close.Enabled = true;

                    ui_bt_switch_show(true, false);

                }
                catch (System.Exception ex)
                {
                    MessageBox.Show("Error:" + ex.Message, "Error");
                    return;
                }
            }
        }

        /* 关闭串口button的响应 */
        private void b_serial_close_Click(object sender, EventArgs e)
        {
            if (serialPort1.IsOpen)//串口处于关闭状态
            {
                serialPort1.Close();//关闭串口

                b_serial_open.Enabled = true;
                b_serial_close.Enabled = false;

                ui_bt_switch_show(false, false);
            }
        }

        /* 清空串口调试区log button的响应 */
        private void b_log_clear_Click(object sender, EventArgs e)
        {
            t_data_recv.Text = "";
        }

        /* 串口发送button的响应 */
        private void b_serial_send_Click(object sender, EventArgs e)
        {
            if (t_data_send.Text == "")
                MessageBox.Show("发送内容为空", "错误提示");
            else
            {
                if (serialPort1.IsOpen)
                    serialPort1.Write(t_data_send.Text);
                else
                    MessageBox.Show("先打开串口", "错误提示");
            }
        }

        /* 购买button的响应 */
        private void b_board_buy_Click(object sender, EventArgs e)
        {
            System.Diagnostics.Process.Start("https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-22329603896.18.5aeb41f90K806g&id=622836061708");
        }

        private void b_bt_start_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_bt_on, null, null, null, null, null, null);
            ui_bt_switch_show(false, true);
        }

        private void b_bt_stop_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_bt_off, null, null, null, null, null, null);
            ui_bt_switch_show(true, false);
        }

        private void b_start_inquiry_Click(object sender, EventArgs e)
        {
            dg_inquiry_result.Rows.Clear();
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_bt_start_inquiry, null, null, null, null, null, null);
        }

        private void b_stop_inquiry_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_bt_stop_inquiry, null, null, null, null, null, null);
        }


        /* 串口搜索 */
        private void search_add_serial_port()
        {
            cb_serial_baudrate.SelectedIndex = 0;
            /* 更新串口按键状态 */

            /*------串口界面参数设置------*/
            int has_port = 0;
            //检查是否含有串口
            string[] str = SerialPort.GetPortNames();

            /* 添加串口 */
            foreach (string s in str)
            {
                has_port++;

                if (!cb_serial_port.Items.Contains(s))
                    cb_serial_port.Items.Add(s);
            }

            if (has_port == 0)
                return;
            //设置默认串口选项
            cb_serial_port.SelectedIndex = 0;
        }

        /* 串口插拔检测 */
        protected override void WndProc(ref Message m)
        {
            if (m.Msg == WM_DEVICE_CHANGE)        // 捕获USB设备的拔出消息WM_DEVICECHANGE
            {
                switch (m.WParam.ToInt32())
                {
                    case DBT_DEVICE_REMOVE_COMPLETE:    // USB拔出    
                    {
                        b_serial_open.Enabled = true;
                        b_serial_close.Enabled = false;

                        ui_bt_switch_show(false, false);
                    }
                    break;
                    case DBT_DEVICEARRIVAL:             // USB插入获取对应串口名称      
                    {
                        search_add_serial_port();
                    }
                    break;
                }
            }
            base.WndProc(ref m);

        }

        /* 串口收到数据 */
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (serialPort1.IsOpen)
            {
                try
                {
                    byte[] read_data = new byte[serialPort1.BytesToRead];
                    serialPort1.Read(read_data, 0, read_data.Length);
                    string recv_data_string = Encoding.GetEncoding("UTF-8").GetString(read_data);

                    string[] arr = Regex.Split(recv_data_string, "\r\n");

                    foreach (string s in arr)
                    {
                        if (s.Contains("{") || (serial_recv_status == PARSEING_JSON))
                        {
                            serial_recv_status = PARSEING_JSON;
                            recv_json_str += s;
                            if (s.Contains("}"))
                            {
                                serial_recv_status = PRINTFING_LOG;
#if  CONSOLE_DEBUG
                                Console.WriteLine(recv_json_str);
#endif

                                json_response status = JsonConvert.DeserializeObject<json_response>(recv_json_str);

                                json_status_recv_parse(status);
                                recv_json_str = "";
                                //return;
                            }

                        }
                        else
                        {
                            if(s != "")
                                t_data_recv.Text += (s + "\r\n");
                        }

                    }

                }
                catch (Exception ex)
                {
                    return;
                }
            }
        }

        /* 蓝牙json命令发送给串口 */
        private void json_cmd_send(string func, string operate, string param1, string param2, string param3, string param4, string param5, string param6)
        {
            json_commmand cmd = new json_commmand();
            cmd.FUNC = func;
            cmd.OPERATE = operate;
            cmd.PARAM1 = param1;
            cmd.PARAM2 = param2;
            cmd.PARAM3 = param3;
            cmd.PARAM4 = param4;
            cmd.PARAM5 = param5;
            cmd.PARAM6 = param6;
            string json_cmd = JsonConvert.SerializeObject(cmd);
#if  CONSOLE_DEBUG
            Console.WriteLine(json_cmd);
#endif
            if (serialPort1.IsOpen)
            {
                serialPort1.WriteLine(json_cmd);
            }
        }

        private void json_status_recv_parse(json_response status)
        {
#if  CONSOLE_DEBUG
            Console.WriteLine("----------json_status_recv_parse-------------");
            Console.WriteLine("json func:" + status.FUNC);
            Console.WriteLine("json operate:" + status.OPERATE);
            Console.WriteLine("json status:" + status.STATUS);
            Console.WriteLine("json param1:" + status.PARAM1);
            Console.WriteLine("json param2:" + status.PARAM2);
            Console.WriteLine("json param3:" + status.PARAM3);
            Console.WriteLine("json param4:" + status.PARAM4);
            Console.WriteLine("json param5:" + status.PARAM5);
            Console.WriteLine("----------json_status_recv_parse  end--------");
#endif
            if (status.FUNC == "BT")
            {
                if (status.OPERATE == "BT_START")
                {
                    if (status.STATUS == "SUCCESS")
                    {
                        /* TODO:profile mask的处理 */
                        ui_bt_inquiry_show(true, false);
                    }
                }

                if (status.OPERATE == "BT_INQUIRY_STATUS")
                {
                    if (status.PARAM1 == "START")
                    {
                        ui_bt_inquiry_show(false, true);
                    }

                    if (status.PARAM1 == "STOP")
                    {
                        ui_bt_inquiry_show(true, false);
                    }
                }

                if (status.OPERATE == "BT_INQUIRY_RESULT")
                {
                    int index = dg_inquiry_result.Rows.Add();
                    dg_inquiry_result.Rows[index].Cells[1].Value = status.PARAM1;
                    dg_inquiry_result.Rows[index].Cells[2].Value = status.PARAM3;

                    if (status.PARAM2 == "HEADSET")
                    {
                        dg_inquiry_result.Rows[index].Cells[0].Value = Properties.Resources.ResourceManager.GetObject("音频");
                    }
                    else
                    {
                        dg_inquiry_result.Rows[index].Cells[0].Value = Properties.Resources.ResourceManager.GetObject("未知");
                    }
                }
            }
        }

        /* 蓝牙开启/关闭button的显示使能 */
        private void ui_bt_switch_show(bool bt_start_show, bool bt_stop_show)
        {
            b_bt_start.Enabled = bt_start_show;
            b_bt_stop.Enabled = bt_stop_show;
        }

        /* 蓝牙搜索/取消搜索button的显示使能 */
        private void ui_bt_inquiry_show(bool bt_start_show, bool bt_stop_show)
        {
            b_start_inquiry.Enabled = bt_start_show;
            b_stop_inquiry.Enabled = bt_stop_show;
        }

        /* 整个UI的初始化 */
        private void ui_init()
        {
            ui_bt_switch_show(false,false);
        }

        
        
    }


    /* JSON类定义 */
    public class json_commmand
    {
        public string FUNC { get; set; }
        public string OPERATE { get; set; }
        public string PARAM1 { get; set; }
        public string PARAM2 { get; set; }
        public string PARAM3 { get; set; }
        public string PARAM4 { get; set; }
        public string PARAM5 { get; set; }
        public string PARAM6 { get; set; }
    }

    public class json_response
    {
        public string FUNC { get; set; }
        public string OPERATE { get; set; }
        public string STATUS { get; set; }
        public string PARAM1 { get; set; }
        public string PARAM2 { get; set; }
        public string PARAM3 { get; set; }
        public string PARAM4 { get; set; }
        public string PARAM5 { get; set; }
        public string PARAM6 { get; set; }
    }
}
