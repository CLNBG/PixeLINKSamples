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
using System.IO;
using System.Drawing;
using PixeLINK;

// 
// A demonstration of how to configure a camera to use triggering with controlled lighting.
// By default, this demo assumes you are using GPO1 to control the lighting, and that your camera supports controlled light feature.
// If your camera does not support GPO1, you can still test triggering with controlled lighting.
//

namespace TriggerWithControlledLight
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window, INotifyPropertyChanged
	{
		private int hCamera = 0;
		private int nButtons = 0;
		private Thread runHardwareTriggerTest;
		private Thread runHardwareTriggerCountdown;
		public bool EnableFreeRunningTriggerButton { get; set; }
		public bool EnableSoftwareTriggerButton { get; set; }
		public bool EnableHardwareTriggerButton { get; set; }
		public bool EnableControlledLight { get; set; }
		public bool EnableGpioOneFlash { get; set; }
		public BitmapImage DisplayImage { get; set; }
		public string TestResultText { get; set; }

		// Alerts the view that specified properties have changed
		public event PropertyChangedEventHandler PropertyChanged;

		public MainWindow()
		{
			Title = "Demo App - TriggerWithControlledLight";
			ResizeMode = ResizeMode.NoResize;
			InitializeComponent();
			SetButtonStates(false);
			EnableControlledLight = false;
			RaisePropertyChanged("EnableControlledLight");
			EnableGpioOneFlash = false;
			RaisePropertyChanged("EnableGpioOneFlash");

			Api.Uninitialize(hCamera);

			// Initializes the camera at MainWindow start up
			ReturnCode rc = Api.Initialize(0, ref hCamera);
			if (!ErrorCheck(rc))
			{
				Application.Current.Shutdown();
				return;
			}

			// If Trigger with Controlled Lighting is supported, sets this feature
			if (IsControlledLightSupported())
			{
				if (!SetTriggerWithControlledLight())
				{
					MessageBox.Show("Setting Trigger with Controlled Lighting failed.", "Note");
					Api.Uninitialize(hCamera);
					Application.Current.Shutdown();
					return;
				}
			}
			else
			{
				MessageBox.Show("Trigger with Controlled Light is not supported.", "Note");
				Api.Uninitialize(hCamera);
				Application.Current.Shutdown();
				return;
			}

			// Checks whether Trigger is supported
			if (!IsTriggerSupported())
			{
				MessageBox.Show("Trigger feature is not supported.", "Note");
				Api.Uninitialize(hCamera);
				Application.Current.Shutdown();
				return;
			}

			// If GPIO is supported, sets GPO1 to Flash
			if (IsGpioSupported())
			{
				if (!SetGpioOneFlash())
				{
					MessageBox.Show("Setting GPO1 to Flash failed.", "Note");
				}
			}
			else
			{
				MessageBox.Show("GPIO feature is not supported.\nTesting only enabled Controlled Lighting.", "Note");
			}
		}

		private void RaisePropertyChanged(string property)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, new PropertyChangedEventArgs(property));
			}
		}		

		// Checks a return code. If it is an API error, shows it as a message.
		private bool ErrorCheck(ReturnCode rc)
		{
			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString(), "API Error");
				return false;
			}
			return true;
		}

		// Checks whether Trigger with Controlled Lighting is supported
		private bool IsControlledLightSupported()
		{
			CameraFeature featureInfo = new CameraFeature();
			ReturnCode rc = Api.GetCameraFeatures(hCamera, Feature.TriggerWithControlledLight, ref featureInfo);
			if (!ErrorCheck(rc))
			{
				return false;
			}
			return featureInfo.IsSupported;
		}

		//
		// Sets Trigger with Controlled Lighting
		/////////////////////////////////////////////////////////////////////////////////////////
		//
		// There is a right way, and a wrong way to set up controlled lighting. 
		// The right way first sets the controlled light feature and THEN sets the triggering. 
		//
		// The wrong way does it in the opposite order. 
		//
		// The reason for this is that the triggering reads the controlled light feature setting
		// only when triggering is enabled, or re-set.
		//
		/////////////////////////////////////////////////////////////////////////////////////////
		private bool SetTriggerWithControlledLight()
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] parms = new float[numParams];

			ReturnCode rc = Api.GetFeature(hCamera, Feature.TriggerWithControlledLight, ref flags, ref numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}

			parms[0] = 1.0f; // ON

			rc = Api.SetFeature(hCamera, Feature.TriggerWithControlledLight, flags, numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}

			EnableControlledLight = true;
			RaisePropertyChanged("EnableControlledLight");
			return true;
		}

		// Checks whether Trigger is supported
		private bool IsTriggerSupported()
		{
			CameraFeature featureInfo = new CameraFeature();
			ReturnCode rc = Api.GetCameraFeatures(hCamera, Feature.Trigger, ref featureInfo);
			if (!ErrorCheck(rc))
			{
				return false;
			}
			if (!featureInfo.IsSupported)
			{
				return featureInfo.IsSupported;
			}

			// Checks which trigger types are available for testing
			if (0 == featureInfo.parameters[1].MinimumValue && 2 == featureInfo.parameters[1].MaximumValue)
			{
				// Free Running, Software, and Hardware trigger types are available
				nButtons = 3;
				SetButtonStates(true);				
			}
			else if (1 == featureInfo.parameters[1].MinimumValue && 2 == featureInfo.parameters[1].MaximumValue)
			{
				// Software and Hardware trigger types are available
				nButtons = 2;
				SetButtonStates(true);				
			}
			else
			{
				// Only Software trigger type is available
				nButtons = 1;
				SetButtonStates(true);				
			}

			return featureInfo.IsSupported;
		}

		// Checks whether GPIO feature is supported
		private bool IsGpioSupported()
		{
			CameraFeature featureInfo = new CameraFeature();
			ReturnCode rc = Api.GetCameraFeatures(hCamera, Feature.Gpio, ref featureInfo);
			if (!ErrorCheck(rc))
			{
				return false;
			}
			return featureInfo.IsSupported;
		}

		// Sets GPO1 to Flash
		private bool SetGpioOneFlash()
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 6;
			float[] parms = new float[numParams];

			// Read the current settings for GPO1
			parms[(int)FeatureParameterIndex.GpioIndex] = 1;
			ReturnCode rc = Api.GetFeature(hCamera, Feature.Gpio, ref flags, ref numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}
			
			// Very important step: Enable GPO1 by clearing the FeatureFlags.Off bit
			flags &= (~FeatureFlags.Off);

			parms[(int)FeatureParameterIndex.GpioIndex] = 1.0f;
			parms[(int)FeatureParameterIndex.GpioMode] = (float)GpioMode.Flash;
			parms[(int)FeatureParameterIndex.GpioPolarity] = (float)Polarity.Positive;
			
			// And write the new settings
			rc = Api.SetFeature(hCamera, Feature.Gpio, flags, numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}

			EnableGpioOneFlash = true;
			RaisePropertyChanged("EnableGpioOneFlash");
			return true;
		}

		// Tests Free Running Trigger with Controlled Lighting
		private void TestFreeRunningTriggerButton_Click(object sender, RoutedEventArgs e)
		{
			SetButtonStates(false);
			DisplayImage = null;
			RaisePropertyChanged("DisplayImage");
			TestFreeRunningTrigger(hCamera);
			SetButtonStates(true);
		}

		// Tests Hardware Trigger with Controlled Lighting
		private void TestHardwareTriggerButton_Click(object sender, RoutedEventArgs e)
		{
			SetButtonStates(false);
			DisplayImage = null;
			RaisePropertyChanged("DisplayImage");

			// Starts a countdown that will result in a failed test if no hardware trigger input is received
			runHardwareTriggerCountdown = new Thread(HardwareTriggerCountdown);
			runHardwareTriggerCountdown.Start();
					
			// Hardware trigger test is performed in a sperate thread so that the UI doesn't freeze while waiting for the hardware trigger input
			runHardwareTriggerTest = new Thread(TestHardwareTrigger);
			runHardwareTriggerTest.IsBackground = true;
			runHardwareTriggerTest.Start();
		}
		
		// Tests Software Trigger with Controlled Lighting
		private void TestSoftwareTriggerButton_Click(object sender, RoutedEventArgs e)
		{
			SetButtonStates(false);
			DisplayImage = null;
			RaisePropertyChanged("DisplayImage");
			TestSoftwareTrigger(hCamera);
			SetButtonStates(true);
		}

		//
		// With free running trigger, starting the stream self-triggers the camera and images will be 'streamed' to the host. 
		// Calling GetNextFrame causes the camera to capture an image.
		//
		private void TestFreeRunningTrigger(int hCamera)
		{
			// Sets Free Running Trigger
			ReturnCode rc = SetTrigger(hCamera,
				0,                              // Trigger Mode 0
				TriggerType.FreeRunning,
				Polarity.Positive,
				0.0f,                           // delay = 0
				0);                             // unused in Trigger Mode 0

			if (!ErrorCheck(rc))
			{
				MessageBox.Show("Setting Trigger failed.", "Note");
				if (ReturnCode.NotPermittedWhileStreaming == rc)
				{
					rc = Api.SetStreamState(hCamera, StreamState.Stop);
					ErrorCheck(rc);
				}
				return;
			}
			
			rc = Api.SetStreamState(hCamera, StreamState.Start);
			if (ErrorCheck(rc))
			{
				// We can now grab one image
				CaptureImage(hCamera, "Free Running Test");
				rc = Api.SetStreamState(hCamera, StreamState.Stop);
				ErrorCheck(rc);
			}
		}

		//
		// With hardware triggering, the camera does not take an image until the 
		// trigger input of the machine vision connector is activated.
		//
		private void TestHardwareTrigger()
		{
			// Sets Hardware Trigger
			ReturnCode rc = SetTrigger(hCamera, 
				0,								// Trigger Mode 0
				TriggerType.Hardware, 
				Polarity.Positive, 
				0.0f,                           // delay = 0
				0.0f);                          // unused in Trigger Mode 0

			if (!ErrorCheck(rc))
			{
				MessageBox.Show("Setting Trigger failed.", "Note");
				return;
			}
			
			rc = Api.SetStreamState(hCamera, StreamState.Start);
			if (ErrorCheck(rc))
			{
				CaptureImage(hCamera, "Hardware Test");
				rc = Api.SetStreamState(hCamera, StreamState.Stop);
				ErrorCheck(rc);
			}
			
			SetButtonStates(true);
			return;
		}

		//
		// With software triggering, calling GetNextFrame causes the camera to capture an image. The camera must be in the 
		// streaming state, but no image will be 'streamed' to the host until GetNextFrame is called.
		//
		private void TestSoftwareTrigger(int hCamera)
		{
			// Sets Software Trigger
			ReturnCode rc = SetTrigger(hCamera,
				0,                              // Trigger Mode 0
				TriggerType.Software,
				Polarity.Negative,
				0.0f,                           // delay = 0
				0);                             // unused in Trigger Mode 0

			if (!ErrorCheck(rc))
			{
				MessageBox.Show("Setting Trigger failed.", "Note");
				return;
			}

			rc = Api.SetStreamState(hCamera, StreamState.Start);
			if (ErrorCheck(rc))
			{
				// We can now grab an image
				CaptureImage(hCamera, "Software Test");
				rc = Api.SetStreamState(hCamera, StreamState.Stop);
				ErrorCheck(rc);
			}

			SetButtonStates(true);
			return;
		}

		// Defines the hardware trigger test countdown thread
		private void HardwareTriggerCountdown()
		{
			int count = 10;

			while (true)
			{
				TestResultText = "Waiting for hardware trigger input. Timeout in " + count.ToString() + "s";
				RaisePropertyChanged("TestResultText");
				Thread.Sleep(1000);
				count--;

				if (count == 0)
				{
					runHardwareTriggerTest.Abort();
					TestResultText = "Hardware Test Result: No hardware trigger detected";
					RaisePropertyChanged("TestResultText");
					SetButtonStates(true);
					Api.SetStreamState(hCamera, StreamState.Stop);
					return;
				}
			}
		}

		// Sets Free Running, Software, or Hardware trigger
		private ReturnCode SetTrigger(int hCamera, int mode, TriggerType type, Polarity polarity, float delay, float triggerParam)
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 5;
			float[] parms = new float[5];

			// Read the current Trigger settings.
			ReturnCode rc = Api.GetFeature(hCamera, Feature.Trigger, ref flags, ref numParams, parms);
			if (!Api.IsSuccess(rc))
			{
				return rc;
			}

			// Very important step: Enable Trigger by clearing the FeatureFlags.Off bit
			flags &= (~FeatureFlags.Off);
			
			// Set Trigger parameters
			parms[(int)FeatureParameterIndex.TriggerMode] = (float)mode;
			parms[(int)FeatureParameterIndex.TriggerType] = (float)type;
			parms[(int)FeatureParameterIndex.TriggerPolarity] = (float)polarity;
			parms[(int)FeatureParameterIndex.TriggerDelay] = delay;
			parms[(int)FeatureParameterIndex.TriggerParameter] = triggerParam;

			rc = Api.SetFeature(hCamera, Feature.Trigger, flags, numParams, parms);

			return rc;
		}

		// Retrieves an image from the camera and displays it in the UI
		private void CaptureImage(int hCamera, string testName)
		{
			// buffer big enough for any current Pixelink camera that supports controlled lighting
			byte[] buffer = new byte[3000 * 3000 * 2]; 
			FrameDescriptor frameDesc = new FrameDescriptor();

			ReturnCode rc = Api.GetNextFrame(hCamera, buffer.Length, buffer, ref frameDesc);

			if (runHardwareTriggerCountdown != null)
			{
				runHardwareTriggerCountdown.Abort();
			}

			if (!ErrorCheck(rc))
			{
				MessageBox.Show("Error capturing image.", "Note");
				DisplayImage = new BitmapImage();
				RaisePropertyChanged("DisplayImage");
				TestResultText = testName + " Result: " + rc.ToString();
				RaisePropertyChanged("TestResultText");
				return;
			}

			int outputImageBufferSize = 5000 * 5000 * 2;
			byte[] outputImageBuffer = new byte[outputImageBufferSize];
			rc = Api.FormatImage(buffer, ref frameDesc, ImageFormat.Bmp, outputImageBuffer, ref outputImageBufferSize);
			if (!ErrorCheck(rc))
			{
				MessageBox.Show("Error formatting image.", "Note");
				DisplayImage = new BitmapImage();
				RaisePropertyChanged("DisplayImage");
				TestResultText = testName + " Result: " + rc.ToString();
				RaisePropertyChanged("TestResultText");
				return;
			}

			TestResultText = testName + " Result: " + rc.ToString();
			RaisePropertyChanged("TestResultText");

			// Converts Bitmap to a BitmapImage so that it can be displayed in the UI window
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

		// Disables Trigger with Controlled Lighting
		private bool DisableControlledLight()
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] parms = new float[numParams];

			ReturnCode rc = Api.GetFeature(hCamera, Feature.TriggerWithControlledLight, ref flags, ref numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}

			parms[0] = 0.0f; // OFF

			rc = Api.SetFeature(hCamera, Feature.TriggerWithControlledLight, flags, numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}

			return true;
		}

		// Disables Trigger
		private bool DisableTrigger()
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 5;
			float[] parms = new float[5];
			ReturnCode rc = Api.GetFeature(hCamera, Feature.Trigger, ref flags, ref numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}

			// Very important step: Disable Trigger by setting the FeatureFlags.Off bit
			flags |= FeatureFlags.Off;

			rc = Api.SetFeature(hCamera, Feature.Trigger, flags, numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}
			return true;
		}

		// Disables GPO1
		private bool DisableGpioOneFlash()
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 6;
			float[] parms = new float[6];

			// Read the current settings for GPO1
			parms[(int)FeatureParameterIndex.GpioIndex] = 1;
			ReturnCode rc = Api.GetFeature(hCamera, Feature.Gpio, ref flags, ref numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}

			// Very important step: Disable Trigger by setting the FeatureFlags.Off bit
			flags |= FeatureFlags.Off;

			rc = Api.SetFeature(hCamera, Feature.Gpio, flags, numParams, parms);
			if (!ErrorCheck(rc))
			{
				return false;
			}
			return true;
		}

		// Controls Free Running, Software, and Hardware trigger test button states
		private void SetButtonStates(bool state)
		{
			if (state)
			{
				switch (nButtons)
				{
					case 1:
						EnableSoftwareTriggerButton = true;
						RaisePropertyChanged("EnableSoftwareTriggerButton");
						break;

					case 2:
						EnableSoftwareTriggerButton = true;
						RaisePropertyChanged("EnableSoftwareTriggerButton");
						EnableHardwareTriggerButton = true;
						RaisePropertyChanged("EnableHardwareTriggerButton");
						break;

					default:
						EnableFreeRunningTriggerButton = true;
						RaisePropertyChanged("EnableFreeRunningTriggerButton");
						EnableSoftwareTriggerButton = true;
						RaisePropertyChanged("EnableSoftwareTriggerButton");
						EnableHardwareTriggerButton = true;
						RaisePropertyChanged("EnableHardwareTriggerButton");
						break;
				}
			}
			else
			{
				EnableFreeRunningTriggerButton = false;
				RaisePropertyChanged("EnableFreeRunningTriggerButton");
				EnableSoftwareTriggerButton = false;
				RaisePropertyChanged("EnableSoftwareTriggerButton");
				EnableHardwareTriggerButton = false;
				RaisePropertyChanged("EnableHardwareTriggerButton");
			}
		}

		// On Window close, disables Controlled Lighiting, Trigger, GPO1 Flash, and uninitializes the camera
		private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			if (!DisableControlledLight())
			{
				MessageBox.Show("Disabling Controlled Lighting failed.", "Note");
			}

			if (!DisableTrigger())
			{
				MessageBox.Show("Disabling Trigger failed.", "Note");
			}

			if (!DisableGpioOneFlash())
			{
				MessageBox.Show("Disabling GPO1 failed.", "Note");
			}

			Api.Uninitialize(hCamera);

			Environment.Exit(Environment.ExitCode);
		}
	}
}
