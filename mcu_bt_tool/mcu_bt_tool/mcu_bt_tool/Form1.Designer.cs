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
            this.b_bt_stop = new System.Windows.Forms.Button();
            this.b_bt_start = new System.Windows.Forms.Button();
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
            this.tp_bt_spp = new System.Windows.Forms.TabPage();
            this.g_log_func = new System.Windows.Forms.GroupBox();
            this.b_serial_send = new System.Windows.Forms.Button();
            this.t_data_send = new System.Windows.Forms.TextBox();
            this.t_data_recv = new System.Windows.Forms.TextBox();
            this.b_log_clear = new System.Windows.Forms.Button();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.groupBox1.SuspendLayout();
            this.tc_bt_function.SuspendLayout();
            this.tp_bt_basic_func.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dg_inquiry_result)).BeginInit();
            this.g_log_func.SuspendLayout();
            this.SuspendLayout();
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
            this.tp_bt_basic_func.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("tp_bt_basic_func.BackgroundImage")));
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
            this.tp_bt_hfp_hf.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("tp_bt_hfp_hf.BackgroundImage")));
            this.tp_bt_hfp_hf.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Stretch;
            this.tp_bt_hfp_hf.Font = new System.Drawing.Font("微软雅黑", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.tp_bt_hfp_hf.Location = new System.Drawing.Point(4, 25);
            this.tp_bt_hfp_hf.Name = "tp_bt_hfp_hf";
            this.tp_bt_hfp_hf.Padding = new System.Windows.Forms.Padding(3);
            this.tp_bt_hfp_hf.Size = new System.Drawing.Size(862, 554);
            this.tp_bt_hfp_hf.TabIndex = 1;
            this.tp_bt_hfp_hf.Text = "蓝牙电话";
            this.tp_bt_hfp_hf.UseVisualStyleBackColor = true;
            // 
            // tp_bt_spp
            // 
            this.tp_bt_spp.Location = new System.Drawing.Point(4, 25);
            this.tp_bt_spp.Name = "tp_bt_spp";
            this.tp_bt_spp.Size = new System.Drawing.Size(862, 554);
            this.tp_bt_spp.TabIndex = 2;
            this.tp_bt_spp.Text = "蓝牙串口";
            this.tp_bt_spp.UseVisualStyleBackColor = true;
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
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
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
    }
}

