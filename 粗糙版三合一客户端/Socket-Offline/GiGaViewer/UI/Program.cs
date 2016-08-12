using System;
using System.Collections.Generic;
using System.Linq;
using System.Windows.Forms;

namespace UI
{
    public class Mode
    {
        public int Selection = -1;

    }
    static class Program
    {
        /// <summary>
        /// 应用程序的主入口点。
        /// </summary>
        [STAThread]
        static void Main()
        {
            ImageCtrl.Mode_ImageCtrl XX = new ImageCtrl.Mode_ImageCtrl();
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new GiGaViewer());
        }
    }
}
