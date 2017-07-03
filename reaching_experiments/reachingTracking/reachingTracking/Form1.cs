using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.IO;

namespace reachingTracking
{
    public partial class MainWindow : Form
    {
        private Thread threadAcq;
        private StreamWriter fileWriter;
        private StreamWriter fileHeader;
        private bool flagAcq = false;
        private Rectangle origin;
        private List<Rectangle> targets;
        private int targetCounter = 0;
        private int totalCounter = 0;
        private Rectangle syncRect;
        private bool flagSync = false;
        private bool flagStart = false;
        private bool flagPause = false;
        private const int maxTrials = 8;
        private const int numbTrialsPause = 2;
        private const int distanceTarget = 400;
        private const int originRadius = 30;
        private const int targetRadius = 30;
        private const int cursorRadius = 10;
        private const int numbTargets = 4;
        //Moving the cursor
        //0: Outside the origin
        private const int cursorOutside = 0;
        //1: Inside the origin
        private const int cursorInside = 1;
        //2: Moving from origin to target
        private const int cursorMoving = 2;

        //Cursor states
        private int cursorState = 0;

        public MainWindow()
        {
            InitializeComponent();            
        }

        private void MainWindow_Load(object sender, EventArgs e)
        {
            typeof(Form).InvokeMember("DoubleBuffered", System.Reflection.BindingFlags.SetProperty | System.Reflection.BindingFlags.Instance | System.Reflection.BindingFlags.NonPublic, null, this, new object[] { true });
            this.BackColor = Color.Black;
            this.threadAcq = new Thread(this.Acquisition);
            this.threadAcq.Priority = ThreadPriority.Normal;
            this.fileWriter = new StreamWriter("arquivoColeta.txt", false);
            this.FormBorderStyle = FormBorderStyle.None;
            this.WindowState = FormWindowState.Maximized;
            //Hides the mouse cursor
            Cursor.Hide();
            //
            //Creating the origin
            this.origin = new Rectangle(this.Width / 2 - originRadius, this.Height / 2 - originRadius, originRadius * 2, originRadius * 2);

            //Creating the targets
            this.targets = new List<Rectangle>();
            this.targets.Add(new Rectangle(this.origin.X + distanceTarget, this.origin.Y, targetRadius * 2, targetRadius * 2));
            this.targets.Add(new Rectangle(this.origin.X - distanceTarget, this.origin.Y, targetRadius * 2, targetRadius * 2));
            this.targets.Add(new Rectangle(this.origin.X, this.origin.Y + distanceTarget, targetRadius * 2, targetRadius * 2));
            this.targets.Add(new Rectangle(this.origin.X, this.origin.Y - distanceTarget, targetRadius * 2, targetRadius * 2));
            
            //Creating the synchronization rectangle
            this.syncRect = new Rectangle(0, this.Height-50, 50, 50);

            //Creates the header file
            this.fileHeader = new StreamWriter("header.txt", false);
            this.fileHeader.Write(this.Width.ToString() + "\t" + this.Height.ToString());
            this.fileHeader.Close();

            //Timer that updates the GUI
            this.timerPlot.Start();

            //First syncing            
            this.timerFirstSync.Start();
        }

        private void Acquisition()
        {
            while(this.flagAcq)
            {
                //Writes: Time X Y
                this.fileWriter.Write(DateTime.Now.ToString("hh:mm:ss.fffffff", System.Globalization.DateTimeFormatInfo.InvariantInfo) + "\t" + MousePosition.X.ToString() + "\t" + MousePosition.Y.ToString() + "\t");
                //If sync is false - 0, else 1
                //Depends on the color of the rectangle used for syncing with EEG
                if (this.flagSync)
                    this.fileWriter.Write("1\n");
                else
                    this.fileWriter.Write("0\n");                                                        
                Thread.Sleep(1); //keep the sampling rate of 1kHz
            }
        }

