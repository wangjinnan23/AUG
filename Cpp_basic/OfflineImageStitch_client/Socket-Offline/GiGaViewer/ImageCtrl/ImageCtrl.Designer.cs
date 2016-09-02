namespace ImageCtrl
{
    partial class ImageCtrl
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

        #region 组件设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.vScrollBarImg = new System.Windows.Forms.VScrollBar();
            this.hScrollBarImg = new System.Windows.Forms.HScrollBar();
            this.SuspendLayout();
            // 
            // vScrollBarImg
            // 
            this.vScrollBarImg.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.vScrollBarImg.Location = new System.Drawing.Point(1280, 0);
            this.vScrollBarImg.Name = "vScrollBarImg";
            this.vScrollBarImg.Size = new System.Drawing.Size(17, 1280);
            this.vScrollBarImg.TabIndex = 0;
            this.vScrollBarImg.Scroll += new System.Windows.Forms.ScrollEventHandler(this.vScrollBarImg_Scroll);
            // 
            // hScrollBarImg
            // 
            this.hScrollBarImg.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.hScrollBarImg.Location = new System.Drawing.Point(0, 1280);
            this.hScrollBarImg.Name = "hScrollBarImg";
            this.hScrollBarImg.Size = new System.Drawing.Size(1280, 17);
            this.hScrollBarImg.TabIndex = 1;
            this.hScrollBarImg.Scroll += new System.Windows.Forms.ScrollEventHandler(this.hScrollBarImg_Scroll);
            // 
            // ImageCtrl
            // 
            this.AllowDrop = true;
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.hScrollBarImg);
            this.Controls.Add(this.vScrollBarImg);
            this.Cursor = System.Windows.Forms.Cursors.Hand;
            this.Name = "ImageCtrl";
            this.Size = new System.Drawing.Size(1297, 1297);
            this.Load += new System.EventHandler(this.ImageCtrl_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.ImageCtrl_Paint);
            //this.MouseClick += new System.Windows.Forms.MouseEventHandler(this.ImageCtrl_MouseClick);
            //this.MouseDoubleClick += new System.Windows.Forms.MouseEventHandler(this.ImageCtrl_MouseDoubleClick);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.ImageCtrl_MouseDown);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.ImageCtrl_MouseMove);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.ImageCtrl_MouseUp);
            this.Resize += new System.EventHandler(this.ImageCtrl_Resize);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.VScrollBar vScrollBarImg;
        private System.Windows.Forms.HScrollBar hScrollBarImg;
    }
}
