namespace reachingTracking
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.timerPlot = new System.Windows.Forms.Timer(this.components);
            this.timerSync = new System.Windows.Forms.Timer(this.components);
            this.timerPause = new System.Windows.Forms.Timer(this.components);
            this.timerFirstSync = new System.Windows.Forms.Timer(this.components);
            this.SuspendLayout();
            // 
            // timerPlot
            // 
            this.timerPlot.Interval = 10;
            this.timerPlot.Tick += new System.EventHandler(this.timerPlot_Tick);
            // 
            // timerSync
            // 
            this.timerSync.Tick += new System.EventHandler(this.timerSync_Tick);
            // 
            // timerPause
            // 
            this.timerPause.Interval = 1000;
            this.timerPause.Tick += new System.EventHandler(this.timerPause_Tick);
            // 
            // timerFirstSync
            // 
            this.timerFirstSync.Interval = 500;
            this.timerFirstSync.Tick += new System.EventHandler(this.timerFirstSync_Tick);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(956, 611);
            this.Name = "MainWindow";
            this.Text = "Reaching Movements";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.MainWindow_FormClosing);
            this.Load += new System.EventHandler(this.MainWindow_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.MainWindow_Paint);
            this.MouseClick += new System.Windows.Forms.MouseEventHandler(this.MainWindow_MouseClick);
            this.ResumeLayout(false);

        }

        #endregion
        private System.Windows.Forms.Timer timerPlot;
        private System.Windows.Forms.Timer timerSync;
        private System.Windows.Forms.Timer timerPause;
        private System.Windows.Forms.Timer timerFirstSync;
    }
}

