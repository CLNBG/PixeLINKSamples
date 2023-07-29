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

namespace AutoWhiteBalance
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {

        private int s_hCamera = 0;
        public MainWindow()
        {
            InitializeComponent();
            this.ResizeMode = ResizeMode.NoResize;
            whiteBalance.IsEnabled = false;
            this.Title = "Auto White Balance";
        }

        private void initialize_Click(object sender, RoutedEventArgs e)
        {
            Cursor originalCursor = this.Cursor;
            this.Cursor = Cursors.Wait;

            int hCamera = 0;
            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show("ERROR: Unable to initialize a camera");
            }
            else
            {
                rc = Api.SetStreamState(hCamera, StreamState.Start);
                if (!Api.IsSuccess(rc))
                {
                    Api.Uninitialize(hCamera);
                    MessageBox.Show("ERROR: Unable to start the image stream");
                }
                else
                {
                    s_hCamera = hCamera;
                    whiteBalance.IsEnabled = true;
                    initialize.IsEnabled = false;
                }
            }

            this.Cursor = originalCursor;
        }

        private void whiteBalance_Click(object sender, RoutedEventArgs e)
        {

            Cursor originalCursor = this.Cursor;
            this.Cursor = Cursors.Wait;

            System.Diagnostics.Debug.Assert(0 != s_hCamera);

            // Start the white balance
            PixeLINK.FeatureFlags flags = 0;
            int numParams = 3;
            float[] parameters = new float[numParams];
            ReturnCode rc = Api.GetFeature(s_hCamera, Feature.WhiteShading, ref flags, ref numParams, parameters);
            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show("ERROR: Unable to read the white balance.");
            }
            else
            {
                flags = FeatureFlags.OnePush;
                rc = Api.SetFeature(s_hCamera, Feature.WhiteShading, flags, numParams, parameters);
                if (!Api.IsSuccess(rc))
                {
                    MessageBox.Show("ERROR: Unable to start the white balance.");
                }
                else
                {
                    rc = WaitForAutoWhiteBalanceToComplete();
                    if (!Api.IsSuccess(rc))
                    {
                        MessageBox.Show("ERROR: Unable to perform the white balance.");
                    }
                    else
                    {
                        MessageBox.Show("White balance complete.");
                    }
                }
            }

            this.Cursor = originalCursor;
        }

        private ReturnCode WaitForAutoWhiteBalanceToComplete()
        {
            PixeLINK.FeatureFlags flags = 0;
            int numParams = 3;
            float[] parameters = new float[numParams];
            ReturnCode rc = ReturnCode.UnknownError;
            for (int i = 0; i < 20; i++)
            {
                rc = Api.GetFeature(s_hCamera, Feature.WhiteShading, ref flags, ref numParams, parameters);
                if (!Api.IsSuccess(rc))
                {
                    return rc;
                }
                // Is it done?
                if (0 == (flags & FeatureFlags.OnePush))
                {
                    return rc;
                }

                System.Threading.Thread.Sleep(1000);
            }

            return ReturnCode.TimeoutError;

        }
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            Environment.Exit(Environment.ExitCode);
        }


    }
}
