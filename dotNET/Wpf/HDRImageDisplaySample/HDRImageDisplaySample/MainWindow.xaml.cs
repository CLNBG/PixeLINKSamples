//
// WPF sample code for displaying images from an HDR Pixelink camera.
//

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
using System.Runtime.InteropServices;

namespace HDRImageDisplaySample
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		/*
		 * Golabal variables
		*/
		// For MainWindow control
		bool isFullScreen = false;
		// For camera control and camera capture processing 
		int hCamera = 0;
		CameraFeature cameraFeature = new CameraFeature();
		TransferBits transfer = new TransferBits();
		int imageWidth = 0;
		int imageHeight = 0;
		BitmapImage image = null;
		public class TransferBits // TransferBits class
		{
			public int hCamera;
			public FrameDescriptor frameDesc;
			public byte[] bits;
			public byte[] FormattedBuf;
		}
		enum ImageGainType // ImageGainType enumeration
		{
			ImageGainZero = 0,
			ImageGainHdr = 1,
			ImageGainTwelve = 12
		}
		// For Thread control of displaying images
		Thread updateImage;
		bool runUpdateThread = true;
		DateTime previousTime;
		double timeCounter;
		double updateReferenceImagesTime = 15000;

		/*
		 * MainWindow and camera initialization
		*/
		public MainWindow()
		{
			InitializeComponent();

			Width = System.Windows.SystemParameters.PrimaryScreenWidth / 1.5;
			Height = System.Windows.SystemParameters.PrimaryScreenHeight / 1.5;

			// Initialize camera
			InitializeCamera();

			// Check whether the connected camera is an HDR camera
			if (!DoesCameraSupportHdr(hCamera))
			{
				MessageBox.Show("This is not an HDR camera.", "Error");
				UnintializeCamera();
				Environment.Exit(Environment.ExitCode);
			}

			// Get current camera exposure time and update the exposure time TextBox
			CameraExposureTime(true);

			// Enable gain HDR mode
			EnableCameraHdrMode();

			// Start display image update thread
			updateImage = new Thread(UpdateImage);
			updateImage.IsBackground = true;
			updateImage.Start();

		}

		/*
		 * MainWindow control functions
		*/
		//
		// Enter full screen view
		//
		private void EnterFullScreen_Button(object sender, RoutedEventArgs e)
		{
			isFullScreen = true;
			WindowStyle = WindowStyle.None;
			WindowState = WindowState.Maximized;
			FullScreenButton.Visibility = Visibility.Hidden;
			ExposureTimeInfo.Visibility = Visibility.Hidden;
			ExposureTimeBox.Visibility = Visibility.Hidden;
			ExposureTimeButton.Visibility = Visibility.Hidden;
			ExposureTimeLabel.Visibility = Visibility.Hidden;
			RefreshPeriodInfo.Visibility = Visibility.Hidden;
			RefreshPeriodBox.Visibility = Visibility.Hidden;
			RefreshPeriodButton.Visibility = Visibility.Hidden;
			RefreshPeriodLabel.Visibility = Visibility.Hidden;
		}

		//
		// Exit full screen view
		//
		private void ExitFullScreen(object sender, KeyEventArgs e)
		{
			if (e.Key == Key.Escape && isFullScreen == true)
			{
				isFullScreen = false;
				WindowState = WindowState.Normal;
				WindowStyle = WindowStyle.SingleBorderWindow;
				FullScreenButton.Visibility = Visibility.Visible;
				ExposureTimeInfo.Visibility = Visibility.Visible;
				ExposureTimeBox.Visibility = Visibility.Visible;
				ExposureTimeButton.Visibility = Visibility.Visible;
				ExposureTimeLabel.Visibility = Visibility.Visible;
				RefreshPeriodInfo.Visibility = Visibility.Visible;
				RefreshPeriodBox.Visibility = Visibility.Visible;
				RefreshPeriodButton.Visibility = Visibility.Visible;
				RefreshPeriodLabel.Visibility = Visibility.Visible;
			}
		}

		//
		// Set a new exposure time
		//
		private void ExposureTimeButton_Click(object sender, RoutedEventArgs e)
		{
			CameraExposureTime();

			// Refresh standard gain images, when new exposure time gets set
			timeCounter = 26000;
		}

		//
		// Set a new pseudo-static image refresh period
		//
		private void RefreshPeriodButton_Click(object sender, RoutedEventArgs e)
		{
			// Return an error, if requested refresh period is not within the range from 5 to 25 seconds
			if (5 > Convert.ToDouble(RefreshPeriodBox.Text) || 25 < Convert.ToDouble(RefreshPeriodBox.Text))
			{
				MessageBox.Show("Refresh period is outside of boundaries.", "Error");
				return;
			}
			updateReferenceImagesTime = Convert.ToDouble(RefreshPeriodBox.Text) * 1000;
		}

		//
		// Resize camera images when MainWindow size changes
		//
		private void Window_SizeChanged(object sender, SizeChangedEventArgs e)
		{
			imageGainZero.Width = ImageGainZeroColumn.ActualWidth;
			imageGainHDR.Width = ImageGainHdrColumn.ActualWidth;
			imageGainTwelve.Width = ImageGainTwelveColumn.ActualWidth;
		}

		//
		// Exit HDRImageDisplaySample
		//
		private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			UnintializeCamera();
			Environment.Exit(Environment.ExitCode);
		}

		/*
		 * Thread control for updating images
		*/
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
											 // Capture camera HDR image
											 CaptureImage(ImageGainType.ImageGainHdr);

											 // Check whether pseudo-static image refresh period elapsed
											 double deltaTime = (DateTime.Now - previousTime).TotalMilliseconds;
											 previousTime = DateTime.Now;
											 timeCounter += deltaTime;

											 if (timeCounter > updateReferenceImagesTime)
											 {
												 // Set standard Gain to 0 and capture an image
												 EnableStandardGain(ImageGainType.ImageGainZero);
												 CaptureImage(ImageGainType.ImageGainZero);

												 // Set standard Gain to 12 and capture an image
												 EnableStandardGain(ImageGainType.ImageGainTwelve);
												 CaptureImage(ImageGainType.ImageGainTwelve);

												 // Set gain HDR to "Camera HDR" mode
												 EnableCameraHdrMode();

												 timeCounter = 0;
											 }

										 }));
				}
				catch (Exception e)
				{
					Console.WriteLine(e);
				}
			}
		}

		/*
		 * Camera control functions
		*/

		//
		// Initializes camera
		//
		// Shows an error and exits application on error
		//
		private void InitializeCamera()
		{
			ReturnCode rc = ReturnCode.UnknownError;

			rc = Api.Initialize(0, ref hCamera);
			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString(), "Error");
				Environment.Exit(Environment.ExitCode);
			}
		}

		//
		// Uninitializes camera
		//
		private void UnintializeCamera()
		{
			Api.Uninitialize(hCamera);
		}

		//
		// Checks if gain HDR is supported by a camera.
		// If gain HDR is supported, this is an HDR camera.
		//
		// Returns true if it is the HDR camera
		//
		private bool DoesCameraSupportHdr(int hCamera)
		{
			ReturnCode rc = ReturnCode.UnknownError;

			// Is feature gian HDR supported?
			rc = Api.GetCameraFeatures(hCamera, Feature.GainHDR, ref cameraFeature);
			if (Api.IsSuccess(rc))
			{
				return cameraFeature.IsSupported;
			}

			return false;
		}

		//
		// Gets or sets camera exposure time and updates exposure time TextBox.
		//
		private void CameraExposureTime(bool getExposureTime = false)
		{
			ReturnCode rc = ReturnCode.UnknownError;
			FeatureFlags flags = FeatureFlags.Manual;
			int numParams = 3;
			float[] exposureTime = new float[numParams];

			// Get current camera exposure time
			if (true == getExposureTime)
			{
				rc = Api.GetFeature(hCamera, Feature.Exposure, ref flags, ref numParams, exposureTime);
				if (Api.IsSuccess(rc))
				{
					ExposureTimeBox.Text = Convert.ToString(exposureTime[0] * 1000);
					return;
				}
			}

			// Set exposure time to a knew value or to its default value of 30 ms, in case get expsure time returns an error.
			exposureTime[0] = ((float)Convert.ToDouble(ExposureTimeBox.Text)) / 1000;
			rc = Api.SetFeature(hCamera, Feature.Exposure, flags, numParams, exposureTime);
			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString(), "Error");
				return;
			}
		}

		//
		// Sets gain HDR mode to "Camera HDR"
		//
		// Shows an error and exits application on error
		//
		private void EnableCameraHdrMode()
		{
			ReturnCode rc = ReturnCode.UnknownError;
			int numParams = 1;
			float[] parms = new float[numParams];

			// Stop camera stream
			rc = Api.SetStreamState(hCamera, StreamState.Stop);
			ApiErrorCheck(rc);

			// Set gain HDR mode to "Camera HDR"
			parms[0] = (float)FeatureParameterIndex.HDRModeCamera;

			rc = Api.SetFeature(hCamera, Feature.GainHDR, FeatureFlags.Manual, numParams, parms);
			ApiErrorCheck(rc);

			// Start camera stream
			rc = Api.SetStreamState(hCamera, StreamState.Start);
			ApiErrorCheck(rc);
		}

		//
		// Sets standard feature Gain
		//
		// Shows an error and exits application on error
		//
		private void EnableStandardGain(ImageGainType imageGainType)
		{
			ReturnCode rc = ReturnCode.UnknownError;
			FeatureFlags flags = 0;
			int numParams = 1;                      // reused for each feature query
			float[] parms = new float[numParams];

			// Stop camera stream
			rc = Api.SetStreamState(hCamera, StreamState.Stop);
			ApiErrorCheck(rc);

			// If gain HDR mode is enabled, disable gain HDR
			rc = Api.GetFeature(hCamera, Feature.GainHDR, ref flags, ref numParams, parms);
			ApiErrorCheck(rc);

			if ((int)FeatureParameterIndex.HDRModeCamera == parms[0] || (int)FeatureParameterIndex.HDRModeInterleaved == parms[0])
			{
				rc = Api.SetFeature(hCamera, Feature.GainHDR, FeatureFlags.Off, numParams, parms);
				ApiErrorCheck(rc);
			}

			// Enable feature gain
			parms[0] = (float)imageGainType;

			rc = Api.SetFeature(hCamera, Feature.Gain, FeatureFlags.Manual, numParams, parms);
			ApiErrorCheck(rc);

			// Start camera stream
			rc = Api.SetStreamState(hCamera, StreamState.Start);
			ApiErrorCheck(rc);
		}

		//
		// Captures an image from the camera
		//
		// Shows an error and exits application on error
		//
		private void CaptureImage(ImageGainType imageGainType)
		{
			ReturnCode rc = ReturnCode.UnknownError;
			FrameDescriptor frameDesc = new FrameDescriptor();
			int destBufferSize = 0;

			// Get image buffer size
			transfer.bits = new byte[getBufferSize()];
			if (0 >= transfer.bits.Length)
			{
				UnintializeCamera();
				Environment.Exit(Environment.ExitCode);
			}

			// Get frame from camera
			rc = Api.GetNextFrame(hCamera, transfer.bits.Length, transfer.bits, ref frameDesc);
			ApiErrorCheck(rc);

			// Get buffer size of the frame streamed from the camera and format the frame to BMP format			
			rc = Api.FormatImage(transfer.bits, ref frameDesc, (PixeLINK.ImageFormat.Bmp), null, ref destBufferSize);
			ApiErrorCheck(rc);

			transfer.FormattedBuf = new byte[destBufferSize];
			rc = Api.FormatImage(transfer.bits, ref frameDesc, (PixeLINK.ImageFormat.Bmp), transfer.FormattedBuf, ref destBufferSize);
			ApiErrorCheck(rc);

			convertToBitmapImage(imageGainType);
		}

		//
		// Gets buffer size for the image
		//
		// Returns 0 on error
		//
		public int getBufferSize()
		{
			ReturnCode rc = ReturnCode.UnknownError;
			FeatureFlags flags = 0;
			int numParams = 4;
			float[] parms = new float[numParams]; // reused for each feature query
			int roiWidth, roiHeight = 0;
			int pixelAddressingValueX, pixelAddressingValueY = 1;
			int numPixels = 0;
			PixeLINK.PixelFormat format;
			float bytesPerPixel = 0f;
			int frameSize = 0;

			// Get region of interest (ROI)
			rc = Api.GetFeature(hCamera, Feature.Roi, ref flags, ref numParams, parms);
			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString(), "Error");
				return 0;
			}

			roiWidth = (int)(parms[(int)FeatureParameterIndex.RoiWidth]);
			roiHeight = (int)(parms[(int)FeatureParameterIndex.RoiHeight]);

			// Record height and width of the streamed image
			imageWidth = roiWidth;
			imageHeight = roiHeight;

			// Query pixel addressing
			rc = Api.GetFeature(hCamera, Feature.PixelAddressing, ref flags, ref numParams, parms);
			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString(), "Error");
				return 0;
			}
			// Width and height factor by which the image is reduced.
			pixelAddressingValueX = (int)parms[2];
			pixelAddressingValueY = (int)parms[3];

			// We can calulate the number of pixels now
			numPixels = (roiWidth / pixelAddressingValueX) * (roiHeight / pixelAddressingValueY);

			// Determine how many bytes per pixel
			rc = Api.GetFeature(hCamera, Feature.PixelFormat, ref flags, ref numParams, parms);
			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString(), "Error");
				return 0;
			}

			format = (PixeLINK.PixelFormat)parms[0];
			bytesPerPixel = Api.BytesPerPixel(format);

			// And now the size of the frame
			frameSize = (int)((float)numPixels * bytesPerPixel);

			// Check which gain HDR mode the camera is using			
			rc = Api.GetFeature(hCamera, Feature.GainHDR, ref flags, ref numParams, parms);
			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString(), "Error");
				return 0;
			}

			// If the camera is using the interleaved HDR mode, double the size of the raw image
			if ((int)FeatureParameterIndex.HDRModeInterleaved == parms[0])
			{
				return frameSize * 2;
			}

			return frameSize;
		}

		// 
		// Converts to Bitmap image and displays it
		//
		// Prints an exception on error
		//
		private void convertToBitmapImage(ImageGainType imageGainType)
		{
			// The BitmapImage() is the type of an object that WPF uses for displaying images
			image = new BitmapImage();

			try
			{
				using (
				// Convert byte array to BitmapImage()
				MemoryStream stream = new MemoryStream(transfer.FormattedBuf))
				{
					image.BeginInit();

					image.DecodePixelWidth = imageWidth;
					image.DecodePixelHeight = imageHeight;
					image.StreamSource = stream;
					image.CacheOption = BitmapCacheOption.OnLoad;

					image.EndInit();
				}

				// Assign the newly created BitmapImage() to display to the window
				switch (imageGainType)
				{
					case ImageGainType.ImageGainZero:
						imageGainZero.Source = image;
						imageGainZero.Width = ImageGainZeroColumn.ActualWidth;
						break;
					case ImageGainType.ImageGainTwelve:
						imageGainTwelve.Source = image;
						imageGainTwelve.Width = ImageGainTwelveColumn.ActualWidth;
						break;
					default:
						imageGainHDR.Source = image;
						imageGainHDR.Width = ImageGainHdrColumn.ActualWidth;
						break;
				}
			}
			catch (Exception e)
			{
				Console.WriteLine(e);
			}
		}

		//
		// If API returns an error, shows the error and exits the application.
		//
		private void ApiErrorCheck(ReturnCode rc)
		{
			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString(), "Error");
				UnintializeCamera();
				Environment.Exit(Environment.ExitCode);
			}
		}

	}
}

