namespace UI
{
    partial class GiGaViewer
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

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(GiGaViewer));
            this.FastReverse = new System.Windows.Forms.Button();
            this.Play = new System.Windows.Forms.Button();
            this.trackBar = new System.Windows.Forms.TrackBar();
            this.FastForward = new System.Windows.Forms.Button();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.文件ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.打开文件ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.图片另存为ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.保存全局图SToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.文件信息ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.查看ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.全屏ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.缩放ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.altF1ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.altF2ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.altF3ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.自动适应ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.前端显示ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.默认ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.总是ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.播放时ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.选项ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.播放ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.播放暂停PToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.停止ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.快进ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.快退BToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.工具TToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.截屏SToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.设定帧率QToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.iPSetting = new System.Windows.Forms.ToolStripMenuItem();
            this.帮助ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.关于ToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.NextOne = new System.Windows.Forms.Button();
            this.LastOne = new System.Windows.Forms.Button();
            this.OpenButton = new System.Windows.Forms.Button();
            this.ScreenShot = new System.Windows.Forms.Button();
            this.FullScreen = new System.Windows.Forms.Button();
            this.OpenPanaroma = new System.Windows.Forms.Button();
            this.Stop = new System.Windows.Forms.Button();
            this.SetOptions = new System.Windows.Forms.Button();
            this.Plot = new System.Windows.Forms.Button();
            this.pause = new System.Windows.Forms.Button();
            this.toolTip = new System.Windows.Forms.ToolTip(this.components);
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.imageCtrl1 = new ImageCtrl.ImageCtrl();
            this.button3 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // FastReverse
            // 
            this.FastReverse.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.FastReverse.BackColor = System.Drawing.Color.Transparent;
            this.FastReverse.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.FastReverse.FlatAppearance.BorderSize = 0;
            this.FastReverse.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.FastReverse.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.FastReverse.Image = ((System.Drawing.Image)(resources.GetObject("FastReverse.Image")));
            this.FastReverse.Location = new System.Drawing.Point(334, 349);
            this.FastReverse.Name = "FastReverse";
            this.FastReverse.Size = new System.Drawing.Size(40, 40);
            this.FastReverse.TabIndex = 3;
            this.toolTip.SetToolTip(this.FastReverse, "快退");
            this.FastReverse.UseVisualStyleBackColor = false;
            // 
            // Play
            // 
            this.Play.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Play.BackColor = System.Drawing.Color.Transparent;
            this.Play.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.Play.FlatAppearance.BorderSize = 0;
            this.Play.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.Play.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Play.Image = ((System.Drawing.Image)(resources.GetObject("Play.Image")));
            this.Play.Location = new System.Drawing.Point(208, 349);
            this.Play.Name = "Play";
            this.Play.Size = new System.Drawing.Size(40, 40);
            this.Play.TabIndex = 4;
            this.toolTip.SetToolTip(this.Play, "播放");
            this.Play.UseVisualStyleBackColor = false;
            this.Play.Click += new System.EventHandler(this.Play_Click);
            // 
            // trackBar
            // 
            this.trackBar.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.trackBar.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.trackBar.Location = new System.Drawing.Point(5, 303);
            this.trackBar.Maximum = 1000;
            this.trackBar.Name = "trackBar";
            this.trackBar.Size = new System.Drawing.Size(693, 45);
            this.trackBar.TabIndex = 5;
            this.trackBar.TickFrequency = 5;
            this.trackBar.TickStyle = System.Windows.Forms.TickStyle.Both;
            // 
            // FastForward
            // 
            this.FastForward.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.FastForward.BackColor = System.Drawing.Color.Transparent;
            this.FastForward.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.FastForward.FlatAppearance.BorderSize = 0;
            this.FastForward.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.FastForward.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.FastForward.Image = ((System.Drawing.Image)(resources.GetObject("FastForward.Image")));
            this.FastForward.Location = new System.Drawing.Point(385, 349);
            this.FastForward.Name = "FastForward";
            this.FastForward.Size = new System.Drawing.Size(40, 40);
            this.FastForward.TabIndex = 6;
            this.toolTip.SetToolTip(this.FastForward, "快进");
            this.FastForward.UseVisualStyleBackColor = false;
            // 
            // menuStrip1
            // 
            this.menuStrip1.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.menuStrip1.Dock = System.Windows.Forms.DockStyle.None;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.文件ToolStripMenuItem,
            this.查看ToolStripMenuItem,
            this.播放ToolStripMenuItem,
            this.工具TToolStripMenuItem,
            this.帮助ToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(305, 25);
            this.menuStrip1.TabIndex = 8;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // 文件ToolStripMenuItem
            // 
            this.文件ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.打开文件ToolStripMenuItem,
            this.图片另存为ToolStripMenuItem,
            this.保存全局图SToolStripMenuItem,
            this.文件信息ToolStripMenuItem});
            this.文件ToolStripMenuItem.Name = "文件ToolStripMenuItem";
            this.文件ToolStripMenuItem.Size = new System.Drawing.Size(58, 21);
            this.文件ToolStripMenuItem.Text = "文件(F)";
            // 
            // 打开文件ToolStripMenuItem
            // 
            this.打开文件ToolStripMenuItem.Name = "打开文件ToolStripMenuItem";
            this.打开文件ToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.打开文件ToolStripMenuItem.Text = "打开文件(O)";
            // 
            // 图片另存为ToolStripMenuItem
            // 
            this.图片另存为ToolStripMenuItem.Name = "图片另存为ToolStripMenuItem";
            this.图片另存为ToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.图片另存为ToolStripMenuItem.Text = "保存图像(I)";
            // 
            // 保存全局图SToolStripMenuItem
            // 
            this.保存全局图SToolStripMenuItem.Name = "保存全局图SToolStripMenuItem";
            this.保存全局图SToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.保存全局图SToolStripMenuItem.Text = "保存全局图(S)";
            // 
            // 文件信息ToolStripMenuItem
            // 
            this.文件信息ToolStripMenuItem.Name = "文件信息ToolStripMenuItem";
            this.文件信息ToolStripMenuItem.Size = new System.Drawing.Size(151, 22);
            this.文件信息ToolStripMenuItem.Text = "文件属性(R)";
            // 
            // 查看ToolStripMenuItem
            // 
            this.查看ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.全屏ToolStripMenuItem,
            this.缩放ToolStripMenuItem,
            this.前端显示ToolStripMenuItem,
            this.选项ToolStripMenuItem});
            this.查看ToolStripMenuItem.Name = "查看ToolStripMenuItem";
            this.查看ToolStripMenuItem.Size = new System.Drawing.Size(60, 21);
            this.查看ToolStripMenuItem.Text = "查看(V)";
            // 
            // 全屏ToolStripMenuItem
            // 
            this.全屏ToolStripMenuItem.Name = "全屏ToolStripMenuItem";
            this.全屏ToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.全屏ToolStripMenuItem.Text = "全屏(U)";
            // 
            // 缩放ToolStripMenuItem
            // 
            this.缩放ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.altF1ToolStripMenuItem,
            this.altF2ToolStripMenuItem,
            this.altF3ToolStripMenuItem,
            this.自动适应ToolStripMenuItem});
            this.缩放ToolStripMenuItem.Name = "缩放ToolStripMenuItem";
            this.缩放ToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.缩放ToolStripMenuItem.Text = "缩放(Z)";
            // 
            // altF1ToolStripMenuItem
            // 
            this.altF1ToolStripMenuItem.Name = "altF1ToolStripMenuItem";
            this.altF1ToolStripMenuItem.Size = new System.Drawing.Size(178, 22);
            this.altF1ToolStripMenuItem.Text = "50%        Alt + F1";
            // 
            // altF2ToolStripMenuItem
            // 
            this.altF2ToolStripMenuItem.Name = "altF2ToolStripMenuItem";
            this.altF2ToolStripMenuItem.Size = new System.Drawing.Size(178, 22);
            this.altF2ToolStripMenuItem.Text = "100%      Alt + F2";
            // 
            // altF3ToolStripMenuItem
            // 
            this.altF3ToolStripMenuItem.Name = "altF3ToolStripMenuItem";
            this.altF3ToolStripMenuItem.Size = new System.Drawing.Size(178, 22);
            this.altF3ToolStripMenuItem.Text = "200%      Alt + F3";
            // 
            // 自动适应ToolStripMenuItem
            // 
            this.自动适应ToolStripMenuItem.Name = "自动适应ToolStripMenuItem";
            this.自动适应ToolStripMenuItem.Size = new System.Drawing.Size(178, 22);
            this.自动适应ToolStripMenuItem.Text = "自动适应 Alt + F4";
            // 
            // 前端显示ToolStripMenuItem
            // 
            this.前端显示ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.默认ToolStripMenuItem,
            this.总是ToolStripMenuItem,
            this.播放时ToolStripMenuItem});
            this.前端显示ToolStripMenuItem.Name = "前端显示ToolStripMenuItem";
            this.前端显示ToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.前端显示ToolStripMenuItem.Text = "前端显示(T)";
            // 
            // 默认ToolStripMenuItem
            // 
            this.默认ToolStripMenuItem.Name = "默认ToolStripMenuItem";
            this.默认ToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
            this.默认ToolStripMenuItem.Text = "默认(D)";
            // 
            // 总是ToolStripMenuItem
            // 
            this.总是ToolStripMenuItem.Name = "总是ToolStripMenuItem";
            this.总是ToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
            this.总是ToolStripMenuItem.Text = "总是(A)";
            // 
            // 播放时ToolStripMenuItem
            // 
            this.播放时ToolStripMenuItem.Name = "播放时ToolStripMenuItem";
            this.播放时ToolStripMenuItem.Size = new System.Drawing.Size(127, 22);
            this.播放时ToolStripMenuItem.Text = "播放时(T)";
            // 
            // 选项ToolStripMenuItem
            // 
            this.选项ToolStripMenuItem.Name = "选项ToolStripMenuItem";
            this.选项ToolStripMenuItem.Size = new System.Drawing.Size(139, 22);
            this.选项ToolStripMenuItem.Text = "选项(O)...";
            // 
            // 播放ToolStripMenuItem
            // 
            this.播放ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.播放暂停PToolStripMenuItem,
            this.停止ToolStripMenuItem,
            this.快进ToolStripMenuItem,
            this.快退BToolStripMenuItem});
            this.播放ToolStripMenuItem.Name = "播放ToolStripMenuItem";
            this.播放ToolStripMenuItem.Size = new System.Drawing.Size(59, 21);
            this.播放ToolStripMenuItem.Text = "播放(P)";
            // 
            // 播放暂停PToolStripMenuItem
            // 
            this.播放暂停PToolStripMenuItem.Name = "播放暂停PToolStripMenuItem";
            this.播放暂停PToolStripMenuItem.Size = new System.Drawing.Size(144, 22);
            this.播放暂停PToolStripMenuItem.Text = "播放/暂停(P)";
            // 
            // 停止ToolStripMenuItem
            // 
            this.停止ToolStripMenuItem.Name = "停止ToolStripMenuItem";
            this.停止ToolStripMenuItem.Size = new System.Drawing.Size(144, 22);
            this.停止ToolStripMenuItem.Text = "停止(S)";
            // 
            // 快进ToolStripMenuItem
            // 
            this.快进ToolStripMenuItem.Name = "快进ToolStripMenuItem";
            this.快进ToolStripMenuItem.Size = new System.Drawing.Size(144, 22);
            this.快进ToolStripMenuItem.Text = "快进(R)";
            // 
            // 快退BToolStripMenuItem
            // 
            this.快退BToolStripMenuItem.Name = "快退BToolStripMenuItem";
            this.快退BToolStripMenuItem.Size = new System.Drawing.Size(144, 22);
            this.快退BToolStripMenuItem.Text = "快退(B)";
            // 
            // 工具TToolStripMenuItem
            // 
            this.工具TToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.截屏SToolStripMenuItem,
            this.设定帧率QToolStripMenuItem,
            this.iPSetting});
            this.工具TToolStripMenuItem.Name = "工具TToolStripMenuItem";
            this.工具TToolStripMenuItem.Size = new System.Drawing.Size(59, 21);
            this.工具TToolStripMenuItem.Text = "工具(T)";
            // 
            // 截屏SToolStripMenuItem
            // 
            this.截屏SToolStripMenuItem.Name = "截屏SToolStripMenuItem";
            this.截屏SToolStripMenuItem.Size = new System.Drawing.Size(142, 22);
            this.截屏SToolStripMenuItem.Text = "截屏(S)";
            // 
            // 设定帧率QToolStripMenuItem
            // 
            this.设定帧率QToolStripMenuItem.Name = "设定帧率QToolStripMenuItem";
            this.设定帧率QToolStripMenuItem.Size = new System.Drawing.Size(142, 22);
            this.设定帧率QToolStripMenuItem.Text = "设定帧率(Q)";
            // 
            // iPSetting
            // 
            this.iPSetting.Name = "iPSetting";
            this.iPSetting.Size = new System.Drawing.Size(142, 22);
            this.iPSetting.Text = "IP Setteing";
            this.iPSetting.Click += new System.EventHandler(this.iPSetting_Click);
            // 
            // 帮助ToolStripMenuItem
            // 
            this.帮助ToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.关于ToolStripMenuItem});
            this.帮助ToolStripMenuItem.Name = "帮助ToolStripMenuItem";
            this.帮助ToolStripMenuItem.Size = new System.Drawing.Size(61, 21);
            this.帮助ToolStripMenuItem.Text = "帮助(H)";
            // 
            // 关于ToolStripMenuItem
            // 
            this.关于ToolStripMenuItem.Name = "关于ToolStripMenuItem";
            this.关于ToolStripMenuItem.Size = new System.Drawing.Size(125, 22);
            this.关于ToolStripMenuItem.Text = "关于(A)...";
            // 
            // NextOne
            // 
            this.NextOne.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.NextOne.BackColor = System.Drawing.Color.Transparent;
            this.NextOne.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.NextOne.FlatAppearance.BorderSize = 0;
            this.NextOne.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.NextOne.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.NextOne.Image = ((System.Drawing.Image)(resources.GetObject("NextOne.Image")));
            this.NextOne.Location = new System.Drawing.Point(434, 349);
            this.NextOne.Name = "NextOne";
            this.NextOne.Size = new System.Drawing.Size(40, 40);
            this.NextOne.TabIndex = 9;
            this.toolTip.SetToolTip(this.NextOne, "下一个文件");
            this.NextOne.UseVisualStyleBackColor = false;
            // 
            // LastOne
            // 
            this.LastOne.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.LastOne.BackColor = System.Drawing.Color.Transparent;
            this.LastOne.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.LastOne.FlatAppearance.BorderSize = 0;
            this.LastOne.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.LastOne.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.LastOne.Image = ((System.Drawing.Image)(resources.GetObject("LastOne.Image")));
            this.LastOne.Location = new System.Drawing.Point(284, 349);
            this.LastOne.Name = "LastOne";
            this.LastOne.Size = new System.Drawing.Size(40, 40);
            this.LastOne.TabIndex = 10;
            this.toolTip.SetToolTip(this.LastOne, "上一个文件");
            this.LastOne.UseVisualStyleBackColor = false;
            // 
            // OpenButton
            // 
            this.OpenButton.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.OpenButton.BackColor = System.Drawing.Color.Transparent;
            this.OpenButton.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.OpenButton.FlatAppearance.BorderSize = 0;
            this.OpenButton.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.OpenButton.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.OpenButton.Image = ((System.Drawing.Image)(resources.GetObject("OpenButton.Image")));
            this.OpenButton.Location = new System.Drawing.Point(5, 349);
            this.OpenButton.Name = "OpenButton";
            this.OpenButton.Size = new System.Drawing.Size(40, 40);
            this.OpenButton.TabIndex = 1;
            this.toolTip.SetToolTip(this.OpenButton, "打开");
            this.OpenButton.UseVisualStyleBackColor = false;
            this.OpenButton.Click += new System.EventHandler(this.OpenButton_Click);
            // 
            // ScreenShot
            // 
            this.ScreenShot.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.ScreenShot.BackColor = System.Drawing.Color.Transparent;
            this.ScreenShot.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.ScreenShot.FlatAppearance.BorderSize = 0;
            this.ScreenShot.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.ScreenShot.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.ScreenShot.Image = ((System.Drawing.Image)(resources.GetObject("ScreenShot.Image")));
            this.ScreenShot.Location = new System.Drawing.Point(597, 349);
            this.ScreenShot.Name = "ScreenShot";
            this.ScreenShot.Size = new System.Drawing.Size(40, 40);
            this.ScreenShot.TabIndex = 12;
            this.toolTip.SetToolTip(this.ScreenShot, "截图");
            this.ScreenShot.UseVisualStyleBackColor = false;
            this.ScreenShot.Click += new System.EventHandler(this.ScreenShot_Click);
            // 
            // FullScreen
            // 
            this.FullScreen.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.FullScreen.BackColor = System.Drawing.Color.Transparent;
            this.FullScreen.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.FullScreen.FlatAppearance.BorderSize = 0;
            this.FullScreen.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.FullScreen.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.FullScreen.Image = ((System.Drawing.Image)(resources.GetObject("FullScreen.Image")));
            this.FullScreen.Location = new System.Drawing.Point(105, 349);
            this.FullScreen.Name = "FullScreen";
            this.FullScreen.Size = new System.Drawing.Size(40, 40);
            this.FullScreen.TabIndex = 13;
            this.toolTip.SetToolTip(this.FullScreen, "全屏");
            this.FullScreen.UseVisualStyleBackColor = false;
            this.FullScreen.Click += new System.EventHandler(this.FullScreen_Click);
            // 
            // OpenPanaroma
            // 
            this.OpenPanaroma.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
            this.OpenPanaroma.BackColor = System.Drawing.Color.Transparent;
            this.OpenPanaroma.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.OpenPanaroma.FlatAppearance.BorderSize = 0;
            this.OpenPanaroma.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.OpenPanaroma.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.OpenPanaroma.Image = ((System.Drawing.Image)(resources.GetObject("OpenPanaroma.Image")));
            this.OpenPanaroma.Location = new System.Drawing.Point(55, 349);
            this.OpenPanaroma.Name = "OpenPanaroma";
            this.OpenPanaroma.Size = new System.Drawing.Size(40, 40);
            this.OpenPanaroma.TabIndex = 11;
            this.toolTip.SetToolTip(this.OpenPanaroma, "小地图");
            this.OpenPanaroma.UseVisualStyleBackColor = false;
            this.OpenPanaroma.Click += new System.EventHandler(this.OpenPanaroma_Click);
            // 
            // Stop
            // 
            this.Stop.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.Stop.BackColor = System.Drawing.Color.Transparent;
            this.Stop.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.Stop.FlatAppearance.BorderSize = 0;
            this.Stop.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.Stop.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Stop.Image = ((System.Drawing.Image)(resources.GetObject("Stop.Image")));
            this.Stop.Location = new System.Drawing.Point(167, 349);
            this.Stop.Name = "Stop";
            this.Stop.Size = new System.Drawing.Size(40, 40);
            this.Stop.TabIndex = 14;
            this.toolTip.SetToolTip(this.Stop, "停止");
            this.Stop.UseVisualStyleBackColor = false;
            // 
            // SetOptions
            // 
            this.SetOptions.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.SetOptions.BackColor = System.Drawing.Color.Transparent;
            this.SetOptions.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.SetOptions.FlatAppearance.BorderSize = 0;
            this.SetOptions.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.SetOptions.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.SetOptions.Image = ((System.Drawing.Image)(resources.GetObject("SetOptions.Image")));
            this.SetOptions.Location = new System.Drawing.Point(647, 349);
            this.SetOptions.Name = "SetOptions";
            this.SetOptions.Size = new System.Drawing.Size(40, 40);
            this.SetOptions.TabIndex = 15;
            this.toolTip.SetToolTip(this.SetOptions, "设置");
            this.SetOptions.UseVisualStyleBackColor = false;
            // 
            // Plot
            // 
            this.Plot.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.Plot.BackColor = System.Drawing.Color.Transparent;
            this.Plot.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.Plot.FlatAppearance.BorderSize = 0;
            this.Plot.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.Plot.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.Plot.Image = ((System.Drawing.Image)(resources.GetObject("Plot.Image")));
            this.Plot.Location = new System.Drawing.Point(548, 349);
            this.Plot.Name = "Plot";
            this.Plot.Size = new System.Drawing.Size(40, 40);
            this.Plot.TabIndex = 16;
            this.toolTip.SetToolTip(this.Plot, "标注");
            this.Plot.UseVisualStyleBackColor = false;
            this.Plot.Click += new System.EventHandler(this.Plot_Click);
            // 
            // pause
            // 
            this.pause.Anchor = System.Windows.Forms.AnchorStyles.Bottom;
            this.pause.BackColor = System.Drawing.Color.Transparent;
            this.pause.BackgroundImageLayout = System.Windows.Forms.ImageLayout.Center;
            this.pause.FlatAppearance.BorderSize = 0;
            this.pause.FlatAppearance.MouseDownBackColor = System.Drawing.SystemColors.ActiveBorder;
            this.pause.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.pause.Image = ((System.Drawing.Image)(resources.GetObject("pause.Image")));
            this.pause.Location = new System.Drawing.Point(242, 349);
            this.pause.Name = "pause";
            this.pause.Size = new System.Drawing.Size(40, 40);
            this.pause.TabIndex = 17;
            this.toolTip.SetToolTip(this.pause, "暂停");
            this.pause.UseVisualStyleBackColor = false;
            this.pause.Click += new System.EventHandler(this.pause_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(480, 340);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(62, 23);
            this.button1.TabIndex = 18;
            this.button1.Text = "离线观看";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(480, 366);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(62, 23);
            this.button2.TabIndex = 19;
            this.button2.Text = "在线观看";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // imageCtrl1
            // 
            this.imageCtrl1.AllowDrop = true;
            this.imageCtrl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.imageCtrl1.Cursor = System.Windows.Forms.Cursors.Hand;
            this.imageCtrl1.Location = new System.Drawing.Point(0, 25);
            this.imageCtrl1.Margin = new System.Windows.Forms.Padding(4);
            this.imageCtrl1.Name = "imageCtrl1";
            this.imageCtrl1.Size = new System.Drawing.Size(700, 278);
            this.imageCtrl1.TabIndex = 0;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(548, 340);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(75, 23);
            this.button3.TabIndex = 20;
            this.button3.Text = "离线拼接";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // GiGaViewer
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ButtonFace;
            this.ClientSize = new System.Drawing.Size(700, 394);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.pause);
            this.Controls.Add(this.Plot);
            this.Controls.Add(this.imageCtrl1);
            this.Controls.Add(this.SetOptions);
            this.Controls.Add(this.Stop);
            this.Controls.Add(this.FullScreen);
            this.Controls.Add(this.ScreenShot);
            this.Controls.Add(this.OpenPanaroma);
            this.Controls.Add(this.LastOne);
            this.Controls.Add(this.NextOne);
            this.Controls.Add(this.FastForward);
            this.Controls.Add(this.Play);
            this.Controls.Add(this.FastReverse);
            this.Controls.Add(this.OpenButton);
            this.Controls.Add(this.trackBar);
            this.Controls.Add(this.menuStrip1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "GiGaViewer";
            this.Text = "GiGaViewer";
            this.Resize += new System.EventHandler(this.GiGaViewer_Resize);
            ((System.ComponentModel.ISupportInitialize)(this.trackBar)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private ImageCtrl.ImageCtrl imageCtrl1;
        private System.Windows.Forms.Button FastReverse;
        private System.Windows.Forms.Button Play;
        private System.Windows.Forms.TrackBar trackBar;
        private System.Windows.Forms.Button FastForward;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem 文件ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 查看ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 播放ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 工具TToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 帮助ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 打开文件ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 图片另存为ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 保存全局图SToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 文件信息ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 播放暂停PToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 停止ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 快进ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 全屏ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 缩放ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem altF1ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem altF2ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem altF3ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 自动适应ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 快退BToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 前端显示ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 默认ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 总是ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 播放时ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 选项ToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 截屏SToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 设定帧率QToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem 关于ToolStripMenuItem;
        private System.Windows.Forms.Button NextOne;
        private System.Windows.Forms.Button LastOne;
        private System.Windows.Forms.Button OpenButton;
        private System.Windows.Forms.Button ScreenShot;
        private System.Windows.Forms.Button FullScreen;
        private System.Windows.Forms.Button OpenPanaroma;
        private System.Windows.Forms.Button Stop;
        private System.Windows.Forms.Button SetOptions;
        private System.Windows.Forms.Button Plot;
        private System.Windows.Forms.Button pause;
        private System.Windows.Forms.ToolTip toolTip;
        private System.Windows.Forms.ToolStripMenuItem iPSetting;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;

    }
}

