using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;

using System.Drawing;
using System.Drawing.Imaging;

using System.Net;
using System.Net.Sockets;


namespace ImageCtrl
{
    public partial class ImageCtrl : UserControl
    {
        public Size GigaImgSize = new Size(12000, 12000);//need to change here

        public int m_frameNum = 1;

        public int m_totalFrameNum = Int32.MaxValue;

        public int m_scaleIndex = 0;

        public Point m_leftTopOnImgRated = new Point();


        Point m_oldLeftTopOnImgRated = new Point();

        public Rectangle m_scrRectShow = new Rectangle();

        public Rectangle m_showImgRect = new Rectangle();

        Rectangle destRect = new Rectangle(0, 0, 1000, 500);////修改图片显示框大小后
      
        //Rectangle destRect = new Rectangle(0, 0, 682, 260);//add rectangle
        
        Point m_mouseLeftDown = new Point();

        bool m_isMouseLeftDown = false;

        Bitmap m_imgShow = null;

        //add extra elements

        Bitmap m_imgPreview = null;
        Size m_imgSize = new Size();
        public int m_baseSquareLength = 512;
       
        public int[] m_scales = new int[] { 3, 6, 9, 12, 15, 18, 21, 24 };


        public SOCKETcomunication m_socketFunc = new SOCKETcomunication();

        public ImageCtrl()  //start main control 
        {
            this.MouseWheel += new MouseEventHandler(ImageCtrl_MouseWheel);
            m_socketFunc.SocketInit();
            InitializeComponent();
            this.DoubleBuffered = true;
            UpdateRectShow();
            UpdateScrollbarInit();
        }

        private void ImageCtrl_Paint(object sender, PaintEventArgs e)
        {
            e.Graphics.FillRectangle(Brushes.Black, m_scrRectShow);
            if (m_imgShow != null)         

            {
          //  e.Graphics.DrawImage(m_imgShow, destRect /*/m_scrRectShow.Location/*/); //draw picture in specific location, need to change here!!

                e.Graphics.DrawImage(m_imgShow, 0, 0);
            }
        }

        private void ImageCtrl_Resize(object sender, EventArgs e)
        {
            UpdateRectShow();
            UpdateMainImg();
            UpdateScrollbar();

            this.Refresh();
        }

        private void ImageCtrl_Load(object sender, EventArgs e)
        {
            UpdateScrollbar();
        }

        public void Start()
        {
            m_leftTopOnImgRated.X = 0;
            m_leftTopOnImgRated.Y = 0;

            UpdateMainImg();
            UpdateScrollbarInit();

            this.Refresh();
        }

        public Bitmap SaveImg()
        {
            return m_imgShow;
        }


        private void BoundryRestriction()
        {
            m_leftTopOnImgRated.X = m_leftTopOnImgRated.X >= GigaImgSize.Width - m_scrRectShow.Width ? GigaImgSize.Width - m_scrRectShow.Width : m_leftTopOnImgRated.X;
            m_leftTopOnImgRated.Y = m_leftTopOnImgRated.Y >= GigaImgSize.Height - m_scrRectShow.Height ? GigaImgSize.Height - m_scrRectShow.Height : m_leftTopOnImgRated.Y;
            m_leftTopOnImgRated.X = m_leftTopOnImgRated.X < 0 ? 0 : m_leftTopOnImgRated.X;
            m_leftTopOnImgRated.Y = m_leftTopOnImgRated.Y < 0 ? 0 : m_leftTopOnImgRated.Y;
        }

        private void UpdateRectShow()
        {
            m_scrRectShow.X = this.hScrollBarImg.Location.X;
            m_scrRectShow.Y = this.vScrollBarImg.Location.Y;
            m_scrRectShow.Width = this.hScrollBarImg.Size.Width - 1;
            m_scrRectShow.Height = this.vScrollBarImg.Size.Height - 1;
            m_scrRectShow.Width = m_scrRectShow.Width < 1 ? 1 : m_scrRectShow.Width;
            m_scrRectShow.Height = m_scrRectShow.Height < 1 ? 1 : m_scrRectShow.Height;
        }

