using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Threading;
using System.Diagnostics;
using System.Windows.Forms.DataVisualization.Charting;
using XBee;


namespace RocketGUI
{
    public partial class MainForm : Form
    {
        private Thread cpuThread;
        private Thread radioThread;
        private Thread plotRadioThread;
        private double[] cpuArray = new double[30];
        private int[] sensorReadArray = new int[30];
        Radio radio = new Radio();
        Chart[] charts;

        public MainForm()
        {
            InitializeComponent();

            //Initialize Charts
            Chart[] charts = new Chart[] { chart1, chart3, chart2, chart4, chart5, chart6 };

            foreach(Chart chart in charts)
            {
                chart.Series["Series1"].Points.Add(0);
                chart.ChartAreas[0].AxisX.ScrollBar.Enabled = true;
            }
        }

        private void getPerformanceCounters()
        {
            var cpuPerfCount = new PerformanceCounter("Processor Information", "% Processor Time", "_Total");

            while (true)
            {
                cpuArray[cpuArray.Length - 1] = Math.Round(cpuPerfCount.NextValue(), 0);

                Array.Copy(cpuArray, 1, cpuArray, 0, cpuArray.Length - 1);

                if (chart1.IsHandleCreated)
                {
                    this.Invoke((MethodInvoker)delegate { UpdateCpuChart(); });

                }
                else
                {

                }

                Thread.Sleep(100);
            }
        }

        private void UpdateCpuChart()
        {
            chart1.Series["Series1"].Points.Clear();

            for (int i = 0; i < cpuArray.Length - 1; ++i)
            {
                chart1.Series["Series1"].Points.Add(cpuArray[i]);
            }
        }

        private void plotRadio()
        {
            while (true)
            {
                if (chart3.IsHandleCreated && chart2.IsHandleCreated)
                {
                    this.Invoke((MethodInvoker)delegate { updateRadioChart(); });

                }
                Thread.Sleep(100);
            }
        }


        private void updateRadioChart()
        {
            chart2.Series["Series1"].Points.Clear();
            chart3.Series["Series1"].Points.Clear();

            for (int i = radio.radioData.Count > 50 ? radio.radioData.Count - 50 : 0 ; i< radio.radioData.Count - 1; ++i)
            {
                chart2.Series["Series1"].Points.Add(radio.radioData[i].tc1);
                chart3.Series["Series1"].Points.Add(radio.radioData[i].pt1);
            }
        }

        private void radioRead()
        {
            radio.readData("mockRadio.py","");
        }

        private void button1_Click(object sender, EventArgs e)
        {
            cpuThread = new Thread(new ThreadStart(this.getPerformanceCounters));
            cpuThread.IsBackground = true;
            cpuThread.Start();
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            chart1.Visible = checkBox1.Checked;
        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            chart3.Visible = checkBox2.Checked;
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            chart2.Visible = checkBox3.Checked;
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            chart4.Visible = checkBox4.Checked;
        }

        private void checkBox5_CheckedChanged(object sender, EventArgs e)
        {
            chart5.Visible = checkBox5.Checked;
        }

        private void checkBox6_CheckedChanged(object sender, EventArgs e)
        {
            chart6.Visible = checkBox6.Checked;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            radio.establishConnection("ye");
        }

        private void button3_Click(object sender, EventArgs e)
        {
            radio.executeScript("radioWrite.py","yee");
        }

        private void button5_Click(object sender, EventArgs e)
        {
            radioThread = new Thread(new ThreadStart(this.radioRead));
            radioThread.IsBackground = true;
            radioThread.Start();

            plotRadioThread = new Thread(new ThreadStart(this.plotRadio));
            plotRadioThread.IsBackground = true;
            plotRadioThread.Start();
        }

    }
}
