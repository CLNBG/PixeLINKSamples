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

namespace PreviewInWindow
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private int m_hCamera = 0;
        private bool m_previewingEnabled;

        private void MainWindow_Loaded(object sender, System.EventArgs e)
        {
            ReturnCode rc = Api.Initialize(0, ref m_hCamera);
            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show("ERROR: Unable to initialize a camera");
                m_hCamera = 0;
            }


            //sets default window size to the current ROI of the camera
            FeatureFlags featureFlags = new FeatureFlags();
            int numberOfParameters = 4;
            float[] parameters = new float[4];
            Api.GetFeature(m_hCamera, Feature.Roi, ref featureFlags, ref numberOfParameters, parameters);
            this.Width = parameters[2];
            this.Height = parameters[3];

            ResizePreviewPictureBox();
            SetPreviewSettings();

        }


        private void MainWindow_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if ((m_previewingEnabled) && (0 != m_hCamera))
            {
                int hWnd = 0;
                Api.SetPreviewState(m_hCamera, PreviewState.Stop, ref hWnd);
            }

            ResizePreviewPictureBox();
            SetPreviewSettings();
        }

        private void ResizePreviewPictureBox()
        {
            preview.Height = this.Height;
            preview.Width = this.Width;
        }

        // Tell the PixeLINK Api to stream a preview into the picture box.
        private void SetPreviewSettings()
        {
            if (0 == m_hCamera)
            {
                return;
            }

            IntPtr windowHandle = new System.Windows.Interop.WindowInteropHelper(this).Handle;

            ReturnCode rc = Api.SetPreviewSettings(m_hCamera, "Title is ignored", (PreviewWindowStyles.Child | PreviewWindowStyles.Visible), 0, 0, Convert.ToInt16(preview.Width), Convert.ToInt16(preview.Height), (int)windowHandle, 0);

            rc = Api.SetStreamState(m_hCamera, StreamState.Start);
            int hWnd = 0;
            rc = Api.SetPreviewState(m_hCamera, PreviewState.Start, ref hWnd);

            m_previewingEnabled = true;

        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Environment.Exit(Environment.ExitCode);
        }


    }
}