        public void UpdateMainImg()
        {
            if (this.m_scrRectShow.Width <= 0 || this.m_scrRectShow.Height <= 0)
            {
                return;
            }

            if (m_imgShow != null)
            {
                m_imgShow.Dispose();
            }

            m_showImgRect.X = m_leftTopOnImgRated.X;//send X and Y to the socket 
            m_showImgRect.Y = m_leftTopOnImgRated.Y;
            m_showImgRect.Width = m_scrRectShow.Width;
            m_showImgRect.Height = m_scrRectShow.Height;

            m_imgShow = m_socketFunc.GetImage(m_frameNum, m_scales[m_scaleIndex], m_showImgRect);//call socket image to save 

            m_imgShow.Save("1.jpg");

            this.Refresh();
        }

        void ImageCtrl_MouseWheel(object sender, MouseEventArgs e)
        {
            /*/if (m_iz.ShowImage == null)
            {
                return;
            }/*/

           // if (!pan.GetPanRec().Contains(e.Location))
            //{
                int oldScaleIndex = m_scaleIndex;

               // if (e.Delta > 0) 
                if (e.Delta < 0)  //此处为修改后的判断条件
                {
                    m_scaleIndex++;
                }
                else
                {
                    m_scaleIndex--;
                }

                m_scaleIndex = m_scaleIndex < 0 ? 0 : m_scaleIndex;
                m_scaleIndex = m_scaleIndex >= m_scales.Length ? m_scales.Length - 1 : m_scaleIndex;
                int dstCenterX = m_leftTopOnImgRated.X - m_scales[oldScaleIndex] * destRect.Width / 2;
                int dstCenterY = m_leftTopOnImgRated.Y - m_scales[oldScaleIndex] * destRect.Height / 2;
                //int dstCenterX = (e.X + m_leftTopOnImgRated.X) * m_scales[m_scaleIndex] / m_scales[oldScaleIndex];
                //int dstCenterY = (e.Y + m_leftTopOnImgRated.Y) * m_scales[m_scaleIndex] / m_scales[oldScaleIndex];

                m_leftTopOnImgRated.X = dstCenterX + m_scales[m_scaleIndex] * destRect.Width / 2;
                m_leftTopOnImgRated.Y = dstCenterY + m_scales[m_scaleIndex] * destRect.Height / 2;

                BoundryRestriction();//边界判断
                UpdateScrollbar();//滚动条更新
                UpdateMainImg();//更新图像
            //}
        }

       
        //click mouse to move the picture 
        private void ImageCtrl_MouseDown(object sender, MouseEventArgs e)
        {
            if (m_imgShow == null)
            {
                return;
            }

            if (!m_scrRectShow.Contains(e.Location))
            {
                return;
            }

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                m_oldLeftTopOnImgRated = m_leftTopOnImgRated;
                m_mouseLeftDown = e.Location;
                m_isMouseLeftDown = true;
            }
        }

