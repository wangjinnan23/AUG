using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using System.Net;
using System.Net.Sockets;

using System.Threading;
using System.Xml;
namespace Hello
{
    public partial class Form1 : Form
    {
        private IPEndPoint ServerInfo;//存放服务器的IP和端口信息
        private Socket ServerSocket;//服务端运行的SOCKET
        private Thread ServerThread;//服务端运行的线程
        private Socket[] ClientSocket;//为客户端建立的SOCKET连接
        private int ClientNumb;//存放客户端数量
        private byte[] MsgBuffer;//存放消息数据
        private object obj;
 
       
        public Form1()
        { 
            InitializeComponent();
            ListenClient();
        }
        private void ListenClient()
        {
            try
            {
                A.aa = 1;
                ServerSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                IPAddress ip = IPAddress.Parse("127.0.0.1");
                ServerInfo = new IPEndPoint(ip, Int32.Parse("3000"));
                ServerSocket.Bind(ServerInfo);
                ServerSocket.Listen(10);

                ClientSocket = new Socket[65535];
                MsgBuffer = new byte[65535];
                ClientNumb = 0;

                ServerThread = new Thread(new ThreadStart(RecieveAccept));
                ServerThread.Start();
            }
            catch (System.Exception ex)
            {

            }
        }
        /// <summary>
        /// 添加阻塞，监听客户端
        /// </summary>
        private void RecieveAccept()
        {
            this.textBox1.Text += "该服务器准备客户端接入";
            while (true)
            {
                //等待接受客户端连接，如果有就执行下边代码，没有就阻塞
                ClientSocket[ClientNumb] = ServerSocket.Accept();
                //接受客户端信息，没有阻塞，则会执行下边输出的代码；如果是Receive则不会执行下边输出代码
                ClientSocket[ClientNumb].BeginReceive(MsgBuffer, 0, MsgBuffer.Length, SocketFlags.None,
                new AsyncCallback(ReceiveCallback), ClientSocket[ClientNumb]);
                this.Invoke((MethodInvoker)delegate
                {
                    lock (this.textBox1)
                        this.textBox1.Text +="\r\n"+"客户端：" + ClientNumb.ToString() + "连接成功！" + A.aa.ToString();// +"\r\n";
                });
                ClientNumb++; A.aa += 100;
            }
        }
        /// <summary>
        /// 回发数据到客户端
        /// </summary>
        /// <param name="ar"></param>
        private void ReceiveCallback(IAsyncResult ar)
        {
            try
            {
                Socket rSocket = (Socket)ar.AsyncState;
                int rEnd = rSocket.EndReceive(ar);

                for (int i = 0; i < ClientNumb; i++)
                {
                    if (ClientSocket[i].Connected)
                    {
                        //发送数据到客户端
                        ClientSocket[i].Send(MsgBuffer, 0, rEnd, SocketFlags.None);
                    }

                    //同时接受客户端回发的数据，用于回发
                    rSocket.BeginReceive(MsgBuffer, 0, MsgBuffer.Length, 0, new AsyncCallback(ReceiveCallback), rSocket);
                }
            }
            catch (System.Exception ex)
            {

            }
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void textBox3_TextChanged(object sender, EventArgs e)
        {

        }
    }



}