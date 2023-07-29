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
using System.Threading;
using System.Windows.Threading;
using System.IO;

namespace PolarFourQuadrant
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            Api.Initialize(0, ref hCamera);
            float[] parm = { (float)PixeLINK.PixelFormat.PolarRaw412 };
            Api.SetFeature(hCamera, Feature.PixelFormat, FeatureFlags.Manual, 1, parm);
            Api.SetStreamState(hCamera, StreamState.Start);
            updateThread = new Thread(new ThreadStart(UpdateImage));
            updateThread.IsBackground = true;
            runThread = true;
            updateThread.Start();
        }

        int hCamera;
        Thread updateThread;
        bool runThread = false;

        public class TransferBits
        {
            public FrameDescriptor frameDesc;
            public byte[] bits;
            public byte[] FormattedBuf;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            runThread = false;
            Api.SetStreamState(hCamera, StreamState.Stop);
            Api.Uninitialize(hCamera);

        }

        TransferBits transfer = new TransferBits();
        int imageHeight = 0;
        int imageWidth = 0;

        byte[] quadrantArray;

        void UpdateImage()
        {
            while (runThread)
            {

                try
                {
                    Application.Current.Dispatcher.Invoke(
                                     DispatcherPriority.SystemIdle,
                                         new Action(() =>
                                         {
                                             transfer.bits = new byte[getBufferSize()];
                                             FrameDescriptor frameDesc = new FrameDescriptor();
                                        
                                             //gets frame from camera
                                             ReturnCode rc = Api.GetNextFrame(hCamera, transfer.bits.Length, transfer.bits, ref frameDesc);


                                             int frameRows = frameDesc.RoiHeight * 2;
                                             int bytesPerRow = frameDesc.RoiWidth * 3;

                                             quadrantArray = new byte[frameDesc.RoiHeight * 2 * frameDesc.RoiWidth * 2];


                                             for (int y = 0; y < frameRows - 1; y += 2)
                                             {
                                                 for (int x = 0; x < bytesPerRow - 1; x += 3)
                                                 {
                                                    // In PolarRaw the data is packed as follows
                                                    //    even rows --> 90   45  90   45  90   45
                                                    //    odd rows  --> 135  0   135  0   135  0

                                                    quadrantArray[(y * 1) * frameDesc.RoiWidth + (x / 3)] = transfer.bits[(y + 1) * bytesPerRow + x + 1]; 
                                                     //display at 0 quadrant

                                                    quadrantArray[(y * 1) * frameDesc.RoiWidth + (x / 3) + frameDesc.RoiWidth] = transfer.bits[y * bytesPerRow + x + 1]; 
                                                     //display at 45 quadrant

                                                    quadrantArray[(y + frameRows) * frameDesc.RoiWidth + (x / 3) + frameDesc.RoiWidth] = transfer.bits[y * bytesPerRow + x]; 
                                                     //display at 90 quadrant

                                                    quadrantArray[(y + frameRows) * frameDesc.RoiWidth + (x / 3)] = transfer.bits[(y + 1) * bytesPerRow + x]; 
                                                     //display at 135 quadrant

                                                 }
                                             }

                                             try
                                             {

                                                 //convert byte array into a displayable bitmap
                                                 var width = frameDesc.RoiWidth * 2;
                                                 var height = frameDesc.RoiHeight * 2;
                                                 var dpiX = 96d;
                                                 var dpiY = 96d;
                                                 var pixelFormat = PixelFormats.Gray8; // for example
                                                 var bytesPerPixel = (pixelFormat.BitsPerPixel + 7) / 8;
                                                 var stride = bytesPerPixel * width;
                                                 var bitmap = BitmapSource.Create(width, height, dpiX, dpiY, pixelFormat, null, quadrantArray, stride);

                                                 //adjust window height to fit image
                                                 double rowHeight = (this.ActualHeight - 150) / 2;
                                                 double columnWidth = rowHeight * ((double)frameDesc.RoiWidth / (double)frameDesc.RoiHeight);
                                                 ColumnDefinition cd = new ColumnDefinition();
                                                 cd.Width = new GridLength(columnWidth, GridUnitType.Pixel);
                                                 Column0.Width = cd.Width;
                                                 Column1.Width = cd.Width;
                                                 RowDefinition rd = new RowDefinition();
                                                 rd.Height = new GridLength(rowHeight, GridUnitType.Pixel);
                                                 Row0.Height = rd.Height;
                                                 Row1.Height = rd.Height;

                                                 //Update Image
                                                 DisplayImage.Source = bitmap;                                              

                                                 

                                             }
                                             catch (Exception e)
                                             {
                                                 Console.WriteLine(e);
                                             }


                                         }));
                }
                catch (Exception e)
                {
                    Console.WriteLine(e);
                }
            }
        }

        //gets the size of the image buffer
        public int getBufferSize()
        {
            FeatureFlags flags = 0;
            int numParms = 1;
            float[] parms = new float[numParms];
            Api.GetFeature(hCamera, Feature.PixelFormat, ref flags, ref numParms, parms);
            float bytesPerPixel = Api.BytesPerPixel((PixeLINK.PixelFormat)parms[0]);

            return (int)(bytesPerPixel * (float)GetNumPixels());
        }

        //gets the number of pixels of the streamed image
        public int GetNumPixels()
        {
            int numParms = 4;
            FeatureFlags flags = 0;
            float[] parms = new float[numParms];
            ReturnCode rc = Api.GetFeature(hCamera, Feature.Roi, ref flags, ref numParms, parms);

            int width = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.RoiWidth]);
            int height = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.RoiHeight]);

            imageHeight = height;
            imageWidth = width;

            return (width * height);

        }

        bool isFullScreen = false;

        private void FullScreen_Click(object sender, RoutedEventArgs e)
        {
            if (isFullScreen == true)
            {
                isFullScreen = false;
                WindowState = WindowState.Normal;
                WindowStyle = WindowStyle.SingleBorderWindow;
                FullscreenButton.Visibility = Visibility.Visible;
            }
            else
            {
                isFullScreen = true;
                WindowStyle = WindowStyle.None;
                WindowState = WindowState.Maximized;
                FullscreenButton.Visibility = Visibility.Hidden;
            }
        }

        private void Window_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Escape && isFullScreen == true)
            {
                isFullScreen = false;
                WindowState = WindowState.Normal;
                WindowStyle = WindowStyle.SingleBorderWindow;
                FullscreenButton.Visibility = Visibility.Visible;
            }
        }

    }
}
