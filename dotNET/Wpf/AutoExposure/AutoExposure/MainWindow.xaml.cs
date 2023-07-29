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

//
// This demonstrates how to control a camera's autoexposure features.
//
// Option 1) Enable autoexposure (FeatureFlags.Auto)
// When autoexposure is enabled, the camera controls the exposure based on its own internal algorithm.
// The exposure will be continually adjusted over time by the camera until autoexposure is disabled.
//
// Option 2) Autoexpose Once. (FeatureFlags.OnePush)
// When initiated, the camera will adjust the exposure based on its own internal algorithm. Once
// a satisfactory exposure has been determined, the camera will release control of the exposure whereupon
// the exposure is now again settable via the SDK.
//

namespace AutoExposure
{

	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		// The camera object that we are interacting with in this application.
		Camera camera = new Camera();
		bool result = false;
		bool previewON = false;
		bool autoExposureON = false;

		public MainWindow()
		{
			InitializeComponent();
			
			this.Title = "Demo App - Auto Exposure";
			this.ResizeMode = ResizeMode.NoResize;
			
			// Initialize the camera at MainWindow start up.
			result = camera.InitializeCamera();
			if (!result)
			{
				MessageBox.Show("Camera is not initialized!", "Error");
				Application.Current.Shutdown();
				return;
			}
			else
			{
				// Load Factory Default Settings to make sure that continuous AutoExposure is not enabled.
				camera.rc = Api.LoadSettings(camera.hCamera, MemoryChannel.FactoryDefaults);
				if (!Api.IsSuccess(camera.rc))
				{
					MessageBox.Show("Factory Defualt Settings weren't loaded!", "Warning");
				}
			}
		}

		// This function starts or stops continuous AutoExposure.
		private void exposure_Click(object sender, RoutedEventArgs e) {
			
			if (camera.hCamera == 0)
			{
				MessageBox.Show("Camera is disconnected.", "Error");
				return;
			}

			// Checks whether continuous AutoExposure is enabled and memorize its state.
			autoExposureON = camera.IsAutoExposureEnabled();

			// Checks whether AutoExposureOnce is in progress.
			if (camera.IsAutoExposureOnceInProgress())
				{
					MessageBox.Show("One time Auto Exposure is in progress!", "Warning");
					return;
				}
			// Enables continuous AutoExposure or switches exposure control to Manual based on the AutoExposure state.
			autoExposureON = camera.EnableAutoExposure(autoExposureON);			
		}

		// This function starts AutoExposureOnce.
		private void onetime_exposure_Click(object sender, RoutedEventArgs e) {
			
			if (camera.IsAutoExposureEnabled())
			{
				MessageBox.Show("Disable continuous Auto Exposure!", "Error");
				return;
			}
			else if (camera.IsAutoExposureOnceInProgress())
			{
				MessageBox.Show("One time Auto Exposure is in progress.", "Warning");
				return;
			}
			else
			{
				if (!camera.StartAutoExposureOnce())
				{
					MessageBox.Show("AutoExposureOnce failed.", "Error");
					return;
				}
			}
		}

		// This function starts or stops streaming the camera and Video Preview.
		private void preview_Click(object sender, RoutedEventArgs e)
		{

			if (camera.hCamera == 0)
			{
				MessageBox.Show("Camera is disconnected!", "Error");
				return;
			}
			if (previewON == false)
			{

				if (!camera.StartPreview())
				{
					previewON = false;
					MessageBox.Show("Preview could not start!", "Error");
					return;
				}
				previewON = true;
			}
			else
			{
				if (!camera.StopPreview())
				{
					previewON = true;
					return;
				}
				previewON = false;
			}
		}

