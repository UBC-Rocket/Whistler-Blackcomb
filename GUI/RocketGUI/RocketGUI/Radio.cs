using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using System.IO;
using XBee;

namespace RocketGUI
{
    public class Radio
    {
        //Define path to python dependencies 
        //WILL NEED TO BE CHANGED FOR SEPERATE MACHINES!
        private const string PATH_TO_PYTHON_COMPILER = "C:/Users/Ben/AppData/Local/Programs/Python/Python36-32/python.exe";

        //Create instance of Radio Data, where all information will be stored
        //public RadioData radioData = new RadioData();

        //Create a lock to be used on the radioData object
        private readonly object dataLock = new object();

        public List<RadioData> radioData = new List<RadioData>();

        public List<RadioData> getRadioData()
        {
            return this.radioData;
        }

        public async void establishConnection(string comPort)
        {
            var controller = new XBeeController();

            // setup a simple callback for each time we discover a node
            controller.NodeDiscovered += async (sender, args) =>
            {
                Console.WriteLine("Discovered {0}", args.Name);

                // setup some pins
                //await args.Node.SetInputOutputConfigurationAsync(InputOutputChannel.Channel2, InputOutputConfiguration.DigitalIn);
                //await args.Node.SetInputOutputConfigurationAsync(InputOutputChannel.Channel3, InputOutputConfiguration.AnalogIn);

                // set sample rate
                await args.Node.SetSampleRateAsync(TimeSpan.FromSeconds(1));

                // register callback for sample recieved from this node
                // TODO: in practice you would want to make sure you only subscribe once (or better yet use Rx)
                args.Node.SampleReceived += (node, sample) => Console.WriteLine("Sample recieved: {0}", sample);
            };

            // open the connection to our coordinator
            //controller = await XBeeController.FindAndOpenAsync(SerialPort.GetPortNames(), 9600);
            await controller.OpenAsync("COM18", 9600);

            //await controller.DiscoverNetworkAsync();

            var localNode = controller.Local;
            // which is the same as calling await controller.GetNodeAsync(); // (address = null)

            var serialNumber = await localNode.GetSerialNumberAsync();
            await localNode.SetNodeIdentifierAsync("Ground Station 2");
            var address = await localNode.GetAddressAsync();
            Console.WriteLine(address);

            var remoteNode = await controller.GetNodeAsync(address, false);

            //Read
            //Console.WriteLine("Discovered Remote");
            //await remoteNode.SetSampleRateAsync(TimeSpan.FromSeconds(2));

            //remoteNode.SampleReceived += (node, sample) => Console.WriteLine("Sample recieved: {0}", sample);


            //Write
            var remoteSerialNumber = await remoteNode.GetSerialNumberAsync();
            Console.WriteLine(remoteSerialNumber);
            await remoteNode.TransmitDataAsync(Encoding.UTF8.GetBytes("Hello!"));
            Console.WriteLine("sent");
            // etc

            Console.WriteLine("finished");

            // now discover the network, which will trigger the NodeDiscovered callback for each node found
            //await controller.DiscoverNetworkAsync();

            controller.Dispose();

            // wait for the samples to flow in...
        }

        /**
        * <summary>
        * Performs the required overhead commands to be able to execute python script from C# command.
        * Called during executeScript and readScript.
        * </summary>
        **/
        private ProcessStartInfo setupScript(string path, string args)
        {
            ProcessStartInfo start = new ProcessStartInfo();
            start.FileName = PATH_TO_PYTHON_COMPILER;
            start.Arguments = string.Format("{0} {1}", path, args);
            start.UseShellExecute = false;
            start.RedirectStandardOutput = true;
            return start;
        }

        /**
         * <summary>
         * Performs a python script in its entirety. Queues any outputs until execution is completed before sending back.
         * Writes any output to console.
         * </summary>
         **/
        public void executeScript(string path, string args)
        {
            ProcessStartInfo start = setupScript(Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), @"..\..\..\..\PythonScripts\",path)), args);
            using (Process process = Process.Start(start))
            {
                using (StreamReader reader = process.StandardOutput)
                {
                    string result = reader.ReadToEnd();
                    Console.Write(result);
                }
            }
        }

        /**
         * <summary>
         * Executes a python script, but sets up event to trigger everytime output data is recieved. 
         * This will act as the continuous reading of radio data. Will perform an action everytime this 
         * event triggers, as well as write it to the console.
         * </summary>
         **/
        public void readData(string path, string args)
        {
            using (var process = new Process())
            {
                ProcessStartInfo start = setupScript(Path.GetFullPath(Path.Combine(Directory.GetCurrentDirectory(), @"..\..\..\..\PythonScripts\", path)), args);
                process.StartInfo = start;
                process.EnableRaisingEvents = true;

                process.OutputDataReceived += (s, o) => { splitData(s, o); }; //Add event handler

                process.Start();
                process.BeginOutputReadLine();
                process.WaitForExit();
            }
        }

        /**
         * <summary>
         * This function is called everytime readData function recieves a new piece of radio information.
         * Function will sort and categorize the given data to populate the fields of radio data class
         * </summary>
         **/
        private void splitData(object s, System.Diagnostics.DataReceivedEventArgs o)
        {
            lock (dataLock)
            {
                List<int> data = o.Data.Split(',').Select(x => int.Parse(x)).ToList();
                RadioData currRadioData = new RadioData(data[0], data[1]);
                radioData.Add(currRadioData);
                Console.WriteLine(currRadioData.tc1);
                Console.WriteLine(currRadioData.pt1);
            }
        }

    }
}
