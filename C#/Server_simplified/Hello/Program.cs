using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;
   
namespace Hello
{
    public class A
    {
        public static int aa = 1;
        public static int clientNum=0;
        public class B
        {
            public static int bb = 2;
        }
    }
   
    static class Program
    {
      
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            A.aa = 1;
            
           
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new Form1());
        }
    }
}
