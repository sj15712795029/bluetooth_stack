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
        float windows_x;  //默认窗口的宽度
        float windows_y;  //默认窗口的高度

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
        string json_bt_cmd_spp_send = "SPP_SEND";
        string json_bt_cmd_hfp_get_operate = "HFP_NET_N";
        string json_bt_cmd_hfp_audio_transfer = "BT_AUDIO_TRANSFER";
        string json_bt_cmd_hfp_answer_call = "HFP_ANSWER";
        string json_bt_cmd_hfp_end_call = "HFP_CALLEND";
        string json_bt_cmd_hfp_call_pn = "HFP_CALLOUT_PN"; /* 通过手机号拨打电话 */
        string json_bt_cmd_hfp_get_lpn = "HFP_LPN"; /* 获取本地号码 */
        string json_bt_cmd_hfp_get_call_pn = "HFP_CLCC"; /* 获取通话号码 */

        /* HFP的call/callsetup status */
        bool bt_hfp_is_call_active = false;
        int bt_hfp_call_time_count = 0;
        public const int HFP_CALL_NO_INPORCESS = 0;
        public const int HFP_CALL_INPORCESS = 1;

        public const int HFP_CALL_NO_CALL = 0;
        public const int HFP_CALL_INCOMING_CALL = 1;
        public const int HFP_CALL_OUTGOING_CALL = 2;
        public const int HFP_CALL_RALERT_OUTGOING_CALL = 3;

        public Form1()
        {
            InitializeComponent();
            System.Windows.Forms.Control.CheckForIllegalCrossThreadCalls = false;

            windows_x = this.Width;   //将窗体的宽度赋值给X
            windows_y = this.Height;   //将窗体的高度赋值给Y
            setTag(this);   //调用setTag函数

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            search_add_serial_port();
            ui_init();        
        }

        private void Form1_Resize(object sender, EventArgs e)
        {
            float newx = (this.Width) / windows_x;   //窗体宽度缩放比例

            float newy = (this.Height) / windows_y;   //窗体高度缩放比例

            setControls(newx, newy, this);   //调用setControls函数
        }

        private void setTag(Control cons)
        {

            foreach (Control con in cons.Controls)
            {
                con.Tag = con.Width + ":" + con.Height + ":" + con.Left + ":" + con.Top + ":" + con.Font.Size;
                if (con.Controls.Count > 0) setTag(con);   //如果此控件存在子控件，则此控件的子控件执行此函数一次
            }

        }

        private void setControls(float newx, float newy, Control cons)
        {

            foreach (Control con in cons.Controls)
            {
                if (con.Tag == null)
                    continue;
                string[] myTag = con.Tag.ToString().Split(':');     //将con的宽、高、左边距、顶边距离及字体大小通过字符“:”分割成数组
                float a = Convert.ToSingle(myTag[0]) * newx;    //根据窗口的缩放比例确定控件相应的值，宽度
                con.Width = (int)a;
                a = Convert.ToSingle(myTag[1]) * newy;    //高度
                con.Height = (int)a;
                a = Convert.ToSingle(myTag[2]) * newx;    //左边距
                con.Left = (int)a;
                a = Convert.ToSingle(myTag[3]) * newy;    //顶边距离
                con.Top = (int)a;
                Single currentSize = Convert.ToSingle(myTag[4]) * newy;     //字体大小
                con.Font = new Font(con.Font.Name, currentSize, con.Font.Style, con.Font.Unit);
                if (con.Controls.Count > 0) setControls(newx, newy, con);     //如果此控件存在子控件，则将相应子控件执行一次setControls函数

            }

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

        private void b_spp_clear_recv_data_Click(object sender, EventArgs e)
        {
            t_spp_recv_data.Text = "";
        }

        private void b_spp_clear_send_data_Click(object sender, EventArgs e)
        {
            t_spp_send_data.Text = "";
        }

        private void b_spp_clear_statistics_Click(object sender, EventArgs e)
        {
            l_spp_recv_count.Text = "0";
            l_spp_send_count.Text = "0";
        }

        private void b_spp_send_Click(object sender, EventArgs e)
        {
            if (t_spp_send_data.Text == "")
            {
                MessageBox.Show("请填入要发送的内容", "错误提示");
                return;
            }

            string strencode = "";
            byte[] utf8 = Encoding.UTF8.GetBytes(t_spp_send_data.Text);
            strencode = Encoding.UTF8.GetString(utf8);

            json_cmd_send(json_bt_cmd_func, json_bt_cmd_spp_send, strencode, strencode.Length.ToString(), null, null, null, null);

            int spp_send_count = Convert.ToInt32(l_spp_send_count.Text) + t_spp_send_data.Text.Length;
            l_spp_send_count.Text = spp_send_count.ToString();

        }

        /* 获取运营商名称 */
        private void b_hfp_get_operate_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_hfp_get_operate, null, null, null, null, null, null);
        }

        private void b_hfp_audio_transfer_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_hfp_audio_transfer, null, null, null, null, null, null);
        }

        private void b_answer_call_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_hfp_answer_call, null, null, null, null, null, null);
        }

        private void b_end_call_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_hfp_end_call, null, null, null, null, null, null);
        }

        private void b_hfp_num1_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "1";
        }

        private void b_hfp_num2_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "2";
        }

        private void b_hfp_num3_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "3";
        }

        private void b_hfp_num4_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "4";
        }

        private void b_hfp_num5_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "5";
        }

        private void b_hfp_num6_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "6";
        }

        private void b_hfp_num7_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "7";
        }

        private void b_hfp_num8_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "8";
        }

        private void b_hfp_num9_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "9";
        }

        private void b_hfp_num0_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text += "0";
        }

        private void b_hfp_clear_num_Click(object sender, EventArgs e)
        {
            l_hfp_call_num.Text = l_hfp_call_num.Text.Substring(0, l_hfp_call_num.Text.Length - 1);
        }

        private void b_hfp_call_number_Click(object sender, EventArgs e)
        {
            if (l_hfp_call_num.Text == "")
            {
                MessageBox.Show("请填入要拨打的手机号码", "错误提示");
                return;
            }
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_hfp_call_pn, l_hfp_call_num.Text, null, null, null, null, null);
        }

        private void b_hfp_get_lpn_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_hfp_get_lpn, null, null, null, null, null, null);
        }

        private void b_hfp_get_call_pn_Click(object sender, EventArgs e)
        {
            json_cmd_send(json_bt_cmd_func, json_bt_cmd_hfp_get_call_pn, null, null, null, null, null, null);
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
                            if (s != "")
                            {
                                t_data_recv.Text += (s + "\r\n");
                                t_data_recv.SelectionStart = t_data_recv.TextLength;
                                t_data_recv.ScrollToCaret();
                            }
                        }

                    }

                }
                catch (Exception ex)
                {
                    recv_json_str = "";
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

                if (status.OPERATE == "BT_CON_RESULT")
                {
                    if (status.PARAM1 == "SPP")
                    {
                        ui_bt_spp_show(true);
                        ui_bt_spp_con_status(true);
                        ui_bt_spp_con_status(status.PARAM2);
                    }

                    if (status.PARAM1 == "HFP")
                    {
                        ui_bt_hfp_show(true);
                        ui_bt_hfp_con_status(true);
                        ui_bt_hfp_con_status(status.PARAM2);
                    }

                }

                if (status.OPERATE == "BT_DISCON_RESULT")
                {
                    if (status.PARAM1 == "SPP")
                    {
                        ui_bt_spp_show(false);
                        ui_bt_spp_con_status(false);
                        ui_bt_spp_con_status(null);
                    }

                    if (status.PARAM1 == "HFP")
                    {
                        ui_bt_hfp_show(false);
                        ui_bt_hfp_answer_call_enable(false);
                        ui_bt_hfp_end_call_enable(false);
                        ui_bt_hfp_con_status(false);
                        ui_bt_hfp_con_status(null);

                        ui_bt_hfp_update_batt(null);
                        ui_bt_hfp_update_signal(null);

                    }
                }

                if (status.OPERATE == "BT_SPP_RECV")
                {
                    t_spp_recv_data.Text += status.PARAM1 + "\r\n";
                    int spp_recv_count = Convert.ToInt32(l_spp_recv_count.Text) + Convert.ToInt32(status.PARAM2);
                    l_spp_recv_count.Text = spp_recv_count.ToString();
                }

                if (status.OPERATE == "BT_HFP_SIGNAL_STRENGTH")
                {
                    string picture_name = "signal_" + status.PARAM1;
                    ui_bt_hfp_update_signal(picture_name);
                }

                if (status.OPERATE == "BT_HFP_BATT_LEVEL")
                {
                    string picture_name = "batt_" + status.PARAM1;
                    ui_bt_hfp_update_batt(picture_name);
                }

                if (status.OPERATE == "BT_HFP_OPERATOR")
                {
                    tb_hfp_operate.Text = status.PARAM1;
                }

                if (status.OPERATE == "BT_HFP_CALL_STATUS")
                {
                    int call_status = Convert.ToInt32(status.PARAM1);
                    if (call_status == HFP_CALL_NO_INPORCESS)
                    {
                        l_hfp_call_status.Text = "无通话";
                        bt_hfp_is_call_active = false;
                        bt_hfp_call_time_count = 0;
                        l_hfp_call_time.Text = "00:00:00";
                        l_hfp_call_pn.Text = "xxx-xxxx-xxxx";

                        ui_bt_hfp_answer_call_enable(false);
                        ui_bt_hfp_end_call_enable(false);
                        
                    }
                    else if (call_status == HFP_CALL_INPORCESS)
                    {
                        bt_hfp_is_call_active = true;
                        l_hfp_call_status.Text = "通话中";
                        ui_bt_hfp_answer_call_enable(false);
                        ui_bt_hfp_end_call_enable(true);
                    }
                }

                if (status.OPERATE == "BT_HFP_CALL_SETUP")
                {
                    int callsetup_status = Convert.ToInt32(status.PARAM1);
                    if (callsetup_status == HFP_CALL_INCOMING_CALL)
                    {
                        l_hfp_call_status.Text = "来电中";
                        ui_bt_hfp_answer_call_enable(true);
                        ui_bt_hfp_end_call_enable(true);
                    }
                    else if (callsetup_status == HFP_CALL_OUTGOING_CALL)
                    {
                        l_hfp_call_status.Text = "去电中";
                        ui_bt_hfp_answer_call_enable(false);
                        ui_bt_hfp_end_call_enable(true);
                    }
                }

                if (status.OPERATE == "BT_SCO_CON_RESULT")
                {
                    b_hfp_audio_transfer.Text = "切换音源" + "(目前声音在HF)";
                }

                if (status.OPERATE == "BT_SCO_DISCON_RESULT")
                {
                    b_hfp_audio_transfer.Text = "切换音源" + "(目前声音在AG)";
                }

                if (status.OPERATE == "BT_HFP_LOCAL_PN")
                {
                    tb_hfp_lpn.Text = status.PARAM1;
                }

                if (status.OPERATE == "BT_HFP_CALL_PN")
                {
                    l_hfp_call_pn.Text = status.PARAM1;
                }
                
                
            }
        }


        /* timer 中断 */
        private void timer1_Tick(object sender, EventArgs e)
        {
            if (bt_hfp_is_call_active)
            {
                bt_hfp_call_time_count += 1;
                int hours = bt_hfp_call_time_count / 3600;
                int minutes = (bt_hfp_call_time_count - hours * 3600) / 60;
                int seconds = bt_hfp_call_time_count - hours * 3600 - minutes * 60;
                l_hfp_call_time.Text = hours.ToString("00") + ":" + minutes.ToString("00") + ":" + seconds.ToString("00");
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

        /* SPP tabpage的显示使能 */
        private void ui_bt_spp_show(bool bt_spp_show)
        {
            if (bt_spp_show)
            {
                t_spp_recv_data.Enabled = true;
                t_spp_send_data.Enabled = true;
                b_spp_clear_recv_data.Enabled = true;
                b_spp_clear_send_data.Enabled = true;
                b_spp_send.Enabled = true;
                b_spp_clear_statistics.Enabled = true;
            }
            else
            {
                t_spp_recv_data.Enabled = false;
                t_spp_send_data.Enabled = false;
                b_spp_clear_recv_data.Enabled = false;
                b_spp_clear_send_data.Enabled = false;
                b_spp_send.Enabled = false;
                b_spp_clear_statistics.Enabled = false;
            }
        }

        /* SPP 刷新连接状态 */
        private void ui_bt_spp_con_status(bool bt_spp_con_status)
        {
            if (bt_spp_con_status)
            {
                l_spp_con_status.Text = "已连接";
            }
            else
            {
                l_spp_con_status.Text = "未连接";
            }
        }

        /* SPP 刷新连接地址 */
        private void ui_bt_spp_con_status(string bt_spp_con_addr)
        {
            if (bt_spp_con_addr != null)
            {
                l_spp_con_addr.Text = bt_spp_con_addr;
            }
            else
            {
                l_spp_con_addr.Text = "00:00:00:00:00:00";
            }
        }

        /* HFP tabpage的显示使能 */
        private void ui_bt_hfp_show(bool bt_hfp_show)
        {
            if (bt_hfp_show)
            {
                /* 所有控件显示 */
#if false
                b_hfp_num1.Enabled = true;
                b_hfp_num2.Enabled = true;
                b_hfp_num3.Enabled = true;
                b_hfp_num4.Enabled = true;
                b_hfp_num5.Enabled = true;
                b_hfp_num6.Enabled = true;
                b_hfp_num7.Enabled = true;
                b_hfp_num8.Enabled = true;
                b_hfp_num9.Enabled = true;
#endif
                gb_hfp_test_area.Enabled = true;
            }
            else
            {
                /* 所有控件灰掉 */
#if false
                b_hfp_num1.Enabled = false;
                b_hfp_num2.Enabled = false;
                b_hfp_num3.Enabled = false;
                b_hfp_num4.Enabled = false;
                b_hfp_num5.Enabled = false;
                b_hfp_num6.Enabled = false;
                b_hfp_num7.Enabled = false;
                b_hfp_num8.Enabled = false;
                b_hfp_num9.Enabled = false;
#endif
                gb_hfp_test_area.Enabled = false;
            }

        }

        /* HFP 刷新连接状态 */
        private void ui_bt_hfp_con_status(bool bt_hfp_con_status)
        {
            if (bt_hfp_con_status)
            {
                l_hfp_con_status.Text = "已连接";
            }
            else
            {
                l_hfp_con_status.Text = "未连接";
            }
        }

        /* HFP 刷新连接地址 */
        private void ui_bt_hfp_con_status(string bt_hfp_con_addr)
        {
            if (bt_hfp_con_addr != null)
            {
                l_hfp_con_addr.Text = bt_hfp_con_addr;
            }
            else
            {
                l_hfp_con_addr.Text = "00:00:00:00:00:00";
            }
        }

        /* 刷新HFP信号图标 */
        private void ui_bt_hfp_update_signal(string picture_name)
        {
            if (picture_name != null)
            {
                pb_hfp_signal.Image = (System.Drawing.Image)Properties.Resources.ResourceManager.GetObject(picture_name);
                pb_hfp_signal.SizeMode = PictureBoxSizeMode.Zoom;
            }
            else
                pb_hfp_signal.Image = null;
        }

        /* 刷新HFP电量图标 */
        private void ui_bt_hfp_update_batt(string picture_name)
        {
            if (picture_name != null)
            {
                pb_hfp_batt.Image = (System.Drawing.Image)Properties.Resources.ResourceManager.GetObject(picture_name);
                pb_hfp_batt.SizeMode = PictureBoxSizeMode.Zoom;
            }
            else
                pb_hfp_batt.Image = null;
        }

        private void ui_bt_hfp_answer_call_enable(bool enable)
        {
            if (enable)
                b_answer_call.Enabled = true;
            else
                b_answer_call.Enabled = false;
        }

        private void ui_bt_hfp_end_call_enable(bool enable)
        {
            if (enable)
                b_end_call.Enabled = true;
            else
                b_end_call.Enabled = false;
        }

        private void ui_bt_hfp_clear_call_num()
        {
            l_hfp_call_num.Text = "";
        }

        /* 整个UI的初始化 */
        private void ui_init()
        {
            ui_bt_switch_show(false,false);
            ui_bt_spp_show(false);
            //ui_bt_hfp_show(false);
            ui_bt_hfp_clear_call_num();
            
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
