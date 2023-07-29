using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.Collections;
using System.ComponentModel;
using System.Diagnostics;
using PixeLINK;

namespace AviCapture
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window , INotifyPropertyChanged
    {
        public MainWindow()
        {
            InitializeComponent();
            EnableStartCapture = true;
            StartCaptureVisibility = Visibility.Visible;
            EnableStopCapture = false;
            StopCaptureVisibility = Visibility.Hidden;
            EnableStopConversion = false;
            StopConversionVisibility = Visibility.Hidden;

            RaisePropertyChanged("EnableStartCapture");
            RaisePropertyChanged("StartCaptureVisibility");
            RaisePropertyChanged("EnableStopCapture");
            RaisePropertyChanged("StopCaptureVisibility");
            RaisePropertyChanged("EnableStopConversion");
            RaisePropertyChanged("StopConversionVisibility");
        }

        #region Capturing

        private Thread captureProgress;
        private Thread convertProgress;
        private int hCamera = 0;
        private int secondsPerClip = 10;
        private int numberOfClips = 20;
        static private AviCaptureState s_pCurrentCapture = new AviCaptureState();
        static private AviConvertState s_pCurrentConversion = new AviConvertState();

        private bool StartCapturing()
        {
            // Delete any existing h264 files so we start with a clean slate
            DeleteFiles("h264");

            Api.Uninitialize(hCamera);

            //initialize camera
            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show("Unable to initialize a PixeLINK camera", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return false;
            }

            // Start streaming and pop up a preview window
            bool initialized = false;
            int tmpHWnd = 0;
            rc = Api.SetStreamState(hCamera, StreamState.Start);
            if (Api.IsSuccess(rc))
            {
                rc = Api.SetPreviewState(hCamera, PreviewState.Start, ref tmpHWnd);
                if (Api.IsSuccess(rc))
                {
                    initialized = true;
                }
            }

            if (!initialized)
            {
                Api.Uninitialize(hCamera);
                MessageBox.Show("Unable to start the camera streaming and previewing", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return false;
            }

            AviCaptureState pCapture = new AviCaptureState(
                hCamera,
                DetermineFramesPerSecond(hCamera) * secondsPerClip,
                numberOfClips,
                "capture",
                CaptureTerminatedCallback);

            if (null == pCapture)
            {
                Api.Uninitialize(hCamera);
                MessageBox.Show("Out of memory", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return false;
            }

            s_pCurrentCapture = pCapture;
            if (pCapture.StartCapturing())
            {
                captureProgress = new Thread(UpdateCaptureProgress);
                captureProgress.Start();
                return true;
            }

            s_pCurrentCapture = null;
            Api.Uninitialize(hCamera);
            var result = MessageBox.Show("Unable to start capturing video", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            return false;
        }

        private bool StopCapturing()
        {
            captureProgress.Abort();
            s_pCurrentCapture.StopRunning();
            int tmpHWnd = 0;
            int hCamera = s_pCurrentCapture.GetCameraHandle();
            Api.SetPreviewState(hCamera, PreviewState.Stop, ref tmpHWnd);
            Api.SetStreamState(hCamera, StreamState.Stop);
            return true;
        }

        private int CaptureTerminatedCallback(int hCamera, int numFramesCapture, ReturnCode returnCode)
        {

            if (Api.IsSuccess(returnCode))
            {
                if (!s_pCurrentCapture.KeepRunning())
                {
                    return (int)returnCode;
                }
                if (!s_pCurrentCapture.CaptureNextClip())
                {
                    return (int)returnCode;
                }
            }
            return (int)ReturnCode.Success;
        }

        #endregion

        #region Utilities
        public static int DetermineFramesPerSecond(int hCamera)
        {
            FeatureFlags flags = new FeatureFlags();
            int numParams = 1;
            float[] framesPerSecond = new float[1];   // time in seconds

            //attempts to find Feature.ActualFrameRate (supported on PL-D only), if it cant find it, find Feature.FrameRate instead
            ReturnCode rc = Api.GetFeature(hCamera, Feature.ActualFrameRate, ref flags, ref numParams, framesPerSecond);
            if (!Api.IsSuccess(rc))
            {
                rc = Api.GetFeature(hCamera, Feature.FrameRate, ref flags, ref numParams, framesPerSecond);
            }
            
            // Round up to the next integer.
            int fps = (int)Math.Ceiling((double)framesPerSecond[0]);
            return fps;
        }

        private bool DeleteFiles(string fileExtention)
        {
            DirectoryInfo di = new DirectoryInfo(Directory.GetCurrentDirectory());
            FileInfo[] files = di.GetFiles("*." + fileExtention).Where(p => p.Extension == "." + fileExtention).ToArray();
            foreach (FileInfo file in files)
                try
                {
                    file.Attributes = FileAttributes.Normal;
                    File.Delete(file.FullName);
                }
                catch { }

            return true;
        }

        private void Converth264FilesToAvi()
        {
            if (null == s_pCurrentConversion)
            {
                return;
            }

            string inputFileName;
            string outputFileName;
            int numberOfFiles = s_pCurrentConversion.GetNumberOfFiles();

            convertProgress = new Thread(UpdateConvertProgress);
            convertProgress.Start();

            // Convert each file
            for (int i = 0; i < numberOfFiles; i++)
            {

                // Did the user ask us to stop?
                if (!s_pCurrentConversion.KeepRunning())
                {
                    break;
                }

                s_pCurrentConversion.SetCurrentIndex(i);

                // Create the name of the output file by changing the extension
                inputFileName = s_pCurrentConversion.GetCurrentFileName().Name;
                outputFileName = inputFileName.Replace("h264", "avi");
 
                // And convert it
                ReturnCode rc = Api.FormatClip(inputFileName, outputFileName, ClipEncoding.H264, VideoFormat.Avi);
                if (!Api.IsSuccess(rc))
                {
                    MessageBox.Show("error: " + rc, "error", MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }

            convertProgress.Abort();

            EnableStartCapture = true;
            StartCaptureVisibility = Visibility.Visible;
            RaisePropertyChanged("EnableStartCapture");
            RaisePropertyChanged("StartCaptureVisibility");
            EnableStopConversion = false;
            StopConversionVisibility = Visibility.Hidden;
            RaisePropertyChanged("EnableStopConversion");
            RaisePropertyChanged("StopConversionVisibility");

            return;
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

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Environment.Exit(Environment.ExitCode);
        }

        private bool StartConverting()
        {
            DeleteFiles("avi");

            AviConvertState pConvert = new AviConvertState();
            if (null == pConvert)
            {
                return false;
            }
            
            s_pCurrentConversion = pConvert;

            Thread pThread = new Thread(Converth264FilesToAvi);
            if (null == pThread)
            {
                return false;
            }

            s_pCurrentConversion.SetThread(pThread);

            return true;
        }

        private bool StopConverting()
        {      
            s_pCurrentConversion.StopConverting();
            return true;
        }

        #endregion

        #region DisplayVariables

        public string CapturingToName { get; set; }
        public double ClipProgress { get; set; }
        public string ConvertingToName { get; set; }
        public bool EnableStartCapture { get; set; }
        public Visibility StartCaptureVisibility { get; set; }
        public Visibility StopCaptureVisibility { get; set; }
        public bool EnableStopCapture { get; set; }
        public bool EnableStopConversion { get; set; }
        public Visibility StopConversionVisibility { get; set; }

        #endregion

        #region DisplayUpdateThreads

        private void UpdateCaptureProgress()
        {
            Stopwatch sw = new Stopwatch();
            sw.Start();
            double startTime = sw.ElapsedMilliseconds / 1000;

            while (s_pCurrentCapture.KeepRunning() == true)
            {
                if (CapturingToName != s_pCurrentCapture.GetCurrentClipFileName())
                {
                    startTime = sw.ElapsedMilliseconds/1000;
                    CapturingToName = s_pCurrentCapture.GetCurrentClipFileName();
                    RaisePropertyChanged("CapturingToName");
                }
                ClipProgress = ((((int)sw.ElapsedMilliseconds / 1000) - startTime) / secondsPerClip);
                RaisePropertyChanged("ClipProgress");
            }
        }

        private void UpdateConvertProgress()
        {
            while (true)
            {
                ConvertingToName = "Converting " + s_pCurrentConversion.GetCurrentFileName() + " to .avi (" + (s_pCurrentConversion.GetCurrentFileIndex() + 1) + " of " + s_pCurrentConversion.GetNumberOfFiles() + ") ";
                RaisePropertyChanged("ConvertingToName");
                Thread.Sleep(250);
                ConvertingToName = "Converting " + s_pCurrentConversion.GetCurrentFileName() + " to .avi (" + (s_pCurrentConversion.GetCurrentFileIndex() + 1) + " of " + s_pCurrentConversion.GetNumberOfFiles() + ") .";
                RaisePropertyChanged("ConvertingToName");
                Thread.Sleep(250);
                ConvertingToName = "Converting " + s_pCurrentConversion.GetCurrentFileName() + " to .avi (" + (s_pCurrentConversion.GetCurrentFileIndex() + 1) + " of " + s_pCurrentConversion.GetNumberOfFiles() + ") ..";
                RaisePropertyChanged("ConvertingToName");
                Thread.Sleep(250);
                ConvertingToName = "Converting " + s_pCurrentConversion.GetCurrentFileName() + " to .avi (" + (s_pCurrentConversion.GetCurrentFileIndex() + 1) + " of " + s_pCurrentConversion.GetNumberOfFiles() + ") ...";
                RaisePropertyChanged("ConvertingToName");
                Thread.Sleep(250);
            }
        }


        #endregion

        #region UI_Buttons

        private void StartCaptureButton_Click(object sender, RoutedEventArgs e)
        {
                if (StartCapturing())
                {
                    EnableStartCapture = false;
                    StartCaptureVisibility = Visibility.Hidden;
                    EnableStopCapture = true;
                    StopCaptureVisibility = Visibility.Visible;
                    RaisePropertyChanged("EnableStartCapture");
                    RaisePropertyChanged("StartCaptureVisibility");
                    RaisePropertyChanged("EnableStopCapture");
                    RaisePropertyChanged("StopCaptureVisibility");
                }
            return;
        }

        private void EndCaptureButton_Click(object sender, RoutedEventArgs e)
        {

            EnableStopCapture = false;
            StopCaptureVisibility = Visibility.Hidden;
            RaisePropertyChanged("EnableStopCapture");
            RaisePropertyChanged("StopCaptureVisibility");

            StopCapturing();

            MessageBoxResult result = MessageBox.Show("Convert h264 files to AVI files?", "Convert Files?", MessageBoxButton.YesNo, MessageBoxImage.Question);

            if (result == MessageBoxResult.Yes)
            {
                EnableStopConversion = true;
                StopConversionVisibility = Visibility.Visible;
                RaisePropertyChanged("EnableStopConversion");
                RaisePropertyChanged("StopConversionVisibility");
                StartConverting();
            }
            else
            {
                EnableStartCapture = true;
                StartCaptureVisibility = Visibility.Visible;
                RaisePropertyChanged("EnableStartCapture");
                RaisePropertyChanged("StartCaptureVisibility");
            }

            return;
        }

        private void StopConversionButton_Click(object sender, RoutedEventArgs e)
        {
            if (s_pCurrentConversion.KeepRunning() == true)
            {
                StopConverting();
                StopConversionButton.Content = "        Please wait.\nCancelling conversion.";
            }
        }

        private void ExitButton_Click(object sender, RoutedEventArgs e)
        {
            StartCaptureButton.IsEnabled = false;
            EndCaptureButton.IsEnabled = false;
            Environment.Exit(Environment.ExitCode);
            StartCaptureButton.IsEnabled = true;
            EndCaptureButton.IsEnabled = true;
        }


        #endregion


    }
}