using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace Windows窗体
{
    public partial class Form100 : Form
    {

        public static int choice = -1;
      
        public Form100()
        {
            InitializeComponent();
        }

        private void label1_Click(object sender, EventArgs e)
        {
            //
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            label1.Visible = true;
            label1.Text = choice.ToString() + "FetchOff_Watch";
            choice++; 
//            label1.Text = "OnlineImageStitch";
            label1.BackColor = Color.White;//背景颜色为蓝色
            label1.Location = new Point(50, 50);//位置
        }

        private void button2_Click(object sender, EventArgs e)
        {
            label1.Visible = true;
            label1.Text = choice.ToString()+"StitchOnline_Watch";
            choice--;
//            label1.Text = "OfflineImageStitch";
            label1.BackColor = Color.Blue;//背景颜色为蓝色
            label1.Location = new Point(50, 50);//位置
        }

        private void button3_Click(object sender, EventArgs e)
        {
            label1.Visible = true;
            label1.Text = choice.ToString() + "StitchOffline";
            choice=0;
            //            label1.Text = "OfflineImageStitch";
            label1.BackColor = Color.Blue;//背景颜色为蓝色
            label1.Location = new Point(50, 50);//位置
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

      
    }
    

}
