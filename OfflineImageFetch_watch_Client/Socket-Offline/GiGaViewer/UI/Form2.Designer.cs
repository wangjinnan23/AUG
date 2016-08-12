namespace UI
{
    partial class Form2
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.txt_Old_PCName = new System.Windows.Forms.Label();
            this.NetCardNumber = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.ServiceName = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.Index = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.ComboBox = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.txtIP = new System.Windows.Forms.TextBox();
            this.PortNum = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // txt_Old_PCName
            // 
            this.txt_Old_PCName.AutoSize = true;
            this.txt_Old_PCName.Location = new System.Drawing.Point(24, 13);
            this.txt_Old_PCName.Name = "txt_Old_PCName";
            this.txt_Old_PCName.Size = new System.Drawing.Size(47, 12);
            this.txt_Old_PCName.TabIndex = 0;
            this.txt_Old_PCName.Text = "PC Name";
            // 
            // NetCardNumber
            // 
            this.NetCardNumber.AutoSize = true;
            this.NetCardNumber.Location = new System.Drawing.Point(24, 54);
            this.NetCardNumber.Name = "NetCardNumber";
            this.NetCardNumber.Size = new System.Drawing.Size(77, 12);
            this.NetCardNumber.TabIndex = 1;
            this.NetCardNumber.Text = "Mac address:";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.ForeColor = System.Drawing.Color.Red;
            this.label3.Location = new System.Drawing.Point(126, 54);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(83, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "NetCardNumber";
            // 
            // ServiceName
            // 
            this.ServiceName.AutoSize = true;
            this.ServiceName.Location = new System.Drawing.Point(231, 54);
            this.ServiceName.Name = "ServiceName";
            this.ServiceName.Size = new System.Drawing.Size(77, 12);
            this.ServiceName.TabIndex = 3;
            this.ServiceName.Text = "Service Name";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.ForeColor = System.Drawing.Color.Red;
            this.label5.Location = new System.Drawing.Point(324, 54);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(77, 12);
            this.label5.TabIndex = 4;
            this.label5.Text = "Service Name";
            // 
            // Index
            // 
            this.Index.AutoSize = true;
            this.Index.Location = new System.Drawing.Point(436, 54);
            this.Index.Name = "Index";
            this.Index.Size = new System.Drawing.Size(35, 12);
            this.Index.TabIndex = 5;
            this.Index.Text = "Index";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.ForeColor = System.Drawing.Color.Red;
            this.label7.Location = new System.Drawing.Point(493, 54);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(35, 12);
            this.label7.TabIndex = 6;
            this.label7.Text = "Index";
            // 
            // ComboBox
            // 
            this.ComboBox.FormattingEnabled = true;
            this.ComboBox.Location = new System.Drawing.Point(145, 13);
            this.ComboBox.Name = "ComboBox";
            this.ComboBox.Size = new System.Drawing.Size(297, 20);
            this.ComboBox.TabIndex = 7;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(298, 106);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(59, 12);
            this.label1.TabIndex = 8;
            this.label1.Text = "Server IP";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(24, 105);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 9;
            this.label2.Text = "Local IP";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(118, 102);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(129, 21);
            this.textBox1.TabIndex = 10;
            // 
            // txtIP
            // 
            this.txtIP.Location = new System.Drawing.Point(363, 103);
            this.txtIP.Name = "txtIP";
            this.txtIP.Size = new System.Drawing.Size(143, 21);
            this.txtIP.TabIndex = 11;
            this.txtIP.TextChanged += new System.EventHandler(this.txtIP_TextChanged);
            // 
            // PortNum
            // 
            this.PortNum.Location = new System.Drawing.Point(363, 163);
            this.PortNum.Name = "PortNum";
            this.PortNum.Size = new System.Drawing.Size(143, 21);
            this.PortNum.TabIndex = 12;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(298, 166);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(29, 12);
            this.label4.TabIndex = 13;
            this.label4.Text = "Port";
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(406, 227);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(75, 23);
            this.button1.TabIndex = 14;
            this.button1.Text = "IP Set";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // Form2
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(564, 262);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.PortNum);
            this.Controls.Add(this.txtIP);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.ComboBox);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.Index);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.ServiceName);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.NetCardNumber);
            this.Controls.Add(this.txt_Old_PCName);
            this.Name = "Form2";
            this.Text = "Form2";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label txt_Old_PCName;
        private System.Windows.Forms.Label NetCardNumber;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label ServiceName;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label Index;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.ComboBox ComboBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox txtIP;
        private System.Windows.Forms.TextBox PortNum;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button1;
    }
}