		// This function makes sure that the camera stops previewing and it is uninitialized. Otherwise MainWindow stays on.
		private void close_mainWindow(object sender, System.ComponentModel.CancelEventArgs e)
		{
			if (!result)
			{
				return;
			}
			if (!camera.StopPreview() || !camera.UninitializeCamera())
			{
				e.Cancel = true;
				MessageBox.Show("Camera is still streaming and initialized!", "Warning");
			}
		}
	}

	public class Camera
	{
		// Class variables.
		public int hCamera = 0;
		public ReturnCode rc = 0;
		public bool res;
		public int Whd = 0;
		public int myint = 0;

		// Class methods.

		public bool InitializeCamera()
		{
			// Initialize a camera
			rc = Api.Initialize(0, ref hCamera);
			return Api.IsSuccess(rc);
		}

		public bool UninitializeCamera()
		{
			// Uninitialize a camera
			rc = Api.Uninitialize(hCamera);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}
			hCamera = 0;
			return true;
		}

		// Functions for controlling Feature Exposure

		// Checks whether continuous AutoExposure is enabled
		public bool IsAutoExposureEnabled()
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] exposure = { };

			// Read current flags and parameter setting.
			rc = Api.GetFeature(hCamera, Feature.Exposure, ref flags, ref numParams, exposure);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}

			res = ((flags & FeatureFlags.Auto) == 0) ? false : true;
			return res;
		}

		// Enables continuous AutoExposure
		public bool EnableAutoExposure(bool enable)
		{

			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] exposure = new float[numParams];

			// Read the current flags and parameter setting.
			rc = Api.GetFeature(hCamera, Feature.Exposure, ref flags, ref numParams, exposure);

			if (numParams != 1)
			{
				return false;
			}
			// Set the flags for turning countinuous AutoExposure on or for switching exposure control to Manual depending on its state.
			if (!enable)
			{
				flags &= ~FeatureFlags.ModeBits;
				flags |= FeatureFlags.Auto;
			}
			else
			{
				flags &= ~FeatureFlags.ModeBits;
				flags |= FeatureFlags.Manual;
			}
			// Turn on or off countinuous AutoExposure.
			rc = Api.SetFeature(hCamera, Feature.Exposure, flags, numParams, exposure);

			enable = !enable;
			//
			// It can take a bit of time for the change to take effect in the camera.
			// Poll the camera until the change is complete (i.e. reflected in the FEATURE_FLAG_AUTO flag.
			//
			while (true)
			{
				System.Threading.Thread.Sleep(50);
				if (enable == IsAutoExposureEnabled())
				{
					break;
				}
			}

			return enable;
		}

		// Checks whether AutoExposure Once operaion is in progress 
		public bool IsAutoExposureOnceInProgress()
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] exposure = { };

			rc = Api.GetFeature(hCamera, Feature.Exposure, ref flags, ref numParams, exposure);

			if (!Api.IsSuccess(rc))
			{
				return false;
			}

			res = ((flags & FeatureFlags.OnePush) == 0) ? false : true;
			return res;
		}

		// Starts AutoExposure Once operation
		public bool StartAutoExposureOnce() {

			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] exposure = new float[numParams];

			// Read the current flags and parameter setting.
			rc = Api.GetFeature(hCamera, Feature.Exposure, ref flags, ref numParams, exposure);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}

			// Enable AutoExposureOnce.
			rc = Api.SetFeature(hCamera, Feature.Exposure, FeatureFlags.OnePush, numParams, exposure);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}

			return true;
		}
		
		// Functions for controlling camera streaming and Video Preview.

		public bool StartPreview()
		{
			// Start streaming the camera.
			rc = Api.SetStreamState(hCamera, StreamState.Start);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}

			// Enable Video Preview.
			return ResizePreviewWindow();
		}

		public bool StopPreview()
		{
			//Stop Video Preview.
			rc = Api.SetPreviewState(hCamera, PreviewState.Stop, ref Whd);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}
			//Stop streaming the camera.
			rc = Api.SetStreamState(hCamera, StreamState.Stop);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}

			return true;
		}

		public bool ResizePreviewWindow()
		{
			//Placement and size of the Preivew Window.
			int left = 900;
			int top = 120;
			int width = 800;
			int height = 600;

			if (hCamera == 0)
			{
				return false;
			}

			rc = Api.SetPreviewSettings(hCamera, "Demo Window", PreviewWindowStyles.Overlapped | PreviewWindowStyles.Visible, left, top, width, height, 0, 0);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}

			//Start Video Preview.
			rc = Api.SetPreviewState(hCamera, PreviewState.Start, ref Whd);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}
			return true;
		}
	}
}
