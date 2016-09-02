using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Management; // System.Management 命名空間 需加入參考System.Management
using System.Runtime.InteropServices;
using System.Text.RegularExpressions;

namespace UI
{
    public partial class Form2 : Form
    {

        ManagementObject objCls; // ManagementObject 類別 , 表示 WMI 執行個體。
        string strCls = "Win32_NetworkAdapterConfiguration"; // WMI 名稱空間 ( Namespace )
        string strNS = "root\\CIMV2"; // WMI 類別 (Class)
        string strIndex; // 用來記錄網路介面卡 Index
        string strPort;

        [DllImport("kernel32.dll")]
        static extern bool SetComputerName(string lpComputerName);
        [DllImport("kernel32.dll")]
        static extern bool SetComputerNameEx(_COMPUTER_NAME_FORMAT iType, string lpComputerName);


        public Form2()
        {
            InitializeComponent();
        }

        private void Form2_Load(object sender, EventArgs e)
        {
            GetAdtInfo(); // 呼叫 GetAdtInfo 程序來取得網路介面卡資訊
            txt_Old_PCName.Text = System.Windows.Forms.SystemInformation.ComputerName;

        }

        private void GetAdtInfo()
        {
            // 指定查詢網路介面卡組態 ( IPEnabled 為 True 的 )
            string strQry = "Select * from Win32_NetworkAdapterConfiguration where IPEnabled=True";

            // ManagementObjectSearcher 類別 , 根據指定的查詢擷取管理物件的集合。
            ManagementObjectSearcher objSc = new ManagementObjectSearcher(strQry);

            // 使用 Foreach 陳述式 存取集合類別中物件 (元素)
            // Get 方法 , 叫用指定的 WMI 查詢 , 並傳回產生的集合。
            foreach (ManagementObject objQry in objSc.Get())
            {
                // 取網路介面卡資訊
                ComboBox.Items.Add(objQry["Caption"]); // 將 Caption 新增至 ComboBox

            }
            ComboBox.SelectedIndex = 0;//預設選擇第一個
        }
        /*public string get_strPort() {
            if(this.strPort!=null)
                return this.strPort;
            else 

        }*/

        private void button1_Click(object sender, EventArgs e)
        {
            if (IsIPAddress(txtIP.Text))
            {
                string strIP = txtIP.Text;  // 設定IP 位址

                strPort = PortNum.Text; // Port位置
                strIndex = label7.Text; //取的要設定之網卡Index
                // 呼叫 SetNetCfg 程序 , 設定網路介面卡組態
                SetNetCfg(strIP, strPort);
            }
            else
            {
                MessageBox.Show("IP位置錯誤，請檢查！", "IP錯誤", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private void SetNetCfg(string strIP, string strPort)
        {

            objCls = new ManagementObject(strNS, strCls + ".INDEX=" + strIndex, null);

            ManagementBaseObject objInPara;

            objInPara = objCls.GetMethodParameters("EnableStatic");
            objInPara["IPAddress"] = new string[] { strIP };
            objCls.InvokeMethod("EnableStatic", objInPara, null);

            objInPara = objCls.GetMethodParameters("SetPort");
            objInPara["DefaultIPPort"] = new string[] { strPort };
            objCls.InvokeMethod("SetPort", objInPara, null);


            MessageBox.Show("IP設定完成！", "設定完成", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }



        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void ComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            GetNetInfo(ComboBox.SelectedItem.ToString());
        }

        void GetNetInfo(string strNetAdapterName)
        {
            // 指定查詢網路介面卡組態 ( IPEnabled 為 True 的 )
            string strQry = "Select * from Win32_NetworkAdapterConfiguration where IPEnabled=True";

            // ManagementObjectSearcher 類別 , 根據指定的查詢擷取管理物件的集合。
            ManagementObjectSearcher objSc = new ManagementObjectSearcher(strQry);

            // 使用 Foreach 陳述式 存取集合類別中物件 (元素)
            // Get 方法 , 叫用指定的 WMI 查詢 , 並傳回產生的集合。
            foreach (ManagementObject objQry in objSc.Get())
            {
                //判斷是否與選取網卡名稱一樣
                if (objQry["Caption"].ToString() == strNetAdapterName)
                {
                    label3.Text = objQry["MACAddress"].ToString();
                    label5.Text = objQry["ServiceName"].ToString();
                    label7.Text = objQry["Index"].ToString();

                }
            }
        }

        public static bool IsIPAddress(string ip)//驗證IP位置是否為正確
        {
            string[] arr = ip.Split('.');
            if (arr.Length != 4)
                return false;
            string pattern = @"\d{1,3}";
            for (int i = 0; i < arr.Length; i++)
            {
                string d = arr[i];
                if (i == 0 && d == "0")
                    return false;
                if (!Regex.IsMatch(d, pattern))
                    return false;
                if (d != "0")
                {
                    d = d.TrimStart('0');
                    if (d == "")
                        return false;
                    if (int.Parse(d) > 255)
                        return false;
                }
            } return true;
        }

        enum _COMPUTER_NAME_FORMAT
        {
            ComputerNameNetBIOS,
            ComputerNameDnsHostname,
            ComputerNameDnsDomain,
            ComputerNameDnsFullyQualified,
            ComputerNamePhysicalNetBIOS,
            ComputerNamePhysicalDnsHostname,
            ComputerNamePhysicalDnsDomain,
            ComputerNamePhysicalDnsFullyQualified,
            ComputerNameMax
        };

        private void comboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            GetNetInfo(ComboBox.SelectedItem.ToString());
        }

        private void ServiceName_Click(object sender, EventArgs e)
        {

        }

        private void txt_Old_PCName_Click(object sender, EventArgs e)
        {

        }

        private void label1_Click_1(object sender, EventArgs e)
        {

        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {

        }

        private void txtIP_TextChanged(object sender, EventArgs e)
        {

        }
    }
}
