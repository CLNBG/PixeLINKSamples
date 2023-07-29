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
using System.Diagnostics;
using System.ComponentModel;
using System.Threading;
using System.IO;
using System.Drawing;
using PixeLINK;

namespace Triggering
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        public MainWindow()
        {
            EnableButtons = true;
            InitializeComponent();

            Api.Uninitialize(hCamera);

            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (!Api.IsSuccess(rc))
            {
                return;
            }

        }

        
        private int hCamera = 0;
        private Thread runHardwareTriggerTest;
        private Thread runHardwareTriggerCountdown;
        public bool EnableButtons { get; set; }
        public BitmapImage DisplayImage { get; set; }
        public string TestResultText { get; set; }

        //alerts the view that specified properties have changed
        public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string property)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(property));
            }
        }

        private void TestFreeRunningTriggerButton_Click(object sender, RoutedEventArgs e)
        {
            if (!IsTriggeringSupported(hCamera))
            {
                Api.Uninitialize(hCamera);
                return;
            }

            // Start with triggering disabled so we start with a clean slate
            DisableTriggering(hCamera);

            TestFreeRunningTrigger(hCamera);
        }

        private void TestSoftwareTriggerButton_Click(object sender, RoutedEventArgs e)
        {
            if (!IsTriggeringSupported(hCamera))
            {
                Api.Uninitialize(hCamera);
                return;
            }

            // Start with triggering disabled so we start with a clean slate
            DisableTriggering(hCamera);

            TestSoftwareTrigger(hCamera);
            
        }

        private void TestHardwareTriggerButton_Click(object sender, RoutedEventArgs e)
        {
            EnableButtons = false;
            RaisePropertyChanged("EnableButtons");
            DisplayImage = null;
            RaisePropertyChanged("DisplayImage");

            //starts a countdown that will result in a failed test if no hardware trigger input is received
            runHardwareTriggerCountdown = new Thread(HardwareTriggerCountdown);
            runHardwareTriggerCountdown.Start();

            if (!IsTriggeringSupported(hCamera))
            {
                Api.Uninitialize(hCamera);
                return;
            }

            // Start with triggering disabled so we start with a clean slate
            DisableTriggering(hCamera);

            //hardware trigger test is performed in a sperate thread to that the UI doeant freeze while waiting for the hardware trigger input
            runHardwareTriggerTest = new Thread(TestHardwareTrigger);
            runHardwareTriggerTest.IsBackground = true;
            runHardwareTriggerTest.Start();
        }

        

        //counts down from ten and kills the hardware tigger test thread if the counter reaches zero
        private void HardwareTriggerCountdown()
        {
            int count = 10;
            while (true)
            {
                TestResultText = "Waiting for hardware trigger input. Timeout in " + count.ToString() + "s";
                RaisePropertyChanged("TestResultText");
                Thread.Sleep(1000);
                count --;

                if (count == 0)
                {
                    runHardwareTriggerTest.Abort();
                    TestResultText = "Hardware Test Result: No hardware trigger detected";
                    RaisePropertyChanged("TestResultText");
                    EnableButtons = true;
                    RaisePropertyChanged("EnableButtons");
                    Api.SetStreamState(hCamera, StreamState.Stop);
                    return;
                }

            }
        }

        //
        // Not all PixeLINK cameras support triggering.
        // Machine vision cameras support triggering, but 
        // microscopy cameras do not.
        //
        //
        private bool IsTriggeringSupported(int hCamera)
        {
            CameraFeature featureInfo = new CameraFeature();
            ReturnCode rc = Api.GetCameraFeatures(hCamera, Feature.Trigger, ref featureInfo);
            return featureInfo.IsSupported;
        }

        private void DisableTriggering(int hCamera)
        {
            FeatureFlags flags = new FeatureFlags();
            int numParams = 5;
            float[] parameters = new float[5];
            ReturnCode rc = Api.GetFeature(hCamera, Feature.Trigger, ref flags, ref numParams, parameters);

            flags |= FeatureFlags.Off;

            rc = Api.SetFeature(hCamera, Feature.Trigger, flags, numParams, parameters);

            return;
        }

        //
        // Set up the camera for triggering, and, enable triggering.
        //
        private void SetTriggering(int hCamera, int mode, TriggerType type, Polarity polarity, float delay, float param)
        {
            FeatureFlags flags = new FeatureFlags();
            int numParams = 5;
            float[] parameters = new float[numParams];

            // Read the current settings
            ReturnCode rc = Api.GetFeature(hCamera, Feature.Trigger, ref flags, ref numParams, parameters);

            // Very important step: Enable triggering by clearing the FeatureFlags.Off bit
            flags &= (~FeatureFlags.Off);

            parameters[(int)FeatureParameterIndex.TriggerMode] = (float)mode;
            parameters[(int)FeatureParameterIndex.TriggerType] = (float)type;
            parameters[(int)FeatureParameterIndex.TriggerPolarity] = (float)polarity;
            parameters[(int)FeatureParameterIndex.TriggerDelay] = delay;
            parameters[(int)FeatureParameterIndex.TriggerParameter] = param;

            // And write the new settings
            rc = Api.SetFeature(hCamera, Feature.Trigger, flags, numParams, parameters);
            return;
        }



        // retrieves image from camera and displays it in the UI
        private void CaptureImage(int hCamera, string testName)
        {
            byte[] buffer = new byte[3000 * 3000 * 2]; // buffer big enough for any current PixeLINK camera
            FrameDescriptor frameDesc = new FrameDescriptor(); // ctor sets size for us

            ReturnCode rc = Api.GetNextFrame(hCamera, buffer.Length, buffer, ref frameDesc);

            if (runHardwareTriggerCountdown != null)
            {
                runHardwareTriggerCountdown.Abort();
            }
            
            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show("Error capturing image: " + rc, "error", MessageBoxButton.OK, MessageBoxImage.Error);
                DisplayImage = new BitmapImage();
                RaisePropertyChanged("DisplayImage");
                TestResultText = testName + " Result: " + rc.ToString();
                RaisePropertyChanged("TestResultText");
                return;
            }

            int outputImageBufferSize = 5000 * 5000 * 2;
            byte[] outputImageBuffer = new byte[outputImageBufferSize];
            Api.FormatImage(buffer, ref frameDesc, ImageFormat.Bmp, outputImageBuffer, ref outputImageBufferSize);

            TestResultText = testName + " Result: " + rc.ToString();
            RaisePropertyChanged("TestResultText");

            //converts Bitmap to BitmapImage so that it can be displayed in the UI window
            Bitmap bitmap = new Bitmap(new MemoryStream(outputImageBuffer));
            using (var memory = new MemoryStream())
            {
                bitmap.Save(memory, System.Drawing.Imaging.ImageFormat.Png);
                memory.Position = 0;

                var bitmapImage = new BitmapImage();
                bitmapImage.BeginInit();
                bitmapImage.StreamSource = memory;
                bitmapImage.CacheOption = BitmapCacheOption.OnLoad;
                bitmapImage.EndInit();
                bitmapImage.Freeze();
                DisplayImage = bitmapImage;
                RaisePropertyChanged("DisplayImage");
            }

        }

        private void TestFreeRunningTrigger(int hCamera)
        {
            SetTriggering(hCamera,
                0,                              // Mode 0 Triggering
                TriggerType.FreeRunning,
                Polarity.Negative,
                0.0f,                           // no delay
                0);                             // unused for Mode 0

            // We can now grab two images (without blocking)
            ReturnCode rc = Api.SetStreamState(hCamera, StreamState.Start);

            CaptureImage(hCamera, "Free Running Test");

            rc = Api.SetStreamState(hCamera, StreamState.Stop);
        }

        //
        // With software triggering, calling GetNextFrame causes 
        // the camera to capture an image. The camera must be in the 
        // streaming state, but no image will be 'streamed' to the host
        // until GetNextFrame is called.
        //
        private void TestSoftwareTrigger(int hCamera)
        {
            SetTriggering(hCamera,
                0,                              // Mode 0 Triggering
                TriggerType.Software,
                Polarity.Negative,
                0.0f,                           // no delay
                0);                             // unused for Mode 0

            // We can now grab two images (without blocking)
            ReturnCode rc = Api.SetStreamState(hCamera, StreamState.Start);
            Debug.Assert(Api.IsSuccess(rc));

            CaptureImage(hCamera, "Software Test");

            rc = Api.SetStreamState(hCamera, StreamState.Stop);
            Debug.Assert(Api.IsSuccess(rc));
        }

        //
        // With hardware triggering, the camera doesn't take an image until the 
        // trigger input of the machine vision connector is activated. 
        //
        private void TestHardwareTrigger()
        {

            SetTriggering(hCamera,
                0,                              // Mode 0 Triggering
                TriggerType.Hardware,
                Polarity.Negative,
                0.0f,                           // no delay
                0);                             // unused for Mode 0

            ReturnCode rc = Api.SetStreamState(hCamera, StreamState.Start);

            CaptureImage(hCamera, "Hardware Test");

            rc = Api.SetStreamState(hCamera, StreamState.Stop);

            EnableButtons = true;
            RaisePropertyChanged("EnableButtons");
            
            return;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Environment.Exit(Environment.ExitCode);
        }

    }
}
