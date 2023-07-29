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
using PixeLINK;

namespace CallBack
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        public MainWindow()
        {
            InitializeComponent();
            StartStopButton.Content = "Start";
        }

        static int s_hCamera;
        static Api.Callback s_callbackDelegate;
        static bool s_idle = true;

        //alerts the view that specified properties have changed
        public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string property)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(property));
            }
        }

        private void StartStopButton_Click(object sender, RoutedEventArgs e)
        {
            if (s_idle)
            {
                if (StartCallback())
                {
                    s_idle = false;
                    StartStopButton.Content = "Stop";

                }
            }
            else
            {
                StopCallback();
                s_idle = true;
                StartStopButton.Content = "Start";
            }
        }

        private bool StartCallback()
        {
            if (!Api.IsSuccess(Api.Initialize(0, ref s_hCamera)))
            {
                return false;
            }


			// Note here that we keep a reference to the callback object that exists for the entire time
			// the callback method will be called.
			// If we don't, the delegate object will be cleaned up with the first garbage collection sweep, and then
			// the next callback from the API will crash.
			s_callbackDelegate = new Api.Callback(MyCallbackFunction);
			Api.SetCallback(s_hCamera, Overlays.Preview, 0xD00D, s_callbackDelegate);


            Api.SetStreamState(s_hCamera, StreamState.Start);

            int hWnd = 0;
            Api.SetPreviewState(s_hCamera, PreviewState.Start, ref hWnd);
            return true;
        }

        public string hCameraDisplayText { get; set; }
        public string pBufDisplayText { get; set; }
        public string DataFormatDisplayText { get; set; }
        public string RoiTopDisplayText { get; set; }
        public string RoiLeftDisplayText { get; set; }
        public string RoiHeightDisplayText { get; set; }
        public string RoiWidthDisplayText { get; set; }
        public string UserDataDisplayText { get; set; }
        public string ImageMeanDisplayText { get; set; }


        private int MyCallbackFunction(int hCamera, System.IntPtr pBuf, PixeLINK.PixelFormat pf, ref FrameDescriptor frameDesc, int userData)
        {
            hCameraDisplayText = hCamera.ToString();
            pBufDisplayText = pBuf.ToString();
            DataFormatDisplayText = pf.ToString();
            RoiTopDisplayText = frameDesc.RoiTop.ToString();
            RoiLeftDisplayText = frameDesc.RoiLeft.ToString();
            RoiHeightDisplayText = frameDesc.RoiHeight.ToString();
            RoiWidthDisplayText = frameDesc.RoiWidth.ToString();
            UserDataDisplayText = userData.ToString();

            long total = 0;
            long numPixels = frameDesc.NumberOfPixels();
            for (int i = 0; i < numPixels; i++)
            {
                total += System.Runtime.InteropServices.Marshal.ReadByte(pBuf, i);
            }
            double mean = (double)total / (double)numPixels;

            ImageMeanDisplayText = Math.Round(mean, 2, MidpointRounding.AwayFromZero).ToString();

            UpdateDisplayText();

            return 0;
        }

        private void UpdateDisplayText()
        {
            RaisePropertyChanged("hCameraDisplayText");
            RaisePropertyChanged("pBufDisplayText");
            RaisePropertyChanged("DataFormatDisplayText");
            RaisePropertyChanged("RoiTopDisplayText");
            RaisePropertyChanged("RoiLeftDisplayText");
            RaisePropertyChanged("RoiHeightDisplayText");
            RaisePropertyChanged("RoiWidthDisplayText");
            RaisePropertyChanged("UserDataDisplayText");
            RaisePropertyChanged("ImageMeanDisplayText");
        }

        private void StopCallback()
        {
            Api.Uninitialize(s_hCamera);
            s_hCamera = 0;
            s_callbackDelegate = null;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Environment.Exit(Environment.ExitCode);
        }

    }
}
