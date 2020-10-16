namespace mcu_bt_tool
{
    partial class Form1
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.b_board_buy = new System.Windows.Forms.Button();
            this.b_serial_close = new System.Windows.Forms.Button();
            this.b_serial_open = new System.Windows.Forms.Button();
            this.cb_serial_baudrate = new System.Windows.Forms.ComboBox();
            this.cb_serial_port = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tc_bt_function = new System.Windows.Forms.TabControl();
            this.tp_bt_basic_func = new System.Windows.Forms.TabPage();
            this.dg_inquiry_result = new System.Windows.Forms.DataGridView();
            this.cod = new System.Windows.Forms.DataGridViewImageColumn();
            this.address = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.name = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.b_stop_inquiry = new System.Windows.Forms.Button();
            this.b_start_inquiry = new System.Windows.Forms.Button();
            this.tp_bt_hfp_hf = new System.Windows.Forms.TabPage();
            this.pb_hfp_batt = new System.Windows.Forms.PictureBox();
            this.pb_hfp_signal = new System.Windows.Forms.PictureBox();
            this.gb_hfp_test_area = new System.Windows.Forms.GroupBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.b_end_call = new System.Windows.Forms.Button();
            this.b_hfp_audio_transfer = new System.Windows.Forms.Button();
            this.b_answer_call = new System.Windows.Forms.Button();
            this.label7 = new System.Windows.Forms.Label();
            this.l_hfp_call_time = new System.Windows.Forms.Label();
            this.l_hfp_call_status = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.b_hfp_get_operate = new System.Windows.Forms.Button();
            this.tb_hfp_operate = new System.Windows.Forms.TextBox();
            this.gb_dial_area = new System.Windows.Forms.GroupBox();
            this.b_hfp_num0 = new System.Windows.Forms.Button();
            this.b_hfp_num1 = new System.Windows.Forms.Button();
            this.l_hfp_call_num = new System.Windows.Forms.Label();
            this.b_hfp_num6 = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.b_hfp_num5 = new System.Windows.Forms.Button();
            this.b_hfp_call_number = new System.Windows.Forms.Button();
            this.b_hfp_num4 = new System.Windows.Forms.Button();
            this.b_hfp_clear_num = new System.Windows.Forms.Button();
            this.b_hfp_num2 = new System.Windows.Forms.Button();
            this.b_hfp_num7 = new System.Windows.Forms.Button();
            this.b_hfp_num9 = new System.Windows.Forms.Button();
            this.b_hfp_num3 = new System.Windows.Forms.Button();
            this.b_hfp_num8 = new System.Windows.Forms.Button();
            this.l_hfp_con_addr = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.l_hfp_con_status = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.tp_bt_spp = new System.Windows.Forms.TabPage();
            this.b_spp_clear_send_data = new System.Windows.Forms.Button();
            this.b_spp_clear_recv_data = new System.Windows.Forms.Button();
            this.b_spp_clear_statistics = new System.Windows.Forms.Button();
            this.b_spp_send = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.t_spp_send_data = new System.Windows.Forms.TextBox();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.t_spp_recv_data = new System.Windows.Forms.TextBox();
            this.l_spp_recv_count = new System.Windows.Forms.Label();
            this.l_spp_send_count = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.l_spp_con_addr = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.l_spp_con_status = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.g_log_func = new System.Windows.Forms.GroupBox();
            this.b_serial_send = new System.Windows.Forms.Button();
            this.t_data_send = new System.Windows.Forms.TextBox();
            this.t_data_recv = new System.Windows.Forms.TextBox();
            this.b_log_clear = new System.Windows.Forms.Button();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.b_bt_stop = new System.Windows.Forms.Button();
            this.b_bt_start = new System.Windows.Forms.Button();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.groupBox1.SuspendLayout();
            this.tc_bt_function.SuspendLayout();
            this.tp_bt_basic_func.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dg_inquiry_result)).BeginInit();
            this.tp_bt_hfp_hf.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_hfp_batt)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_hfp_signal)).BeginInit();
            this.gb_hfp_test_area.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.gb_dial_area.SuspendLayout();
            this.tp_bt_spp.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.g_log_func.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.BackColor = System.Drawing.Color.Transparent;
            this.groupBox1.Controls.Add(this.b_board_buy);
            this.groupBox1.Controls.Add(this.b_serial_close);
            this.groupBox1.Controls.Add(this.b_serial_open);
            this.groupBox1.Controls.Add(this.cb_serial_baudrate);
            this.groupBox1.Controls.Add(this.cb_serial_port);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(7, 10);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(586, 100);
            this.groupBox1.TabIndex = 3;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "串口配置区";
            // 
            // b_board_buy
            // 
            this.b_board_buy.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("b_board_buy.BackgroundImage")));
            this.b_board_buy.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.b_board_buy.Location = new System.Drawing.Point(450, 25);
            this.b_board_buy.Name = "b_board_buy";
            this.b_board_buy.Size = new System.Drawing.Size(130, 65);
            this.b_board_buy.TabIndex = 6;
            this.b_board_buy.UseVisualStyleBackColor = true;
            this.b_board_buy.Click += new System.EventHandler(this.b_board_buy_Click);
            // 
            // b_serial_close
            // 
            this.b_serial_close.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.b_serial_close.Enabled = false;
            this.b_serial_close.Image = ((System.Drawing.Image)(resources.GetObject("b_serial_close.Image")));
            this.b_serial_close.Location = new System.Drawing.Point(333, 25);
            this.b_serial_close.Name = "b_serial_close";
            this.b_serial_close.Size = new System.Drawing.Size(95, 65);
            this.b_serial_close.TabIndex = 5;
            this.b_serial_close.UseVisualStyleBackColor = true;
            this.b_serial_close.Click += new System.EventHandler(this.b_serial_close_Click);
            // 
            // b_serial_open
            // 
            this.b_serial_open.BackgroundImageLayout = System.Windows.Forms.ImageLayout.None;
            this.b_serial_open.Image = ((System.Drawing.Image)(resources.GetObject("b_serial_open.Image")));
            this.b_serial_open.Location = new System.Drawing.Point(220, 25);
            this.b_serial_open.Name = "b_serial_open";
            this.b_serial_open.Size = new System.Drawing.Size(95, 65);
            this.b_serial_open.TabIndex = 4;
            this.b_serial_open.UseVisualStyleBackColor = true;
            this.b_serial_open.Click += new System.EventHandler(this.b_serial_open_Click);
            // 
            // cb_serial_baudrate
            // 
            this.cb_serial_baudrate.AutoCompleteCustomSource.AddRange(new string[] {
            "115200",
            "921600"});
            this.cb_serial_baudrate.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cb_serial_baudrate.FormattingEnabled = true;
            this.cb_serial_baudrate.Items.AddRange(new object[] {
            "115200",
            "921600"});
            this.cb_serial_baudrate.Location = new System.Drawing.Point(73, 66);
            this.cb_serial_baudrate.Name = "cb_serial_baudrate";
            this.cb_serial_baudrate.Size = new System.Drawing.Size(121, 23);
            this.cb_serial_baudrate.TabIndex = 3;
            // 
            // cb_serial_port
            // 
            this.cb_serial_port.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cb_serial_port.FormattingEnabled = true;
            this.cb_serial_port.Location = new System.Drawing.Point(73, 25);
            this.cb_serial_port.Name = "cb_serial_port";
            this.cb_serial_port.Size = new System.Drawing.Size(121, 23);
            this.cb_serial_port.TabIndex = 2;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(8, 66);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(67, 15);
            this.label2.TabIndex = 1;
            this.label2.Text = "波特率：";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(8, 25);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(67, 15);
            this.label1.TabIndex = 0;
            this.label1.Text = "串口号：";
            // 
            // tc_bt_function
            // 
            this.tc_bt_function.Controls.Add(this.tp_bt_basic_func);
            this.tc_bt_function.Controls.Add(this.tp_bt_hfp_hf);
            this.tc_bt_function.Controls.Add(this.tp_bt_spp);
            this.tc_bt_function.Location = new System.Drawing.Point(8, 116);
            this.tc_bt_function.Name = "tc_bt_function";
            this.tc_bt_function.SelectedIndex = 0;
            this.tc_bt_function.Size = new System.Drawing.Size(870, 583);
            this.tc_bt_function.TabIndex = 5;
            // 
            // tp_bt_basic_func
            // 
            this.tp_bt_basic_func.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.tp_bt_basic_func.Controls.Add(this.dg_inquiry_result);
            this.tp_bt_basic_func.Controls.Add(this.b_stop_inquiry);
            this.tp_bt_basic_func.Controls.Add(this.b_start_inquiry);
            this.tp_bt_basic_func.Location = new System.Drawing.Point(4, 25);
            this.tp_bt_basic_func.Name = "tp_bt_basic_func";
            this.tp_bt_basic_func.Padding = new System.Windows.Forms.Padding(3);
            this.tp_bt_basic_func.Size = new System.Drawing.Size(862, 554);
            this.tp_bt_basic_func.TabIndex = 0;
            this.tp_bt_basic_func.Text = "基本功能";
            this.tp_bt_basic_func.UseVisualStyleBackColor = true;
            // 
            // dg_inquiry_result
            // 
            this.dg_inquiry_result.BackgroundColor = System.Drawing.Color.White;
            this.dg_inquiry_result.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dg_inquiry_result.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.cod,
            this.address,
            this.name});
            this.dg_inquiry_result.Location = new System.Drawing.Point(133, 26);
            this.dg_inquiry_result.Name = "dg_inquiry_result";
            this.dg_inquiry_result.RowTemplate.Height = 27;
            this.dg_inquiry_result.Size = new System.Drawing.Size(706, 503);
            this.dg_inquiry_result.TabIndex = 9;
            // 
            // cod
            // 
            this.cod.HeaderText = "设备类型";
            this.cod.Name = "cod";
            // 
            // address
            // 
            this.address.HeaderText = "设备地址";
            this.address.Name = "address";
            this.address.Width = 150;
            // 
            // name
            // 
            this.name.HeaderText = "设备名称";
            this.name.Name = "name";
            this.name.Width = 200;
            // 
            // b_stop_inquiry
            // 
            this.b_stop_inquiry.Enabled = false;
            this.b_stop_inquiry.Image = ((System.Drawing.Image)(resources.GetObject("b_stop_inquiry.Image")));
            this.b_stop_inquiry.Location = new System.Drawing.Point(23, 147);
            this.b_stop_inquiry.Name = "b_stop_inquiry";
            this.b_stop_inquiry.Size = new System.Drawing.Size(100, 104);
            this.b_stop_inquiry.TabIndex = 8;
            this.b_stop_inquiry.UseVisualStyleBackColor = true;
            this.b_stop_inquiry.Click += new System.EventHandler(this.b_stop_inquiry_Click);
            // 
            // b_start_inquiry
            // 
            this.b_start_inquiry.Enabled = false;
            this.b_start_inquiry.Image = ((System.Drawing.Image)(resources.GetObject("b_start_inquiry.Image")));
            this.b_start_inquiry.Location = new System.Drawing.Point(23, 26);
            this.b_start_inquiry.Name = "b_start_inquiry";
            this.b_start_inquiry.Size = new System.Drawing.Size(104, 104);
            this.b_start_inquiry.TabIndex = 7;
            this.b_start_inquiry.UseVisualStyleBackColor = true;
            this.b_start_inquiry.Click += new System.EventHandler(this.b_start_inquiry_Click);
            // 
            // tp_bt_hfp_hf
            // 
            this.tp_bt_hfp_hf.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.tp_bt_hfp_hf.Controls.Add(this.pb_hfp_batt);
            this.tp_bt_hfp_hf.Controls.Add(this.pb_hfp_signal);
            this.tp_bt_hfp_hf.Controls.Add(this.gb_hfp_test_area);
            this.tp_bt_hfp_hf.Controls.Add(this.l_hfp_con_addr);
            this.tp_bt_hfp_hf.Controls.Add(this.label5);
            this.tp_bt_hfp_hf.Controls.Add(this.l_hfp_con_status);
            this.tp_bt_hfp_hf.Controls.Add(this.label8);
            this.tp_bt_hfp_hf.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.tp_bt_hfp_hf.Location = new System.Drawing.Point(4, 25);
            this.tp_bt_hfp_hf.Name = "tp_bt_hfp_hf";
            this.tp_bt_hfp_hf.Padding = new System.Windows.Forms.Padding(3);
            this.tp_bt_hfp_hf.Size = new System.Drawing.Size(862, 554);
            this.tp_bt_hfp_hf.TabIndex = 1;
            this.tp_bt_hfp_hf.Text = "蓝牙电话";
            this.tp_bt_hfp_hf.UseVisualStyleBackColor = true;
            // 
            // pb_hfp_batt
            // 
            this.pb_hfp_batt.Location = new System.Drawing.Point(780, 13);
            this.pb_hfp_batt.Name = "pb_hfp_batt";
            this.pb_hfp_batt.Size = new System.Drawing.Size(53, 50);
            this.pb_hfp_batt.TabIndex = 23;
            this.pb_hfp_batt.TabStop = false;
            // 
            // pb_hfp_signal
            // 
            this.pb_hfp_signal.Location = new System.Drawing.Point(709, 13);
            this.pb_hfp_signal.Name = "pb_hfp_signal";
            this.pb_hfp_signal.Size = new System.Drawing.Size(53, 50);
            this.pb_hfp_signal.TabIndex = 22;
            this.pb_hfp_signal.TabStop = false;
            // 
            // gb_hfp_test_area
            // 
            this.gb_hfp_test_area.Controls.Add(this.groupBox4);
            this.gb_hfp_test_area.Controls.Add(this.groupBox3);
            this.gb_hfp_test_area.Controls.Add(this.gb_dial_area);
            this.gb_hfp_test_area.Location = new System.Drawing.Point(6, 62);
            this.gb_hfp_test_area.Name = "gb_hfp_test_area";
            this.gb_hfp_test_area.Size = new System.Drawing.Size(850, 486);
            this.gb_hfp_test_area.TabIndex = 21;
            this.gb_hfp_test_area.TabStop = false;
            this.gb_hfp_test_area.Text = "蓝牙电话测试区";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.b_end_call);
            this.groupBox4.Controls.Add(this.b_hfp_audio_transfer);
            this.groupBox4.Controls.Add(this.b_answer_call);
            this.groupBox4.Controls.Add(this.label7);
            this.groupBox4.Controls.Add(this.l_hfp_call_time);
            this.groupBox4.Controls.Add(this.l_hfp_call_status);
            this.groupBox4.Location = new System.Drawing.Point(254, 16);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(264, 464);
            this.groupBox4.TabIndex = 25;
            this.groupBox4.TabStop = false;
            // 
            // b_end_call
            // 
            this.b_end_call.Location = new System.Drawing.Point(142, 159);
            this.b_end_call.Name = "b_end_call";
            this.b_end_call.Size = new System.Drawing.Size(109, 38);
            this.b_end_call.TabIndex = 5;
            this.b_end_call.Text = "挂断电话";
            this.b_end_call.UseVisualStyleBackColor = true;
            this.b_end_call.Click += new System.EventHandler(this.b_end_call_Click);
            // 
            // b_hfp_audio_transfer
            // 
            this.b_hfp_audio_transfer.Location = new System.Drawing.Point(11, 215);
            this.b_hfp_audio_transfer.Name = "b_hfp_audio_transfer";
            this.b_hfp_audio_transfer.Size = new System.Drawing.Size(240, 52);
            this.b_hfp_audio_transfer.TabIndex = 4;
            this.b_hfp_audio_transfer.Text = "切换音源";
            this.b_hfp_audio_transfer.UseVisualStyleBackColor = true;
            this.b_hfp_audio_transfer.Click += new System.EventHandler(this.b_hfp_audio_transfer_Click);
            // 
            // b_answer_call
            // 
            this.b_answer_call.Location = new System.Drawing.Point(11, 159);
            this.b_answer_call.Name = "b_answer_call";
            this.b_answer_call.Size = new System.Drawing.Size(115, 38);
            this.b_answer_call.TabIndex = 3;
            this.b_answer_call.Text = "接听电话";
            this.b_answer_call.UseVisualStyleBackColor = true;
            this.b_answer_call.Click += new System.EventHandler(this.b_answer_call_Click);
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("微软雅黑", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label7.Location = new System.Drawing.Point(39, 110);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(188, 31);
            this.label7.TabIndex = 2;
            this.label7.Text = "xxx-xxxx-xxxx";
            // 
            // l_hfp_call_time
            // 
            this.l_hfp_call_time.AutoSize = true;
            this.l_hfp_call_time.Font = new System.Drawing.Font("微软雅黑", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.l_hfp_call_time.Location = new System.Drawing.Point(62, 75);
            this.l_hfp_call_time.Name = "l_hfp_call_time";
            this.l_hfp_call_time.Size = new System.Drawing.Size(118, 31);
            this.l_hfp_call_time.TabIndex = 1;
            this.l_hfp_call_time.Text = "00:00:00";
            // 
            // l_hfp_call_status
            // 
            this.l_hfp_call_status.AutoSize = true;
            this.l_hfp_call_status.Font = new System.Drawing.Font("微软雅黑", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.l_hfp_call_status.Location = new System.Drawing.Point(77, 31);
            this.l_hfp_call_status.Name = "l_hfp_call_status";
            this.l_hfp_call_status.Size = new System.Drawing.Size(86, 31);
            this.l_hfp_call_status.TabIndex = 0;
            this.l_hfp_call_status.Text = "无通话";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.b_hfp_get_operate);
            this.groupBox3.Controls.Add(this.tb_hfp_operate);
            this.groupBox3.Location = new System.Drawing.Point(7, 26);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(241, 454);
            this.groupBox3.TabIndex = 24;
            this.groupBox3.TabStop = false;
            // 
            // b_hfp_get_operate
            // 
            this.b_hfp_get_operate.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.b_hfp_get_operate.Location = new System.Drawing.Point(129, 26);
            this.b_hfp_get_operate.Name = "b_hfp_get_operate";
            this.b_hfp_get_operate.Size = new System.Drawing.Size(105, 39);
            this.b_hfp_get_operate.TabIndex = 1;
            this.b_hfp_get_operate.Text = "获取运营商";
            this.b_hfp_get_operate.UseVisualStyleBackColor = true;
            this.b_hfp_get_operate.Click += new System.EventHandler(this.b_hfp_get_operate_Click);
            // 
            // tb_hfp_operate
            // 
            this.tb_hfp_operate.Location = new System.Drawing.Point(7, 27);
            this.tb_hfp_operate.Multiline = true;
            this.tb_hfp_operate.Name = "tb_hfp_operate";
            this.tb_hfp_operate.Size = new System.Drawing.Size(116, 38);
            this.tb_hfp_operate.TabIndex = 0;
            // 
            // gb_dial_area
            // 
            this.gb_dial_area.Controls.Add(this.b_hfp_num0);
            this.gb_dial_area.Controls.Add(this.b_hfp_num1);
            this.gb_dial_area.Controls.Add(this.l_hfp_call_num);
            this.gb_dial_area.Controls.Add(this.b_hfp_num6);
            this.gb_dial_area.Controls.Add(this.label4);
            this.gb_dial_area.Controls.Add(this.b_hfp_num5);
            this.gb_dial_area.Controls.Add(this.b_hfp_call_number);
            this.gb_dial_area.Controls.Add(this.b_hfp_num4);
            this.gb_dial_area.Controls.Add(this.b_hfp_clear_num);
            this.gb_dial_area.Controls.Add(this.b_hfp_num2);
            this.gb_dial_area.Controls.Add(this.b_hfp_num7);
            this.gb_dial_area.Controls.Add(this.b_hfp_num9);
            this.gb_dial_area.Controls.Add(this.b_hfp_num3);
            this.gb_dial_area.Controls.Add(this.b_hfp_num8);
            this.gb_dial_area.Location = new System.Drawing.Point(524, 16);
            this.gb_dial_area.Name = "gb_dial_area";
            this.gb_dial_area.Size = new System.Drawing.Size(320, 464);
            this.gb_dial_area.TabIndex = 23;
            this.gb_dial_area.TabStop = false;
            // 
            // b_hfp_num0
            // 
            this.b_hfp_num0.Location = new System.Drawing.Point(115, 357);
            this.b_hfp_num0.Name = "b_hfp_num0";
            this.b_hfp_num0.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num0.TabIndex = 23;
            this.b_hfp_num0.Text = "0";
            this.b_hfp_num0.UseVisualStyleBackColor = true;
            this.b_hfp_num0.Click += new System.EventHandler(this.b_hfp_num0_Click);
            // 
            // b_hfp_num1
            // 
            this.b_hfp_num1.Location = new System.Drawing.Point(11, 69);
            this.b_hfp_num1.Name = "b_hfp_num1";
            this.b_hfp_num1.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num1.TabIndex = 10;
            this.b_hfp_num1.Text = "1";
            this.b_hfp_num1.UseVisualStyleBackColor = true;
            this.b_hfp_num1.Click += new System.EventHandler(this.b_hfp_num1_Click);
            // 
            // l_hfp_call_num
            // 
            this.l_hfp_call_num.AutoSize = true;
            this.l_hfp_call_num.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.l_hfp_call_num.Location = new System.Drawing.Point(110, 17);
            this.l_hfp_call_num.Name = "l_hfp_call_num";
            this.l_hfp_call_num.Size = new System.Drawing.Size(140, 27);
            this.l_hfp_call_num.TabIndex = 22;
            this.l_hfp_call_num.Text = "xxx-xxxx-xxxx";
            // 
            // b_hfp_num6
            // 
            this.b_hfp_num6.Location = new System.Drawing.Point(213, 165);
            this.b_hfp_num6.Name = "b_hfp_num6";
            this.b_hfp_num6.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num6.TabIndex = 14;
            this.b_hfp_num6.Text = "6";
            this.b_hfp_num6.UseVisualStyleBackColor = true;
            this.b_hfp_num6.Click += new System.EventHandler(this.b_hfp_num6_Click);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label4.Location = new System.Drawing.Point(7, 17);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(97, 27);
            this.label4.TabIndex = 21;
            this.label4.Text = "电话号码:";
            // 
            // b_hfp_num5
            // 
            this.b_hfp_num5.Location = new System.Drawing.Point(113, 165);
            this.b_hfp_num5.Name = "b_hfp_num5";
            this.b_hfp_num5.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num5.TabIndex = 15;
            this.b_hfp_num5.Text = "5";
            this.b_hfp_num5.UseVisualStyleBackColor = true;
            this.b_hfp_num5.Click += new System.EventHandler(this.b_hfp_num5_Click);
            // 
            // b_hfp_call_number
            // 
            this.b_hfp_call_number.Location = new System.Drawing.Point(12, 357);
            this.b_hfp_call_number.Name = "b_hfp_call_number";
            this.b_hfp_call_number.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_call_number.TabIndex = 20;
            this.b_hfp_call_number.Text = "拨打电话";
            this.b_hfp_call_number.UseVisualStyleBackColor = true;
            this.b_hfp_call_number.Click += new System.EventHandler(this.b_hfp_call_number_Click);
            // 
            // b_hfp_num4
            // 
            this.b_hfp_num4.Location = new System.Drawing.Point(11, 165);
            this.b_hfp_num4.Name = "b_hfp_num4";
            this.b_hfp_num4.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num4.TabIndex = 13;
            this.b_hfp_num4.Text = "4";
            this.b_hfp_num4.UseVisualStyleBackColor = true;
            this.b_hfp_num4.Click += new System.EventHandler(this.b_hfp_num4_Click);
            // 
            // b_hfp_clear_num
            // 
            this.b_hfp_clear_num.Location = new System.Drawing.Point(213, 357);
            this.b_hfp_clear_num.Name = "b_hfp_clear_num";
            this.b_hfp_clear_num.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_clear_num.TabIndex = 19;
            this.b_hfp_clear_num.Text = "清除号码";
            this.b_hfp_clear_num.UseVisualStyleBackColor = true;
            this.b_hfp_clear_num.Click += new System.EventHandler(this.b_hfp_clear_num_Click);
            // 
            // b_hfp_num2
            // 
            this.b_hfp_num2.Location = new System.Drawing.Point(113, 69);
            this.b_hfp_num2.Name = "b_hfp_num2";
            this.b_hfp_num2.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num2.TabIndex = 12;
            this.b_hfp_num2.Text = "2";
            this.b_hfp_num2.UseVisualStyleBackColor = true;
            this.b_hfp_num2.Click += new System.EventHandler(this.b_hfp_num2_Click);
            // 
            // b_hfp_num7
            // 
            this.b_hfp_num7.Location = new System.Drawing.Point(11, 261);
            this.b_hfp_num7.Name = "b_hfp_num7";
            this.b_hfp_num7.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num7.TabIndex = 16;
            this.b_hfp_num7.Text = "7";
            this.b_hfp_num7.UseVisualStyleBackColor = true;
            this.b_hfp_num7.Click += new System.EventHandler(this.b_hfp_num7_Click);
            // 
            // b_hfp_num9
            // 
            this.b_hfp_num9.Location = new System.Drawing.Point(213, 261);
            this.b_hfp_num9.Name = "b_hfp_num9";
            this.b_hfp_num9.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num9.TabIndex = 17;
            this.b_hfp_num9.Text = "9";
            this.b_hfp_num9.UseVisualStyleBackColor = true;
            this.b_hfp_num9.Click += new System.EventHandler(this.b_hfp_num9_Click);
            // 
            // b_hfp_num3
            // 
            this.b_hfp_num3.Location = new System.Drawing.Point(213, 69);
            this.b_hfp_num3.Name = "b_hfp_num3";
            this.b_hfp_num3.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num3.TabIndex = 11;
            this.b_hfp_num3.Text = "3";
            this.b_hfp_num3.UseVisualStyleBackColor = true;
            this.b_hfp_num3.Click += new System.EventHandler(this.b_hfp_num3_Click);
            // 
            // b_hfp_num8
            // 
            this.b_hfp_num8.Location = new System.Drawing.Point(113, 261);
            this.b_hfp_num8.Name = "b_hfp_num8";
            this.b_hfp_num8.Size = new System.Drawing.Size(90, 90);
            this.b_hfp_num8.TabIndex = 18;
            this.b_hfp_num8.Text = "8";
            this.b_hfp_num8.UseVisualStyleBackColor = true;
            this.b_hfp_num8.Click += new System.EventHandler(this.b_hfp_num8_Click);
            // 
            // l_hfp_con_addr
            // 
            this.l_hfp_con_addr.AutoSize = true;
            this.l_hfp_con_addr.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.l_hfp_con_addr.Location = new System.Drawing.Point(421, 21);
            this.l_hfp_con_addr.Name = "l_hfp_con_addr";
            this.l_hfp_con_addr.Size = new System.Drawing.Size(186, 27);
            this.l_hfp_con_addr.TabIndex = 7;
            this.l_hfp_con_addr.Text = "00:00:00:00:00:00";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label5.Location = new System.Drawing.Point(277, 20);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(138, 27);
            this.label5.TabIndex = 6;
            this.label5.Text = "HFP连接地址:";
            // 
            // l_hfp_con_status
            // 
            this.l_hfp_con_status.AutoSize = true;
            this.l_hfp_con_status.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.l_hfp_con_status.Location = new System.Drawing.Point(151, 20);
            this.l_hfp_con_status.Name = "l_hfp_con_status";
            this.l_hfp_con_status.Size = new System.Drawing.Size(72, 27);
            this.l_hfp_con_status.TabIndex = 5;
            this.l_hfp_con_status.Text = "未连接";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Font = new System.Drawing.Font("微软雅黑", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label8.Location = new System.Drawing.Point(7, 20);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(138, 27);
            this.label8.TabIndex = 4;
            this.label8.Text = "HFP连接状态:";
            // 
            // tp_bt_spp
            // 
            this.tp_bt_spp.Controls.Add(this.b_spp_clear_send_data);
            this.tp_bt_spp.Controls.Add(this.b_spp_clear_recv_data);
            this.tp_bt_spp.Controls.Add(this.b_spp_clear_statistics);
            this.tp_bt_spp.Controls.Add(this.b_spp_send);
            this.tp_bt_spp.Controls.Add(this.groupBox2);
            this.tp_bt_spp.Controls.Add(this.groupBox6);
            this.tp_bt_spp.Controls.Add(this.l_spp_recv_count);
            this.tp_bt_spp.Controls.Add(this.l_spp_send_count);
            this.tp_bt_spp.Controls.Add(this.label20);
            this.tp_bt_spp.Controls.Add(this.label21);
            this.tp_bt_spp.Controls.Add(this.l_spp_con_addr);
            this.tp_bt_spp.Controls.Add(this.label6);
            this.tp_bt_spp.Controls.Add(this.l_spp_con_status);
            this.tp_bt_spp.Controls.Add(this.label3);
            this.tp_bt_spp.Location = new System.Drawing.Point(4, 25);
            this.tp_bt_spp.Name = "tp_bt_spp";
            this.tp_bt_spp.Size = new System.Drawing.Size(862, 554);
            this.tp_bt_spp.TabIndex = 2;
            this.tp_bt_spp.Text = "蓝牙串口";
            this.tp_bt_spp.UseVisualStyleBackColor = true;
            // 
            // b_spp_clear_send_data
            // 
            this.b_spp_clear_send_data.Location = new System.Drawing.Point(693, 389);
            this.b_spp_clear_send_data.Name = "b_spp_clear_send_data";
            this.b_spp_clear_send_data.Size = new System.Drawing.Size(133, 31);
            this.b_spp_clear_send_data.TabIndex = 37;
            this.b_spp_clear_send_data.Text = "清除发送";
            this.b_spp_clear_send_data.UseVisualStyleBackColor = true;
            this.b_spp_clear_send_data.Click += new System.EventHandler(this.b_spp_clear_send_data_Click);
            // 
            // b_spp_clear_recv_data
            // 
            this.b_spp_clear_recv_data.Location = new System.Drawing.Point(560, 389);
            this.b_spp_clear_recv_data.Name = "b_spp_clear_recv_data";
            this.b_spp_clear_recv_data.Size = new System.Drawing.Size(123, 31);
            this.b_spp_clear_recv_data.TabIndex = 36;
            this.b_spp_clear_recv_data.Text = "清除接收";
            this.b_spp_clear_recv_data.UseVisualStyleBackColor = true;
            this.b_spp_clear_recv_data.Click += new System.EventHandler(this.b_spp_clear_recv_data_Click);
            // 
            // b_spp_clear_statistics
            // 
            this.b_spp_clear_statistics.Location = new System.Drawing.Point(560, 461);
            this.b_spp_clear_statistics.Name = "b_spp_clear_statistics";
            this.b_spp_clear_statistics.Size = new System.Drawing.Size(266, 30);
            this.b_spp_clear_statistics.TabIndex = 35;
            this.b_spp_clear_statistics.Text = "清除统计";
            this.b_spp_clear_statistics.UseVisualStyleBackColor = true;
            this.b_spp_clear_statistics.Click += new System.EventHandler(this.b_spp_clear_statistics_Click);
            // 
            // b_spp_send
            // 
            this.b_spp_send.Location = new System.Drawing.Point(560, 426);
            this.b_spp_send.Name = "b_spp_send";
            this.b_spp_send.Size = new System.Drawing.Size(266, 29);
            this.b_spp_send.TabIndex = 34;
            this.b_spp_send.Text = "发送";
            this.b_spp_send.UseVisualStyleBackColor = true;
            this.b_spp_send.Click += new System.EventHandler(this.b_spp_send_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.t_spp_send_data);
            this.groupBox2.Location = new System.Drawing.Point(12, 368);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(530, 163);
            this.groupBox2.TabIndex = 33;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "发送区";
            // 
            // t_spp_send_data
            // 
            this.t_spp_send_data.Location = new System.Drawing.Point(8, 23);
            this.t_spp_send_data.Multiline = true;
            this.t_spp_send_data.Name = "t_spp_send_data";
            this.t_spp_send_data.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.t_spp_send_data.Size = new System.Drawing.Size(509, 134);
            this.t_spp_send_data.TabIndex = 0;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.t_spp_recv_data);
            this.groupBox6.Location = new System.Drawing.Point(12, 107);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(839, 261);
            this.groupBox6.TabIndex = 32;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "接收区";
            // 
            // t_spp_recv_data
            // 
            this.t_spp_recv_data.Location = new System.Drawing.Point(6, 26);
            this.t_spp_recv_data.Multiline = true;
            this.t_spp_recv_data.Name = "t_spp_recv_data";
            this.t_spp_recv_data.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.t_spp_recv_data.Size = new System.Drawing.Size(827, 229);
            this.t_spp_recv_data.TabIndex = 0;
            // 
            // l_spp_recv_count
            // 
            this.l_spp_recv_count.AutoSize = true;
            this.l_spp_recv_count.Location = new System.Drawing.Point(620, 503);
            this.l_spp_recv_count.Name = "l_spp_recv_count";
            this.l_spp_recv_count.Size = new System.Drawing.Size(15, 15);
            this.l_spp_recv_count.TabIndex = 31;
            this.l_spp_recv_count.Text = "0";
            // 
            // l_spp_send_count
            // 
            this.l_spp_send_count.AutoSize = true;
            this.l_spp_send_count.Location = new System.Drawing.Point(751, 503);
            this.l_spp_send_count.Name = "l_spp_send_count";
            this.l_spp_send_count.Size = new System.Drawing.Size(15, 15);
            this.l_spp_send_count.TabIndex = 30;
            this.l_spp_send_count.Text = "0";
            // 
            // label20
            // 
            this.label20.AutoSize = true;
            this.label20.Location = new System.Drawing.Point(702, 503);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(45, 15);
            this.label20.TabIndex = 29;
            this.label20.Text = "发送:";
            // 
            // label21
            // 
            this.label21.AutoSize = true;
            this.label21.Location = new System.Drawing.Point(571, 503);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(45, 15);
            this.label21.TabIndex = 28;
            this.label21.Text = "接收:";
            // 
            // l_spp_con_addr
            // 
            this.l_spp_con_addr.AutoSize = true;
            this.l_spp_con_addr.Font = new System.Drawing.Font("微软雅黑", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.l_spp_con_addr.Location = new System.Drawing.Point(562, 55);
            this.l_spp_con_addr.Name = "l_spp_con_addr";
            this.l_spp_con_addr.Size = new System.Drawing.Size(229, 31);
            this.l_spp_con_addr.TabIndex = 3;
            this.l_spp_con_addr.Text = "00:00:00:00:00:00";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("微软雅黑", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label6.Location = new System.Drawing.Point(389, 55);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(163, 31);
            this.label6.TabIndex = 2;
            this.label6.Text = "SPP连接地址:";
            // 
            // l_spp_con_status
            // 
            this.l_spp_con_status.AutoSize = true;
            this.l_spp_con_status.Font = new System.Drawing.Font("微软雅黑", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.l_spp_con_status.Location = new System.Drawing.Point(195, 55);
            this.l_spp_con_status.Name = "l_spp_con_status";
            this.l_spp_con_status.Size = new System.Drawing.Size(86, 31);
            this.l_spp_con_status.TabIndex = 1;
            this.l_spp_con_status.Text = "未连接";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("微软雅黑", 13.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label3.Location = new System.Drawing.Point(26, 55);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(163, 31);
            this.label3.TabIndex = 0;
            this.label3.Text = "SPP连接状态:";
            // 
            // g_log_func
            // 
            this.g_log_func.BackColor = System.Drawing.Color.Transparent;
            this.g_log_func.Controls.Add(this.b_serial_send);
            this.g_log_func.Controls.Add(this.t_data_send);
            this.g_log_func.Controls.Add(this.t_data_recv);
            this.g_log_func.Controls.Add(this.b_log_clear);
            this.g_log_func.Location = new System.Drawing.Point(884, 12);
            this.g_log_func.Name = "g_log_func";
            this.g_log_func.Size = new System.Drawing.Size(541, 690);
            this.g_log_func.TabIndex = 6;
            this.g_log_func.TabStop = false;
            this.g_log_func.Text = "串口调试区";
            // 
            // b_serial_send
            // 
            this.b_serial_send.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("b_serial_send.BackgroundImage")));
            this.b_serial_send.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Zoom;
            this.b_serial_send.Location = new System.Drawing.Point(412, 614);
            this.b_serial_send.Name = "b_serial_send";
            this.b_serial_send.Size = new System.Drawing.Size(114, 67);
            this.b_serial_send.TabIndex = 3;
            this.b_serial_send.UseVisualStyleBackColor = true;
            this.b_serial_send.Click += new System.EventHandler(this.b_serial_send_Click);
            // 
            // t_data_send
            // 
            this.t_data_send.Location = new System.Drawing.Point(18, 614);
            this.t_data_send.Multiline = true;
            this.t_data_send.Name = "t_data_send";
            this.t_data_send.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.t_data_send.Size = new System.Drawing.Size(388, 66);
            this.t_data_send.TabIndex = 2;
            this.t_data_send.Text = "shop220811498.taobao.com";
            // 
            // t_data_recv
            // 
            this.t_data_recv.Location = new System.Drawing.Point(16, 23);
            this.t_data_recv.Multiline = true;
            this.t_data_recv.Name = "t_data_recv";
            this.t_data_recv.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.t_data_recv.Size = new System.Drawing.Size(510, 542);
            this.t_data_recv.TabIndex = 1;
            // 
            // b_log_clear
            // 
            this.b_log_clear.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("b_log_clear.BackgroundImage")));
            this.b_log_clear.Location = new System.Drawing.Point(18, 571);
            this.b_log_clear.Name = "b_log_clear";
            this.b_log_clear.Size = new System.Drawing.Size(508, 37);
            this.b_log_clear.TabIndex = 0;
            this.b_log_clear.Text = "清空调试日志";
            this.b_log_clear.UseVisualStyleBackColor = true;
            this.b_log_clear.Click += new System.EventHandler(this.b_log_clear_Click);
            // 
            // serialPort1
            // 
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // b_bt_stop
            // 
            this.b_bt_stop.Enabled = false;
            this.b_bt_stop.Image = ((System.Drawing.Image)(resources.GetObject("b_bt_stop.Image")));
            this.b_bt_stop.Location = new System.Drawing.Point(748, 20);
            this.b_bt_stop.Name = "b_bt_stop";
            this.b_bt_stop.Size = new System.Drawing.Size(89, 90);
            this.b_bt_stop.TabIndex = 4;
            this.b_bt_stop.UseVisualStyleBackColor = true;
            this.b_bt_stop.Click += new System.EventHandler(this.b_bt_stop_Click);
            // 
            // b_bt_start
            // 
            this.b_bt_start.Enabled = false;
            this.b_bt_start.Image = ((System.Drawing.Image)(resources.GetObject("b_bt_start.Image")));
            this.b_bt_start.Location = new System.Drawing.Point(624, 20);
            this.b_bt_start.Name = "b_bt_start";
            this.b_bt_start.Size = new System.Drawing.Size(91, 90);
            this.b_bt_start.TabIndex = 2;
            this.b_bt_start.UseVisualStyleBackColor = true;
            this.b_bt_start.Click += new System.EventHandler(this.b_bt_start_Click);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 1000;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.ClientSize = new System.Drawing.Size(1436, 705);
            this.Controls.Add(this.b_bt_stop);
            this.Controls.Add(this.b_bt_start);
            this.Controls.Add(this.g_log_func);
            this.Controls.Add(this.tc_bt_function);
            this.Controls.Add(this.groupBox1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.Resize += new System.EventHandler(this.Form1_Resize);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tc_bt_function.ResumeLayout(false);
            this.tp_bt_basic_func.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dg_inquiry_result)).EndInit();
            this.tp_bt_hfp_hf.ResumeLayout(false);
            this.tp_bt_hfp_hf.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pb_hfp_batt)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pb_hfp_signal)).EndInit();
            this.gb_hfp_test_area.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.gb_dial_area.ResumeLayout(false);
            this.gb_dial_area.PerformLayout();
            this.tp_bt_spp.ResumeLayout(false);
            this.tp_bt_spp.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.g_log_func.ResumeLayout(false);
            this.g_log_func.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button b_bt_stop;
        private System.Windows.Forms.Button b_bt_start;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button b_board_buy;
        private System.Windows.Forms.Button b_serial_close;
        private System.Windows.Forms.Button b_serial_open;
        private System.Windows.Forms.ComboBox cb_serial_baudrate;
        private System.Windows.Forms.ComboBox cb_serial_port;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TabControl tc_bt_function;
        private System.Windows.Forms.TabPage tp_bt_basic_func;
        private System.Windows.Forms.TabPage tp_bt_hfp_hf;
        private System.Windows.Forms.TabPage tp_bt_spp;
        private System.Windows.Forms.GroupBox g_log_func;
        private System.Windows.Forms.Button b_serial_send;
        private System.Windows.Forms.TextBox t_data_send;
        private System.Windows.Forms.TextBox t_data_recv;
        private System.Windows.Forms.Button b_log_clear;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Button b_stop_inquiry;
        private System.Windows.Forms.Button b_start_inquiry;
        private System.Windows.Forms.DataGridView dg_inquiry_result;
        private System.Windows.Forms.DataGridViewImageColumn cod;
        private System.Windows.Forms.DataGridViewTextBoxColumn address;
        private System.Windows.Forms.DataGridViewTextBoxColumn name;
        private System.Windows.Forms.Button b_spp_clear_send_data;
        private System.Windows.Forms.Button b_spp_clear_recv_data;
        private System.Windows.Forms.Button b_spp_clear_statistics;
        private System.Windows.Forms.Button b_spp_send;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TextBox t_spp_send_data;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.TextBox t_spp_recv_data;
        private System.Windows.Forms.Label l_spp_recv_count;
        private System.Windows.Forms.Label l_spp_send_count;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Label l_spp_con_addr;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label l_spp_con_status;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label l_hfp_con_addr;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label l_hfp_con_status;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Button b_hfp_num8;
        private System.Windows.Forms.Button b_hfp_num9;
        private System.Windows.Forms.Button b_hfp_num7;
        private System.Windows.Forms.Button b_hfp_num5;
        private System.Windows.Forms.Button b_hfp_num6;
        private System.Windows.Forms.Button b_hfp_num4;
        private System.Windows.Forms.Button b_hfp_num2;
        private System.Windows.Forms.Button b_hfp_num3;
        private System.Windows.Forms.Button b_hfp_num1;
        private System.Windows.Forms.GroupBox gb_hfp_test_area;
        private System.Windows.Forms.GroupBox gb_dial_area;
        private System.Windows.Forms.Label l_hfp_call_num;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button b_hfp_call_number;
        private System.Windows.Forms.Button b_hfp_clear_num;
        private System.Windows.Forms.Button b_hfp_num0;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.PictureBox pb_hfp_batt;
        private System.Windows.Forms.PictureBox pb_hfp_signal;
        private System.Windows.Forms.Button b_hfp_get_operate;
        private System.Windows.Forms.TextBox tb_hfp_operate;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Button b_end_call;
        private System.Windows.Forms.Button b_hfp_audio_transfer;
        private System.Windows.Forms.Button b_answer_call;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label l_hfp_call_time;
        private System.Windows.Forms.Label l_hfp_call_status;
        private System.Windows.Forms.Timer timer1;
    }
}

