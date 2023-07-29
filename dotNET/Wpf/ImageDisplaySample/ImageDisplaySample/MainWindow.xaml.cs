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
using System.IO;
using System.Threading;
using System.Windows.Threading;

namespace ImageDisplaySample
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        int hCamera = 0;
        TransferBits transfer = new TransferBits();
        PixeLINK.PixelFormat format;
        BitmapImage image = null;
        int imageHeight = 0;
        int imageWidth = 0;
        Thread updateImage;
        bool runUpdateThread = true;

        public MainWindow()
        {
            InitializeComponent();

            //initialize camera
            ReturnCode rc = Api.Initialize(0, ref hCamera);

            if (Api.IsSuccess(rc))
            {
                //start camera stream
                rc = Api.SetStreamState(hCamera, StreamState.Start);

                //start display image update thread
                updateImage = new Thread(UpdateImage);
                updateImage.IsBackground = true;
                updateImage.Start();
            }
        }

        void UpdateImage()
        {
            while (runUpdateThread)
            {

                try
                {
                    Application.Current.Dispatcher.Invoke(
                        DispatcherPriority.SystemIdle,
                            new System.Action(() =>
                            {
                                transfer.bits = new byte[getBufferSize()];
                                FrameDescriptor frameDesc = new FrameDescriptor();
                                int destBufferSize = 0;

                                //gets frame from camera
                                ReturnCode rc = Api.GetNextFrame(hCamera, transfer.bits.Length, transfer.bits, ref frameDesc);

                                //gets the buffer size of the frame streamed from the camera and formats the frame to BMP format
                                Api.FormatImage(transfer.bits, ref frameDesc, (PixeLINK.ImageFormat.Bmp), null, ref destBufferSize);
                                transfer.FormattedBuf = new byte[destBufferSize];
                                rc = Api.FormatImage(transfer.bits, ref frameDesc, (PixeLINK.ImageFormat.Bmp), transfer.FormattedBuf, ref destBufferSize);

                                // the BitmapImage() is the type of object that WPF uses for displaying images
                                image = new BitmapImage();


                                //draws a crosshair in the middle of the frame
                                transfer.FormattedBuf = SetCrosshairOverlay(transfer.FormattedBuf);

                                try
                                {

                                    using (
                                    //converts byte array to BitmapImage()
                                    MemoryStream stream = new MemoryStream(transfer.FormattedBuf))
                                    {
                                        image.BeginInit();

                                        image.DecodePixelHeight = imageHeight;
                                        image.DecodePixelWidth = imageWidth;

                                        image.StreamSource = stream;
                                        image.CacheOption = BitmapCacheOption.OnLoad;
                                        image.EndInit();


                                    }

                                    //assigned the newly created BitmapImage() to display to the window
                                    still.Source = image;

                                    double scale = .75 * System.Windows.SystemParameters.PrimaryScreenHeight / imageHeight;

                                    this.Height = imageHeight * scale;
                                    this.Width = imageWidth * scale;

                                    still.Height = this.Height;
                                    still.Width = this.Width;

                                }
                                catch (Exception e)
                                {
                                    Console.WriteLine(e);
                                }


                            }));
                }
                catch
                {

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
            format = (PixeLINK.PixelFormat)parms[0];
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


            //records height and width of the streamed image
            imageHeight = height;
            imageWidth = width;

            return (width * height) ;

        }

        private byte[] SetCrosshairOverlay(byte[] photoBytes)
        {
            switch (format)
            {
                case PixeLINK.PixelFormat.Mono16:
                case PixeLINK.PixelFormat.Mono12Packed:
                case PixeLINK.PixelFormat.Mono8:
                case PixeLINK.PixelFormat.Mono12PackedMsfirst:
                case PixeLINK.PixelFormat.Mono10PackedMsfirst:
                    int firstPixelIndex;

                    int crosshairSize = (imageHeight * .1) < (imageWidth * .1) ? (int)(imageHeight * .1) : (int)(imageWidth * .1);

                    for (int i = 0; i < imageHeight; i++)
                    {
                        firstPixelIndex = 1078 + (i) * imageWidth + (imageWidth / 2);
                        photoBytes[firstPixelIndex] = 255;
                        firstPixelIndex = 1078 + (i) * imageWidth + ((imageWidth / 2) - 1);
                        photoBytes[firstPixelIndex] = 255;
                        firstPixelIndex = 1078 + (i) * imageWidth + ((imageWidth / 2) + 1);
                        photoBytes[firstPixelIndex] = 255;
                    }

                    for (int i = 0; i < imageWidth; i++)
                    {
                        firstPixelIndex = 1078 + ((imageHeight / 2) - 1) * imageWidth + (i);
                        photoBytes[firstPixelIndex] = 255;
                        firstPixelIndex = 1078 + ((imageHeight / 2) - 0) * imageWidth + (i);
                        photoBytes[firstPixelIndex] = 255;
                        firstPixelIndex = 1078 + ((imageHeight / 2) + 1) * imageWidth + (i);
                        photoBytes[firstPixelIndex] = 255;
                    }


                    return photoBytes;

                case PixeLINK.PixelFormat.Bayer8BGGR:
                case PixeLINK.PixelFormat.Bayer8GBRG:
                case PixeLINK.PixelFormat.Bayer8GRBG:
                case PixeLINK.PixelFormat.Bayer8RGGB:
                case PixeLINK.PixelFormat.Bayer16BGGR:
                case PixeLINK.PixelFormat.Bayer16GBRG:
                case PixeLINK.PixelFormat.Bayer16GRBG:
                case PixeLINK.PixelFormat.Bayer16RGGB:
                case PixeLINK.PixelFormat.Yuv422:
                case PixeLINK.PixelFormat.Rgb24:
                case PixeLINK.PixelFormat.Rgb48:
                case PixeLINK.PixelFormat.Bayer12BGGRPacked:
                case PixeLINK.PixelFormat.Bayer12GBRGPacked:
                case PixeLINK.PixelFormat.Bayer12GRBGPacked:
                case PixeLINK.PixelFormat.Bayer12RGGBPacked:
                case PixeLINK.PixelFormat.Bayer12GRBGPackedMsfirst:
                case PixeLINK.PixelFormat.Bayer12RGGBPackedMsfirst:
                case PixeLINK.PixelFormat.Bayer12GBRGPackedMsfirst:
                case PixeLINK.PixelFormat.Bayer12BGGRPackedMsfirst:
                case PixeLINK.PixelFormat.Bayer10BGGRPackedMsfirst:
                case PixeLINK.PixelFormat.Bayer10GBRGPackedMsfirst:
                case PixeLINK.PixelFormat.Bayer10PackedMsfirst:
                case PixeLINK.PixelFormat.Bayer10RGGBPackedMsfirst:

                    double crosshairSizeMultipleOfThree = (imageHeight * .1) < (imageWidth * .1) ? (int)(imageHeight * .1) : (int)(imageWidth * .1);

                    crosshairSize = (int)(Math.Round((crosshairSizeMultipleOfThree / 3)) * 3);


                    for (int i = 0; i < imageHeight ; i++)
                    {
                        firstPixelIndex = 54 + (i) * (imageWidth * 3) + (((imageWidth * 3) / 2) - 3);
                        photoBytes[firstPixelIndex] = 255;
                        photoBytes[firstPixelIndex + 1] = 255;
                        photoBytes[firstPixelIndex + 2] = 255;

                        firstPixelIndex = 54 + (i) * (imageWidth * 3) + (((imageWidth * 3) / 2) - 0);
                        photoBytes[firstPixelIndex] = 255;
                        photoBytes[firstPixelIndex + 1] = 255;
                        photoBytes[firstPixelIndex + 2] = 255;

                        firstPixelIndex = 54 + (i) * (imageWidth * 3) + (((imageWidth * 3) / 2) + 3);
                        photoBytes[firstPixelIndex] = 255;
                        photoBytes[firstPixelIndex + 1] = 255;
                        photoBytes[firstPixelIndex + 2] = 255;
                    }

                    for (int i = 1; i < imageWidth + 1; i++)
                    {


                        firstPixelIndex = 54 + ((imageHeight / 2)) * (imageWidth * 3) + ( (3 * i) );
                        photoBytes[firstPixelIndex] = 255;
                        photoBytes[firstPixelIndex + 1] = 255;
                        photoBytes[firstPixelIndex + 2] = 255;

                        firstPixelIndex = 54 + ((imageHeight / 2) - 1) * (imageWidth * 3) + ((3 * i) );
                        photoBytes[firstPixelIndex] = 255;
                        photoBytes[firstPixelIndex + 1] = 255;
                        photoBytes[firstPixelIndex + 2] = 255;

                        firstPixelIndex = 54 + ((imageHeight / 2) + 1) * (imageWidth * 3) + ((3 * i));
                        photoBytes[firstPixelIndex] = 255;
                        photoBytes[firstPixelIndex + 1] = 255;
                        photoBytes[firstPixelIndex + 2] = 255;


                    }

                    return photoBytes;

            }
            return photoBytes;
        }

        private void OnApplicationExit(object sender, EventArgs e)
        {
            //ends thread and stops the camera stream
            runUpdateThread = false;
            Api.SetStreamState(hCamera, StreamState.Stop);
        }
    }

    public class TransferBits
    {
        public int hCamera;
        public FrameDescriptor frameDesc;
        public byte[] bits;
        public byte[] FormattedBuf;
    }
}