        private void ImageCtrl_MouseUp(object sender, MouseEventArgs e)
        {
            if (m_imgShow == null)
            {
                return;
            }

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                m_isMouseLeftDown = false;
            }
        }


        private void ImageCtrl_MouseMove(object sender, MouseEventArgs e)//need to change here to the right 
        {
            if (m_imgShow == null)
            {
                return;
            }

            this.Focus();

            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                if (m_isMouseLeftDown)//decide whether user click left of the mouse 
                {
                    int offsetX = e.X - m_mouseLeftDown.X;
                    int offsetY = e.Y - m_mouseLeftDown.Y;
                    m_leftTopOnImgRated.X = m_oldLeftTopOnImgRated.X - offsetX;
                    m_leftTopOnImgRated.Y = m_oldLeftTopOnImgRated.Y - offsetY;
                    
                    BoundryRestriction();
                    UpdateMainImg();
                    UpdateScrollbar();
                }
                this.Refresh();
            }
        }
        
       
        private void vScrollBarImg_Scroll(object sender, ScrollEventArgs e)
        {
            m_leftTopOnImgRated.Y = this.vScrollBarImg.Value;
            UpdateMainImg();
        }

        private void hScrollBarImg_Scroll(object sender, ScrollEventArgs e)
        {
            m_leftTopOnImgRated.X = this.hScrollBarImg.Value;
            UpdateMainImg();

        }

        void UpdateScrollbar()
        {
            if (this.m_scrRectShow.Width <= 0 || this.m_scrRectShow.Height <= 0)
            {
                this.hScrollBarImg.Enabled = false;
                this.vScrollBarImg.Enabled = false;
                return;
            }

            if (m_imgShow == null)
            {
                this.hScrollBarImg.Enabled = false;
                this.vScrollBarImg.Enabled = false;
                return;
            }

            int dstWidth = GigaImgSize.Width;
            int dstHeight = GigaImgSize.Height;

            if (dstWidth <= this.m_scrRectShow.Width)
            {
                this.hScrollBarImg.Enabled = false;
            }
            else
            {
                this.hScrollBarImg.Enabled = true;
                this.hScrollBarImg.Minimum = 0;
                this.hScrollBarImg.Maximum = dstWidth - this.m_scrRectShow.Width;
                this.hScrollBarImg.Value = m_leftTopOnImgRated.X;
            }

            if (dstHeight <= this.m_scrRectShow.Height)
            {
                this.vScrollBarImg.Enabled = false;
            }
            else
            {
                this.vScrollBarImg.Enabled = true;
                this.vScrollBarImg.Minimum = 0;
                this.vScrollBarImg.Maximum = dstHeight - this.m_scrRectShow.Height;
                this.vScrollBarImg.Value = m_leftTopOnImgRated.Y;
            }
        }

        void UpdateScrollbarInit()
        {
            if (this.m_scrRectShow.Width <= 0 || this.m_scrRectShow.Height <= 0)
            {
                this.hScrollBarImg.Enabled = false;
                this.vScrollBarImg.Enabled = false;

                return;
            }
            if (m_imgShow == null)
            {
                this.hScrollBarImg.Enabled = false;
                this.vScrollBarImg.Enabled = false;
                return;
            }

            int dstWidth = GigaImgSize.Width;
            int dstHeight = GigaImgSize.Height;

            if (dstWidth <= this.m_scrRectShow.Width)
            {
                this.hScrollBarImg.Enabled = false;
            }
            else
            {
                this.hScrollBarImg.Enabled = true;
                this.hScrollBarImg.Minimum = 0;
                this.hScrollBarImg.Maximum = dstWidth - this.m_scrRectShow.Width;
                this.hScrollBarImg.Value = m_leftTopOnImgRated.X;
            }

            if (dstHeight <= this.m_scrRectShow.Height)
            {
                this.vScrollBarImg.Enabled = false;
            }
            else
            {
                this.vScrollBarImg.Enabled = true;
                this.vScrollBarImg.Minimum = 0;
                this.vScrollBarImg.Maximum = dstHeight - this.m_scrRectShow.Height;
                this.vScrollBarImg.Value = m_leftTopOnImgRated.Y;
            }
        }
    }
         
    public class SOCKETcomunication
    {
        int port = 5555;  //2000
        public Point m_RightDownOnImgRated = new Point();//add

        string host = "166.111.65.127";
       // string host="166.111.65.195"
        //Central server port=7000
        //166.111.66.151
        //Windows IP= 166.111.66.2 port=  2000
        //Linux IP=166.111.65.252  port = 2000

        public Socket m_socket;
        //public Socket new_socket;

      

        public void IPPORTProcess(string buf, ref string IPInput, ref string PortInput)
        {
            string[] command = buf.Split(':');
            switch (Convert.ToInt32(command[0]))
            {
                case 0x0020:
                    IPInput = command[1];
                    PortInput = command[2];
                    break;
                case 0x0000:
                    break; // box output
                default:
                    break;
            }
        }

        public void SocketInit() 
        {

        m_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);         

        IPAddress ServerIP = IPAddress.Parse(host);

        IPEndPoint IPE = new IPEndPoint(ServerIP, port);
       
        m_socket.Connect(IPE);

      //      Switch of Online and Offline
            //int[] temp2 = new int[8] { 1, 1, 960, 540, 5000, 5960, 1000, 1540};
            //byte[] message2 = new byte[32];

            //for (int i = 0; i < 8; i++)
            //{
            //    (System.BitConverter.GetBytes(temp2[i])).CopyTo(message2, i * 4);

            //}


            //m_socket.Send(message2);
            

        //send commend to central server

        /*/
        int command = 30;
        m_socket.Send(Encoding.ASCII.GetBytes(command.ToString()));

        byte[] get = new byte[32];
        int A = m_socket.Receive(get);
        string IPInput = "", PortInput = "";
        string buf = System.Text.Encoding.Default.GetString(get);
        
        IPPORTProcess(buf, ref IPInput, ref PortInput);

        int intA = 0;
        int.TryParse(PortInput, out intA);

        new_socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);

        IPAddress NewServerIP = IPAddress.Parse(IPInput);

        IPEndPoint NewIPE = new IPEndPoint(NewServerIP, intA);

        new_socket.Connect(NewIPE);
        /*/
        }

        

        public Bitmap GetImage(int frameNum, int ScaleIndex, Rectangle m_showImgRect)
        {
            int[] paramsRelated = {frameNum, ScaleIndex, m_showImgRect.X, m_showImgRect.Y, m_showImgRect.Width, m_showImgRect.Height };

            string s = string.Format("{0:d},{1:d},{2:d},{3:d},{4:d},{5:d}", frameNum, ScaleIndex, m_showImgRect.X, m_showImgRect.Y, m_showImgRect.Width, m_showImgRect.Height);

            byte[] msg = Encoding.UTF8.GetBytes(s);
          
            //指令{message, frame, width, height, lefttopX, rightdownX, lefttopY, rightdownY}
            
          
            
            
            m_RightDownOnImgRated.X = m_showImgRect.X + m_showImgRect.Width * ScaleIndex;
            m_RightDownOnImgRated.Y = m_showImgRect.Y + m_showImgRect.Height * ScaleIndex;

          //    int[] temp = new int[8] { 0, 1, 682, 260, 3100, 7146, 8000, 9000 };/*{ 0, 1, m_showImgRect.Width, m_showImgRect.Height, m_showImgRect.X, m_RightDownOnImgRated.X, m_showImgRect.Y, m_RightDownOnImgRated.Y };*/
           //  int[] temp = new int[8] { 0, 1, 682, 260, 386, 2432, 8491, 9271 };
      //*******   int[] temp = new int[8] { 0, 1, 682, 260, 3550, 9710 ,5430 ,7760 };
       //     int[] temp = new int[8] {0,1,682, 260, 1000,3000,8250,9350 };
          //           int[] temp = new int[8] { 0, 1, 682, 260, 8000, 12000, 1100, 6600 };
        
           int[] temp = new int[8] { 0, 1, m_showImgRect.Width, m_showImgRect.Height, (m_showImgRect.X ) , (m_RightDownOnImgRated.X ) , (m_showImgRect.Y) , (m_RightDownOnImgRated.Y ) };  
              Console.WriteLine("This is the LOCATION you send " + m_showImgRect + m_RightDownOnImgRated);
              byte[] message = new byte[32];
              for (int i = 0; i < 8; i++)
              {
                  (System.BitConverter.GetBytes(temp[i])).CopyTo(message, i * 4);

              }
              m_socket.Send(message);


              byte[] imgData = new byte[m_showImgRect.Width * m_showImgRect.Height];
       
           int flag = m_socket.Receive(imgData,0,imgData.Length, SocketFlags.None);



           while (flag != imgData.Length)
           {
               flag += m_socket.Receive(imgData, flag, imgData.Length - flag, SocketFlags.None);
           }
            
            for(int i=1000 ; i<1050 ; i++)
                Console.WriteLine("This is the message you receive " + imgData[i]);
            
            return ConvertToBmp(m_showImgRect.Width, m_showImgRect.Height, imgData);
          
            
        }

        static Bitmap ConvertToBmp(int width, int height, byte[] imgData)
        {

            Bitmap bmp = new Bitmap(width, height, System.Drawing.Imaging.PixelFormat.Format24bppRgb);

            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            BitmapData bmpData = bmp.LockBits(rect, ImageLockMode.ReadOnly, bmp.PixelFormat);

            int bytes = Math.Abs(bmpData.Stride) * bmp.Height;

            byte[] imgData24 = new byte[bytes];

            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    byte v = imgData[y * width + x];
                    imgData24[y * bmpData.Stride + 3 * x + 0] = v;
                    imgData24[y * bmpData.Stride + 3 * x + 1] = v;
                    imgData24[y * bmpData.Stride + 3 * x + 2] = v;
                }
            }

            IntPtr ptr = bmpData.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(imgData24, 0, ptr, bytes);

            bmp.UnlockBits(bmpData);

            return bmp;
        }
    }
}
