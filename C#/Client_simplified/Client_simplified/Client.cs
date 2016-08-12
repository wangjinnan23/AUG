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
namespace Client_simplified
{
    public partial class Client : Form
    {
        private IPEndPoint ServerInfo;
        private Socket ClientSocket;
        private object obj;
        //信息接收缓存
        private Byte[] MsgBuffer;
        //信息发送存储
        private Byte[] MsgSend;
    
        public Client()
        {
            InitializeComponent();
            ConnectServer();
            this.button1.Click += new EventHandler(button1_Click);
        }
     /// <summary>
    /// 打开客户端，即连接服务器
    /// </summary>
    private void ConnectServer()
    {
        try
        {
            ClientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            MsgBuffer = new byte[65535];
            MsgSend = new byte[65535];
            IPAddress ip = IPAddress.Parse("127.0.0.1");
            ServerInfo = new IPEndPoint(ip, Int32.Parse("3000"));
            ClientSocket.Connect(ServerInfo);
    //发送信息至服务器
            ClientSocket.Send(Encoding.Unicode.GetBytes("\r\n用户： 进入系统！" + "\r\n"));
            ClientSocket.BeginReceive(MsgBuffer, 0, MsgBuffer.Length, SocketFlags.None,
            new AsyncCallback(ReceiveCallback), null);
            this.textBox1.Text += "登录服务器成功\r\n ";
        }
        catch (System.Exception ex)
        {
    
        }
    }
    private void ReceiveCallback(IAsyncResult ar)
    {
        int rEnd = ClientSocket.EndReceive(ar);
        this.Invoke((MethodInvoker)delegate
        {
        lock (this.textBox1)
        {
            // this.textBox1.Text += Encoding.Unicode.GetString(MsgBuffer, 0, rEnd) + "\r\n";
//             this.textBox1.Text += "abc显示区域" + "\r\n";
//            this.textBox1.Text += this.textBox2.Text + "\n\r";
        }
        });
        ClientSocket.BeginReceive(MsgBuffer, 0, MsgBuffer.Length, 0, new AsyncCallback(ReceiveCallback), null);
    }

  

    private void button1_Click(object sender, EventArgs e)
    {
        this.textBox1.Text +=  "\n\r"+this.textBox2.Text + "\n\r";
         MsgSend = Encoding.Unicode.GetBytes("Client says " + this.textBox2.Text + "\n\r");
//        MsgSend = Encoding.Unicode.GetBytes("说：\n" + this.textBox2.Text + "\n\r");
        if (ClientSocket.Connected)
        {
            ClientSocket.Send(MsgSend);
        }
    }

    private void Client_Load(object sender, EventArgs e)
    {

    }

    private void textBox1_TextChanged(object sender, EventArgs e)
    {

    }
    }
}
