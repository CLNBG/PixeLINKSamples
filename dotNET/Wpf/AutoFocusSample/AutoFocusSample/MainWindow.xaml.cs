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
using PixeLINK;
using System.ComponentModel;
using System.Threading;
using System.Windows.Threading;

namespace AutoFocusSample
{
    /// <summary>
    /// This sample application demonstrates how to use the Auto-Focus operation
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {

        int hCamera = 0;

        public MainWindow()
        {
            InitializeComponent();
            Api.Initialize(0, ref hCamera);
            StartImageCapture();
        }

        /// <summary>
        /// Code to run the auto focus operation once
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void AutoFocusButton_Click(object sender, RoutedEventArgs e)
        {
            float[] parameters = new float[1];
            ReturnCode rc = Api.SetFeature(hCamera, Feature.Focus, FeatureFlags.OnePush, 1, parameters);
        }




        /// <summary>
        /// Code to generate video preview
        /// </summary>
        /// 
        private WriteableBitmap _bitmap;

        public event PropertyChangedEventHandler PropertyChanged;

        public WriteableBitmap bitmap
        {
            get { return _bitmap; }
            set
            {
                _bitmap = value;
                RaisePropertyChanged("bitmap");
            }
        }

        public void RaisePropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        Thread imageCaptureThread;
        bool runImageCapture = false;
        bool updateROI = true;

        int counter = 0;
        public void StartImageCapture()
        {
           

            ReturnCode rcStart = Api.SetStreamState(hCamera, StreamState.Start);

            imageCaptureThread = new Thread(() => CaptureImages());
            imageCaptureThread.IsBackground = true;
            runImageCapture = true;
            imageCaptureThread.Name = "Simon" + counter++;
            imageCaptureThread.Start();
        }

        byte[] formatedBuffer;
        FeatureFlags Focusflags = new FeatureFlags();


        public void CaptureImages()
        {
            CameraFeature cf = new CameraFeature();
            Api.GetCameraFeatures(hCamera, Feature.Roi, ref cf);

            int bufferSize = (int)cf.parameters[3].MaximumValue * (int)cf.parameters[2].MaximumValue * 2;

            byte[] buffer = new byte[bufferSize];

            int formattedBufferSize = 0;

            FrameDescriptor fd = new FrameDescriptor();

            while (runImageCapture)
            {
                if (updateROI)
                {
                    cf = new CameraFeature();
                    Api.GetCameraFeatures(hCamera, Feature.Roi, ref cf);
                    Api.SetStreamState(hCamera, StreamState.Start);
                    bufferSize = (int)cf.parameters[3].MaximumValue * (int)cf.parameters[2].MaximumValue * 2;
                    buffer = new byte[bufferSize];
                    formattedBufferSize = 0;
                    updateROI = false;
                }

                ReturnCode rc = Api.GetNextFrame(hCamera, bufferSize, buffer, ref fd);

                if (!Api.IsSuccess(rc))
                {
                    Console.WriteLine(rc);
                }

                Api.FormatImage(buffer, ref fd, ImageFormat.RawRgb24, null, ref formattedBufferSize);

                formatedBuffer = new byte[formattedBufferSize];

                Api.FormatImage(buffer, ref fd, ImageFormat.RawRgb24, formatedBuffer, ref formattedBufferSize);

                int roiWidth = fd.RoiWidth;
                int roiHeight = fd.RoiHeight;

                Application.Current.Dispatcher.Invoke(
                DispatcherPriority.SystemIdle,
                new System.Action(() =>
                {
                    still.Width = 500;
                    still.Height = still.Width * roiHeight / roiWidth;

                    bitmap = new WriteableBitmap(roiWidth, roiHeight, 300, 300, System.Windows.Media.PixelFormats.Bgr24, BitmapPalettes.WebPalette);
                    int stride = (bitmap.PixelWidth * bitmap.Format.BitsPerPixel + 7) / 8;
                    bitmap.WritePixels(new Int32Rect(0, 0, roiWidth, roiHeight), formatedBuffer, stride, 0);



                }));


            }


        }


    }
}
