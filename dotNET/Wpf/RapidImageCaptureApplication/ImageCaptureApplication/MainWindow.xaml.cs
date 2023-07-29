using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Threading;
using System.Diagnostics;
using System.Text.RegularExpressions;
using PixeLINK;
using System.IO;

using Microsoft.Win32;



namespace RapidImageCaptureApplication
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 

        //the purpose of this application is to capture and then save images as quickly as possible.
        //the images are captures on a single thread using Api.GetNextFrame.
        //each images is formatted and saved on it's own indevidual thread so frames are not missed while the images are being processed


    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        public MainWindow()
        {
            InitializeComponent();

            detectCamera = new Thread(DetectCamera);
            detectCamera.Priority = ThreadPriority.Lowest;
            killDetectCamera = false;
            detectCamera.IsBackground = true;
            detectCamera.Start();
        }

        private Thread runTestThread;
        private Thread detectCamera;

        private bool killTestThread;
        private bool killDetectCamera;

        private string directoryPath = Environment.GetFolderPath(Environment.SpecialFolder.MyPictures);
        private string imageName = "Image";

        private ImageFormat imageFormat = ImageFormat.Bmp;
        private string fileExtention = ".bmp";


        void DetectCamera()
        {
            Stopwatch detectCameraStopwatch = new Stopwatch();
            detectCameraStopwatch.Start();

            long currentTime;

            while (!killDetectCamera)
            {
                currentTime = detectCameraStopwatch.ElapsedMilliseconds;

                ReturnCode rc = Api.Initialize(0, ref hCamera);
                if (Api.IsSuccess(rc))
                {
                    CameraInformation ci = new CameraInformation();
                    Api.GetCameraInformation(hCamera, ref ci);
                    cameraName = ci.CameraName;
                    RaisePropertyChanged("cameraName");
                    enableStartButton = true;
                    RaisePropertyChanged("enableStartButton");
                    Api.Uninitialize(hCamera);
                }
                else
                {
                    enableStartButton = false;
                    RaisePropertyChanged("enableStartButton");
                    cameraName = "None";
                    RaisePropertyChanged("cameraName");
                }



                while (detectCameraStopwatch.ElapsedMilliseconds - currentTime < 2000)
                {
                }

            }
        }

        private bool enableStartButton = true;
        public bool EnableStartButton
        {
            get
            {
                return enableStartButton;
            }
            set
            {
                enableStartButton = value;
            }
        }

        private bool enableStopButton = false;
        public bool EnableStopButton
        {
            get
            {
                return enableStopButton;
            }
            set
            {
                enableStopButton = value;
            }
        }

        private string cameraName = "None";

        public string CameraName
        {
            get
            {
                return cameraName;
            }
            set
            {
                cameraName = value;
            }
        }

        public string DirectoryPath
        {
            get
            {
                return directoryPath;
            }
            set
            {
                directoryPath = value;
                RaisePropertyChanged("DirectoryPath");
            }
        }

        private float actualFrameRate = 0;
        public float ActualFrameRate
        {
            get
            {
                return actualFrameRate;
            }
            set
            {
                actualFrameRate = value;
            }
        }

        //alerts the view that specified properties have changed
        public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string property)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(property));
            }
        }

        private int hCamera = 0;

        private Stopwatch sw;

        private void startTestBtn_Click(object sender, RoutedEventArgs e)
        {
            ReturnCode rc;

            //detectCamera.Abort();

            killDetectCamera = true;

            elapsedTime = "0.000s";
            RaisePropertyChanged("elapsedTime");
            expectedFrameCount = 0;

            FeatureFlags ff = new FeatureFlags();
            float[] p = new float[1];
            int i = 1;
            rc = Api.Initialize(0, ref hCamera);
            rc = Api.GetFeature(hCamera, Feature.ActualFrameRate, ref ff, ref i, p);
            rc = Api.Uninitialize(hCamera);
            actualFrameRate = p[0];
            actualFrameRate = (float)(Math.Round((double)actualFrameRate, 2));
            RaisePropertyChanged("actualFrameRate");


            lostFrames = 0;

            RaisePropertyChanged("expectedFrameCount");
            RaisePropertyChanged("lostFrames");


            enableStartButton = false;
            enableStopButton = true;
            RaisePropertyChanged("enableStartButton");
            RaisePropertyChanged("enableStopButton");

            sw = new Stopwatch();



            sw.Start();
            runTestThread = new Thread(RunTest);
            runTestThread.IsBackground = true;
            runTestThread.Priority = ThreadPriority.Highest;
            killTestThread = false;
            runTestThread.Start();


            ImageNameTextBox.IsEnabled = false;
            BrowseButton.IsEnabled = false;
        }

        private void endTestBtn_Click(object sender, RoutedEventArgs e)
        {

            enableStartButton = true;
            enableStopButton = false;
            RaisePropertyChanged("enableStartButton");
            RaisePropertyChanged("enableStopButton");

            elapsedTime = ((sw.ElapsedMilliseconds) * 0.001).ToString() + "s";
            RaisePropertyChanged("elapsedTime");

            expectedFrameCount = frameDesc.FrameNumber;
            actualFrameRate = frameDesc.ActualFrameRate;
            actualFrameRate = (float)(Math.Round((double)actualFrameRate, 2));
            RaisePropertyChanged("actualFrameRate");
            RaisePropertyChanged("expectedFrameCount");
            RaisePropertyChanged("lostFrames");

            ReturnCode rc;


            //runTestTread.Abort();
            killTestThread = true;


            rc = Api.SetStreamState(hCamera, StreamState.Stop);
            rc = Api.Uninitialize(hCamera);

            sw.Stop();

            ImageNameTextBox.IsEnabled = true;
            BrowseButton.IsEnabled = true;

            detectCamera = new Thread(DetectCamera);
            detectCamera.IsBackground = true;
            detectCamera.Start();
        }

        private string elapsedTime = "0.000s";
        public string ElapsedTime
        {
            get
            {
                return elapsedTime;
            }
            set
            {
                elapsedTime = value;
            }
        }

        public string ImageName
        {
            get
            {
                return imageName;
            }
            set
            {
                imageName = value;
                RaisePropertyChanged("ImageName");
            }
        }

        private int expectedFrameCount;
        public int ExpectedFrameCount
        {
            get
            {
                return expectedFrameCount;
            }
            set
            {
                expectedFrameCount = value;
            }
        }

        private int lostFrames;
        public int LostFrames
        {
            get
            {
                return lostFrames;
            }
            set
            {
                lostFrames = value;
            }
        }

        private int actualFrameCount;
        public int ActualFrameCount
        {
            get
            {
                return actualFrameCount;
            }
            set
            {
                actualFrameCount = value;
            }
        }

        private FrameDescriptor frameDesc;

        void UpdateUI()
        {
            expectedFrameCount = frameDesc.FrameNumber + 1;
            actualFrameRate = frameDesc.ActualFrameRate;
            actualFrameRate = (float)(Math.Round((double)actualFrameRate, 2));
            RaisePropertyChanged("actualFrameRate");
            RaisePropertyChanged("expectedFrameCount");
            RaisePropertyChanged("lostFrames");
            elapsedTime = ((sw.ElapsedMilliseconds) * 0.001).ToString() + "s";
            RaisePropertyChanged("elapsedTime");
        }

        void UpdateUIFinal()
        {
            expectedFrameCount = frameDesc.FrameNumber + 1;
            actualFrameRate = frameDesc.ActualFrameRate;
            actualFrameRate = (float)(Math.Round((double)actualFrameRate, 2));
            RaisePropertyChanged("actualFrameRate");
            RaisePropertyChanged("expectedFrameCount");
            RaisePropertyChanged("lostFrames");
            elapsedTime = ((sw.ElapsedMilliseconds) * 0.001).ToString() + "s";
            RaisePropertyChanged("elapsedTime");

            lostFrames = frameDesc.FrameNumber + 1 - Directory.GetFiles(directoryPath + "\\" + imageName).Length;

        }

        //int hCamera;

        void RunTest()
        {
            hCamera = 0;
            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show("Error: Unable to initialize a camera\n");
                //Console.Out.WriteLine("Error: Unable to initialize a camera\n");
            }


            if (Directory.Exists(directoryPath + "//" + imageName))
            {
                foreach (string fileName in Directory.GetFiles(directoryPath + "//" + imageName))
                {
                    File.Delete(fileName);
                }

                Directory.Delete(directoryPath + "//" + imageName);
            }



            // Create the subfolder
            System.IO.Directory.CreateDirectory(directoryPath + "//" + imageName);

            FeatureFlags roiFlags = new FeatureFlags();
            int roiNumberofParameters = 4;
            float[] roiParameters = new float[4];
            Api.GetFeature(hCamera, Feature.Roi, ref roiFlags, ref roiNumberofParameters, roiParameters);

            int bufferSize = (int)roiParameters[2] * (int)roiParameters[3] * 3;



            byte[] frameBuffer = new Byte[bufferSize];
            

            frameDesc = new FrameDescriptor();
            frameDesc.StructSize = System.Runtime.InteropServices.Marshal.SizeOf(frameDesc);

            long cycleTime;

            sw.Reset();
            sw.Start();

            Task saveImageTask;

            if (Api.IsSuccess(Api.SetStreamState(hCamera, StreamState.Start)))
            {
                while (true)
                {
                    cycleTime = sw.ElapsedMilliseconds;

                    while (sw.ElapsedMilliseconds - cycleTime < 2000)
                    {
                        Api.GetNextFrame(hCamera, frameBuffer.Length, frameBuffer, ref frameDesc);
                        saveImageTask = new Task(delegate () { SaveImage(frameBuffer, frameDesc.FrameNumber, frameBuffer, frameDesc); });
                        saveImageTask.Start();

                        if (killTestThread)
                        {
                            break;
                        }

                    }

                    UpdateUI();

                    if (killTestThread)
                    {
                        break;
                    }

                }

                UpdateUIFinal();

            }
            else
            {
                return;
            }

            return;

        }

        public void SaveImage( byte[] buffer, int increment, byte[] frameBuffer, FrameDescriptor frameDesc )
        {
            int bufferSize = 0;
            Api.FormatImage(frameBuffer, ref frameDesc, imageFormat, null, ref bufferSize);
            byte[] formatedBuffer = new byte[bufferSize];
            Api.FormatImage(frameBuffer, ref frameDesc, imageFormat, formatedBuffer, ref bufferSize);

            try
            {
                File.WriteAllBytes(directoryPath + "\\" + imageName + "\\" + imageName + frameDesc.FrameNumber + fileExtention, formatedBuffer);
            }
            catch (Exception e)
            {
                lostFrames++;
            }

        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Environment.Exit(Environment.ExitCode);
        }

        private void BrowseButton_Click(object sender, RoutedEventArgs e)
        {
            Window ParentWindow = Window.GetWindow(this);
           

            System.Windows.Forms.FolderBrowserDialog dialog = new System.Windows.Forms.FolderBrowserDialog();
            dialog.Description = "Please select a folder.";
            System.Windows.Forms.DialogResult result = dialog.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.OK)
                DirectoryTextBox.Text = dialog.SelectedPath;
        }
    }
}