        //Paints the GUI
        private void MainWindow_Paint(object sender, PaintEventArgs e)
        {
            //Origin
            if (!this.flagPause)
            {
                Pen porigin = new Pen(Color.Red, 1);
                e.Graphics.DrawRectangle(porigin, origin);
            }

            //Targets
            Pen ptargets = new Pen(Color.Green, 1);
            for (int i = 0; i < numbTargets; i++)
                e.Graphics.DrawEllipse(ptargets, this.targets[i]);

            //Cursor
            Pen p = new Pen(Color.Blue, 1);
            e.Graphics.DrawEllipse(p, MousePosition.X - cursorRadius, MousePosition.Y - cursorRadius, cursorRadius * 2, cursorRadius * 2);

            if (this.flagStart && !this.flagPause)
            {
                //Updating cursor status
                if (this.cursorState == cursorOutside) //Cursor outside the origin
                {
                    if (Cursor.Position.X >= origin.X && Cursor.Position.X <= origin.X + origin.Width && Cursor.Position.Y >= origin.Y && Cursor.Position.Y <= origin.Y + origin.Height)
                    {
                        this.cursorState = cursorInside;
                        if (totalCounter == maxTrials)
                        {
                            this.flagAcq = false;
                            this.flagStart = false;
                            this.fileWriter.Close();
                            MessageBox.Show("Seu experimento acabou!", "Aviso", MessageBoxButtons.OK, MessageBoxIcon.Information);
                        }
                        else if (targetCounter == numbTrialsPause)
                        {
                            this.timerPause.Start();
                            this.targetCounter = 0;
                            this.flagPause = true;
                        }                        
                    }
                }
                else if (this.cursorState == cursorInside) //Cursor inside the origin
                {
                    if (!(Cursor.Position.X >= origin.X && Cursor.Position.X <= origin.X + origin.Width && Cursor.Position.Y >= origin.Y && Cursor.Position.Y <= origin.Y + origin.Height))
                    {
                        this.cursorState = cursorMoving;
                        this.flagSync = true;
                        this.timerSync.Start();
                    }
                }
                else if (this.cursorState == cursorMoving) //Cursor moving
                {
                    //Checks if the cursor has hit any target
                    for (int i = 0; i < numbTargets; i++)
                    {
                        if (Cursor.Position.X >= this.targets[i].X && Cursor.Position.X <= this.targets[i].X + this.targets[i].Width && Cursor.Position.Y >= this.targets[i].Y && Cursor.Position.Y <= this.targets[i].Y + this.targets[i].Height)
                        {
                            //When the cursor hits the target, the cursor state resets
                            this.cursorState = cursorOutside;
                            //Increments the target counter
                            this.targetCounter++;
                            //Increments the total counter
                            this.totalCounter++;                          
                        }
                    }
                }
            }

            //Sync 
            Pen psync;
            if(this.flagSync)
                psync = new Pen(Color.White, 100);
            else
                psync = new Pen(Color.Black, 100);
            e.Graphics.DrawRectangle(psync, syncRect);
        }

        private void timerPlot_Tick(object sender, EventArgs e)
        {            
            this.Refresh();
        }

        private void timerSync_Tick(object sender, EventArgs e)
        {
            this.timerSync.Stop();
            this.flagSync = false;            
        }

        private void MainWindow_MouseClick(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
            {
                if (this.flagStart == false)
                {
                    this.threadAcq.Start();
                    this.flagAcq = true;
                    this.flagStart = true;
                }
                else
                {
                    this.flagAcq = false;
                    this.flagStart = false;
                    this.fileWriter.Close();                    
                }
            }
        }

        private void timerPause_Tick(object sender, EventArgs e)
        {            
            this.timerPause.Stop();
            this.flagPause = false;
        }

        //Kills the thread if it is still running
        private void MainWindow_FormClosing(object sender, FormClosingEventArgs e)
        {
            this.flagAcq = false;
            this.fileWriter.Close();
        }

        private void timerFirstSync_Tick(object sender, EventArgs e)
        {
            this.timerFirstSync.Stop();
            this.flagSync = true;
            this.timerSync.Start();
        }
    }
}
