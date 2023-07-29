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
// Demonstrates how to enable and set frame rate
//
// The functions EnableManualFrameRateControl() and SetFrameRate() are only functions that you probably need.
// Their functionality can also be combined into one.
// The other functions are used for demonstrating the frame rate control result.
//

namespace Framerate
{
	/// <summary>
	/// Interaction logic for MainWindow.xaml
	/// </summary>
	public partial class MainWindow : Window
	{
		// The camera object that we are interacting with in this application.
		Camera camera = new Camera();
		bool result = false;
		public static bool previewON = false;
		public static bool isFrameRateControlOn = false;


		public MainWindow()
		{
			InitializeComponent();
			Title = "Demo App - Framerate";
			ResizeMode = ResizeMode.NoResize;

			// Initialize the camera and load Factoy Default Settings at MainWindow start up
			result = camera.InitializeCamera();
			if (!result)
			{
				MessageBox.Show("Camera is not initialized!", "Error");
				Application.Current.Shutdown();
				return;
			}

			// Load Factory Default Settings 
			camera.rc = Api.LoadSettings(camera.hCamera, MemoryChannel.FactoryDefaults);
			if (!Api.IsSuccess(camera.rc))
			{
				MessageBox.Show("Factory Defualt Settings couldn't be loaded!", "Error");
				camera.UninitializeCamera();
				Application.Current.Shutdown();
			}

			// Check current frame rate value and update textBox
			textBox.Text = camera.CurrentFrameRate(textBox.Text);
		}

		//Enable/disable manual Frame Rate control. If maunal Frame Rate control is disabled, Auto Frame Rate is enabled.
		private void enableFrameRate_Click(object sender, RoutedEventArgs e)
		{
			// Enable manual Frame Rate control and memorize its state
			isFrameRateControlOn = camera.EnableManualFrameRateControl(isFrameRateControlOn);
			// Check current frame rate value and update textBox
			textBox.Text = camera.CurrentFrameRate(textBox.Text);
		}

		private void setFrameRate_Click(object sender, RoutedEventArgs e)
		{
			// Set a new Frame Rate value
			camera.SetFrameRate(textBox.Text);
			// Check current frame rate value and update textBox
			textBox.Text = camera.CurrentFrameRate(textBox.Text);
		}
		
		private void preview_Click(object sender, RoutedEventArgs e)
		{
			// This function starts or stops streaming the camera and Video Preview
			if (camera.hCamera == 0)
			{
				MessageBox.Show("Camera is disconnected!", "Error");
				return;
			}
			if (!previewON)
			{
				if (!camera.SetandStartPreview())
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
		public bool streamState = false;
		public bool previewPaused = false;

		// Class methods.
		public bool InitializeCamera()
		{
			// Initialize a camera
			rc = Api.Initialize(0, ref hCamera);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}

			return true;
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

		// Get the current frame rate value
		public string CurrentFrameRate(string text)
		{

			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] fpsParams = new float[numParams];

			rc = Api.GetFeature(hCamera, Feature.FrameRate, ref flags, ref numParams, fpsParams);

			if (!Api.IsSuccess(rc))
			{
				MessageBox.Show(rc.ToString());
				return text;
			}
			
			text = (string)Convert.ToString(Math.Round(fpsParams[0], 3));
			
			return text;
		}

		// Enable manual Frame Rate control
		public bool EnableManualFrameRateControl(bool enable)
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] fpsParams = new float[numParams];

			// Read the current flags and parameter setting.
			rc = Api.GetFeature(hCamera, Feature.FrameRate, ref flags, ref numParams, fpsParams);

			if (numParams != 1)
			{
				return false;
			}
			// Set the flags for turning Frame Rate manual control on or off depending on its state.
			if (!enable)
			{
				flags &= ~FeatureFlags.ModeBits;
				flags |= FeatureFlags.Manual;
			}
			else
			{
				flags &= ~FeatureFlags.ModeBits;
				flags |= FeatureFlags.Auto;
			}

			// Copy the original stream state and stop streaming a camera.
			// For avoiding input unavailable message, Video Preview can be paused during settings change.
			streamState = MainWindow.previewON;
			if (streamState)
			{
				PausePreview();
				streamControl(false);
			}
			rc = Api.SetFeature(hCamera, Feature.FrameRate, flags, numParams, fpsParams);
			if (!Api.IsSuccess(rc))
			{
				// Recover camera stream and Video Preview original state.
				streamControl(MainWindow.previewON);
				if (previewPaused)
				{
					StartPreview();
				}
				MessageBox.Show("Frame Rate manual control wasn't enabled!", "Error");
				return enable;
			}
			// Recover camera stream and Video Preview original state.
			streamControl(MainWindow.previewON);
			if (previewPaused)
			{
				StartPreview();
			}
			enable = !enable;
			return enable;
		}

		// Set Frame Rate to a new value
		public void SetFrameRate(string valueFPS)
		{
			FeatureFlags flags = new FeatureFlags();
			int numParams = 1;
			float[] fpsParams = new float[numParams];

			// Read the current flags, so they can be used as it is.
			rc = Api.GetFeature(hCamera, Feature.FrameRate, ref flags, ref numParams, fpsParams);

			fpsParams[0] = (float)Convert.ToDouble(valueFPS);

			// Copy the original stream state and stop streaming a camera.
			// For avoiding input unavailable message, Video Preview can be paused during settings change.
			streamState = MainWindow.previewON;
			if (streamState)
			{
				PausePreview();
				streamControl(false);
			}
			rc = Api.SetFeature(hCamera, Feature.FrameRate, flags, numParams, fpsParams);
			if (!Api.IsSuccess(rc))
			{
				string message = rc.ToString();
				// Recover camera stream and Video Preview original state.
				streamControl(MainWindow.previewON);
				if (previewPaused)
				{
					StartPreview();
				}
				MessageBox.Show(message, "Error");
				return;
			}

			// Recover camera stream and Video Preview original state.
			streamControl(MainWindow.previewON);
			if (previewPaused)
			{
				StartPreview();
			}
			if (!MainWindow.isFrameRateControlOn)
			{
				MessageBox.Show("Manual Frame Rate control is not enabled!", "Warning");
			}
		}
		
		// Control camera stream state		
		public void streamControl(bool enable)
		{
			// Turns camera stream ON and OFF upon request.
			if (!streamState && enable)
			{
				rc = Api.SetStreamState(hCamera, StreamState.Start);
				if (Api.IsSuccess(rc))
				{
					streamState = true;
					return;
				}
			}
			if (streamState)
			{
				rc = Api.SetStreamState(hCamera, StreamState.Stop);
				if (Api.IsSuccess(rc))
				{
					streamState = false;
				}
			}
		}

		// Functions for controlling camera streaming and Video Preview.
		public bool SetandStartPreview()
		{
			// Start streaming the camera.
			rc = Api.SetStreamState(hCamera, StreamState.Start);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}
			
			// Set and start Video Preview.
			return ResizePreviewWindow();
		}

		public bool StartPreview() {

			// Start Video Preview
			rc = Api.SetPreviewState(hCamera, PreviewState.Start, ref Whd);
			if (!Api.IsSuccess(rc))
			{
				return false;
			}
			return true;
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

		public bool PausePreview()
		{
			// Pause Video Preview
			rc = Api.SetPreviewState(hCamera, PreviewState.Pause, ref Whd);
			if (!Api.IsSuccess(rc))
			{
				return previewPaused = false;
			}
			return previewPaused = true;
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
