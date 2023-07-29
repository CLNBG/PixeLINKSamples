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
using System.Text.RegularExpressions;

using PixeLINK;

namespace FrameLossApplication
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window, INotifyPropertyChanged
    {
        public MainWindow()
        {
            InitializeComponent();
            detectCamera = new Thread(DetectCamera);
            detectCamera.Priority = ThreadPriority.Lowest;
            detectCamera.Start();
        }

        private Thread runTestTread;
        private Thread detectCamera;
		private int hCamera = 0;

		void DetectCamera()
        {
			while (true)
            {				
				ReturnCode rc = Api.Initialize(0, ref hCamera);
                if (Api.IsSuccess(rc))
                {
                    CameraInformation cameraInfo = new CameraInformation();
                    Api.GetCameraInformation(hCamera, ref cameraInfo);
					cameraName = cameraInfo.CameraName;
					RaisePropertyChanged("cameraName");
                    enableStartButton = true;
                    RaisePropertyChanged("enableStartButton");
					return;
                }
                else
                {
					cameraName = "None";
					RaisePropertyChanged("cameraName");
					enableStartButton = false;
                    RaisePropertyChanged("enableStartButton");
				}
				Thread.Sleep(250);
            }
		}

		private string cameraName = "None";
		public string CameraName
		{
			get
			{
				return cameraName;
			}
			set
			{
				cameraName = value;
			}
		}

		private bool enableStartButton = false;
        public bool EnableStartButton
        {
            get
            {
                return enableStartButton;
            }
            set
            {
                enableStartButton = value;
            }
        }

        private bool enableStopButton = false;
        public bool EnableStopButton
        {
            get
            {
                return enableStopButton;
            }
            set
            {
                enableStopButton = value;
            }
        }

		private string elapsedTime = "0.000s";
		public string ElapsedTime
		{
			get
			{
				return elapsedTime;
			}
			set
			{
				elapsedTime = value;
			}
		}

		private int expectedFrameCount = 0;
		public int ExpectedFrameCount
		{
			get
			{
				return expectedFrameCount;
			}
			set
			{
				expectedFrameCount = value;
			}
		}

		private int lostFrames = 0;
		public int LostFrames
		{
			get
			{
				return lostFrames;
			}
			set
			{
				lostFrames = value;
			}
		}

		private float descriptorFrameRate = 0.0f;
        public float DescriptorFrameRate
		{
            get
            {
                return descriptorFrameRate;
            }
            set
            {
                descriptorFrameRate = value;
            }
        }

		private float calculatedFrameRate = 0.0f;
		public float CalculatedFrameRate
		{
			get
			{
				return calculatedFrameRate;
			}
			set
			{
				calculatedFrameRate = value;
			}
		}

		private int actualFrameCount = 0;
		public int ActualFrameCount
		{
			get
			{
				return actualFrameCount;
			}
			set
			{
				actualFrameCount = value;
			}
		}

		// Alerts the view that specified properties have changed
		public event PropertyChangedEventHandler PropertyChanged;

        private void RaisePropertyChanged(string property)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(property));
            }
        }

        private Stopwatch sw;
		private static Api.Callback s_callbackDelegate;

		private void StartTestBtn_Click(object sender, RoutedEventArgs e)
        {
			ReturnCode rc;
			sw = new Stopwatch();

			if (0 == hCamera)
			{
				rc = Api.Initialize(0, ref hCamera);
				if (!Api.IsSuccess(rc))
				{
					detectCamera = new Thread(DetectCamera);
					detectCamera.Start();
					enableStartButton = false;
					RaisePropertyChanged("enableStartButton");
					return;
				}
			}

			// Set the frame callback
			s_callbackDelegate = new Api.Callback(FrameCallbackFunction);
			Api.SetCallback(hCamera, Overlays.Frame, 0, s_callbackDelegate);

			// Set initial values of the UI
			elapsedTime = "0.000s";
            RaisePropertyChanged("elapsedTime");

			expectedFrameCount = 0;
			RaisePropertyChanged("expectedFrameCount");

			lostFrames = 0;
			RaisePropertyChanged("lostFrames");

			FeatureFlags flags = new FeatureFlags();
            float[] param = new float[1];
            int numParams = 1;
			Api.GetFeature(hCamera, Feature.ActualFrameRate, ref flags, ref numParams, param);
            descriptorFrameRate = param[0];
            descriptorFrameRate = (float)(Math.Round((double)descriptorFrameRate, 2));
			RaisePropertyChanged("descriptorFrameRate");

			calculatedFrameRate = 0.0f;
			RaisePropertyChanged("calculatedFrameRate");
			
            enableStartButton = false;
			RaisePropertyChanged("enableStartButton");

			enableStopButton = true;
            RaisePropertyChanged("enableStopButton");
						
			// Start test thread
			runTestTread = new Thread(RunTest);
            runTestTread.Priority = ThreadPriority.Highest;
            runTestTread.Start();
        }

		private void EndTestBtn_Click(object sender, RoutedEventArgs e)
		{
			long startWaitPeriod;
			ReturnCode rc;

			runTestTread.Abort();
			// Stop streaming the camera
			Api.SetStreamState(hCamera, StreamState.Stop);
			// Disable the frame callback
			Api.SetCallback(hCamera, Overlays.Frame, 0, null);
			rc = Api.Uninitialize(hCamera);
			if (Api.IsSuccess(rc))
			{
				hCamera = 0;
			}

			// Wait 1 second for the frame callback queue to get processed
			startWaitPeriod = sw.ElapsedMilliseconds;
			while (sw.ElapsedMilliseconds - startWaitPeriod < 500)
			{
				Thread.Sleep(50);
			}
			sw.Stop();
			
			// Update UI with the final test results
			elapsedTime = (testRunTime * 0.001).ToString() + "s";
			RaisePropertyChanged("elapsedTime");

			expectedFrameCount = expectedFrameNum;
			RaisePropertyChanged("expectedFrameCount");

			lostFrames = lostFrameCount;
			RaisePropertyChanged("lostFrames");

            descriptorFrameRate = (float)(Math.Round((double)actualFrameRate, 2));
			RaisePropertyChanged("descriptorFrameRate");

			calculatedFrameRate = (float)(Math.Round((double)frameRate, 2));
			RaisePropertyChanged("calculatedFrameRate");

			enableStartButton = true;
			RaisePropertyChanged("enableStartButton");

			enableStopButton = false;
			RaisePropertyChanged("enableStopButton");
        }

		void UpdateUI()
        {
			elapsedTime = ((sw.ElapsedMilliseconds) * 0.001).ToString() + "s";
			descriptorFrameRate = (float)(Math.Round((double)descriptorFrameRate, 2));
			calculatedFrameRate = (float)(Math.Round((double)calculatedFrameRate, 2));
			RaisePropertyChanged("elapsedTime");
			RaisePropertyChanged("lostFrames");
			RaisePropertyChanged("expectedFrameCount");
			RaisePropertyChanged("descriptorFrameRate");
			RaisePropertyChanged("calculatedFrameRate");
        }

		private static Mutex mut = new Mutex();
		bool receivedFirstFrame = false;
		int expectedFrameNum = 0;
		int lostFrameCount = 0;
		int startFrame = 0;
		float actualFrameRate = 0.0f;
		double frameRate = 0.0f;
		double startFrameTime = 0.0f;
		long startTestTime = 0;
		long testRunTime = 0;

		void RunTest()
        {
			long startUpdatePeriod = 0;
			bool isUiUpdated = false;
			receivedFirstFrame = false;
			lostFrameCount = 0;
			// Start the Stopwatch timer
			sw.Start();
			// Start streaming the camera
			if (Api.IsSuccess(Api.SetStreamState(hCamera, StreamState.Start)))
            {
				startUpdatePeriod = sw.ElapsedMilliseconds;

				while (true)
                {   
					// Check update period time every 0.5 seconds and update UI every 2 seconds   
					if (sw.ElapsedMilliseconds - startUpdatePeriod > 2000 && !isUiUpdated)
					{
						startUpdatePeriod = sw.ElapsedMilliseconds;
						descriptorFrameRate = actualFrameRate;
						calculatedFrameRate = (float)frameRate;
						expectedFrameCount = expectedFrameNum;
						lostFrames = lostFrameCount;
						UpdateUI();
						isUiUpdated = true;
					}
					else if (isUiUpdated)
					{
						isUiUpdated = false;
					}
					Thread.Sleep(500);
                }
            }
            else {
                return;
            }
        }
		
		private int FrameCallbackFunction(int hCamera, System.IntPtr pBuf, PixeLINK.PixelFormat pf, ref FrameDescriptor frameDesc, int userData)
		{
			int frameNumDelta;
			double timePeriod;
			int elapsedFrames;

			mut.WaitOne();
			if (!receivedFirstFrame)
			{
				startTestTime = sw.ElapsedMilliseconds;
				expectedFrameNum = frameDesc.FrameNumber;
			}
			
			if (frameDesc.FrameNumber < expectedFrameNum)
			{
				// This frame is 'older' than expected.  We are just receiving this one
				// later than exected, because the Windows OS happeneded to run a new frame
				// before this one.  This frame was previouly reported as lost, so decrease
				// the lost count by 1
				lostFrameCount--;
			}
			else
			{
				// This frame is either the one expected, or one newer than expected. In this
				// latter case, we will assume that all of the missing frames are indeed lost, and
				// count them as a lost frame. However if we are wrong and the lost frame is to
				// be delivered later, than lostFrameCount will be corrected then
				frameNumDelta = frameDesc.FrameNumber - expectedFrameNum;
				lostFrameCount += frameNumDelta;
				expectedFrameNum = frameDesc.FrameNumber + 1;
			}
			
			if (!receivedFirstFrame)
			{
				startFrameTime = frameDesc.dFrameTime;
				startFrame = frameDesc.FrameNumber;
				receivedFirstFrame = true;
			}
			timePeriod = (frameDesc.dFrameTime - startFrameTime) * 1000.0f; // milliseconds
			elapsedFrames = frameDesc.FrameNumber - startFrame;

			if (timePeriod >= 50 && elapsedFrames >= 5)
			{
				frameRate = (elapsedFrames / timePeriod) * 1000.0f; // seconds
				startFrameTime = frameDesc.dFrameTime;
				startFrame = frameDesc.FrameNumber;
				actualFrameRate = frameDesc.ActualFrameRate;
			}
			
			testRunTime = sw.ElapsedMilliseconds - startTestTime;
			mut.ReleaseMutex();
			
			return 0;
		}

		private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
			Environment.Exit(Environment.ExitCode);
        }

    }
}
