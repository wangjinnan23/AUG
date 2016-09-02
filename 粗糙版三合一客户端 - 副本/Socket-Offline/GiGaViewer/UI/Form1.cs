using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
//add
//using System.IO;
//using System.Net;
//using System.Net.Sockets;

namespace UI
{
    public partial class GiGaViewer : Form
    {
        bool isFullScreen = false;
        bool isConnect = false;

        public GiGaViewer()
        {
            InitializeComponent();
        }
        private void FullScreen_Click(object sender, EventArgs e)
        {
            isFullScreen = !isFullScreen;
            if (isFullScreen)
            {
                this.WindowState = FormWindowState.Maximized;
            }
            else
            {
                this.WindowState = FormWindowState.Normal;
            }
        }

        private void OpenButton_Click(object sender, EventArgs e)
        {

            //what is the function?
            isConnect = !isConnect;

            if (isConnect)
            {
                this.imageCtrl1.Start();
            }
            else
            {
                this.imageCtrl1.m_socketFunc.m_socket.Close();
            }
        }

        private void Play_Click(object sender, EventArgs e){}

        private void pause_Click(object sender, EventArgs e){}

        //show Panarama window, but still no function here
        private void OpenPanaroma_Click(object sender, EventArgs e){}

        private void ScreenShot_Click(object sender, EventArgs e)
        {
            Bitmap curBitmap = imageCtrl1.SaveImg();
            if (curBitmap == null) return;

            SaveFileDialog saveDlg = new SaveFileDialog();
            saveDlg.Title = "局部图片另存为";
            saveDlg.OverwritePrompt = true;
            saveDlg.Filter = "JPG文件(*.jpg)|*.jpg|PNG文件(*.png)|*.png|BMP文件(*.bmp)|*.bmp";
            saveDlg.ShowHelp = true;

            if (saveDlg.ShowDialog() == DialogResult.OK)
            {
                string fileName = saveDlg.FileName;
                string strFilExtn = fileName.Remove(0, fileName.Length - 3);
                switch (strFilExtn)
                {
                    case "bmp":
                        curBitmap.Save(fileName, System.Drawing.Imaging.ImageFormat.Bmp);
                        break;
                    case "jpg":
                        curBitmap.Save(fileName, System.Drawing.Imaging.ImageFormat.Jpeg);
                        break;
                    case "png":
                        curBitmap.Save(fileName, System.Drawing.Imaging.ImageFormat.Png);
                        break;
                    default:
                        break;
                }
            }
        }

        private void Plot_Click(object sender, EventArgs e) { }

     
        private void GiGaViewer_Resize(object sender, EventArgs e)
        {
            this.Refresh();
        }


        //show viewr 
        private void GiGaViewer_Load(object sender, EventArgs e)
        {}


        //IP setteing window
        private void iPSetting_Click(object sender, EventArgs e)
        {
            Form2 f2 = new Form2();
            f2.ShowDialog();         

        }

        private void button1_Click(object sender, EventArgs e)
        {
     
//            this.MouseWheel += new MouseEventHandler(imageCtrl1.ImageCtrl_MouseWheel);
            imageCtrl1.m_socketFunc.SocketInit();
//            InitializeComponent(); 
//            this.DoubleBuffered = true;
            imageCtrl1.UpdateRectShow();
//            imageCtrl1.UpdateScrollbarInit();
            imageCtrl1.UpdateMainImg();//UpdateMainImg
             

            //m_socketFunc.SocketInit();
            //InitializeComponent();
            //this.DoubleBuffered = true;
            //UpdateRectShow();
            //UpdateScrollbarInit();
        }

        private void button2_Click(object sender, EventArgs e)
        {
            UI.Mode A = new Mode();
            A.Selection = 1;
            ImageCtrl.Mode_ImageCtrl AA = new ImageCtrl.Mode_ImageCtrl();
            AA.Selection = A.Selection;
            imageCtrl1.m_socketFunc.SocketInit1();
            imageCtrl1.UpdateRectShow();
            imageCtrl1.UpdateMainImg1();//UpdateMainImg
          //  imageCtrl1.UpdateScrollbarInit();//UpdateScrollbarInit();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            imageCtrl1.m_socketFunc.SocketInit2();
            imageCtrl1.UpdateRectShow();
            //imageCtrl1.UpdateMainImg1();


        }

        

    }
}
