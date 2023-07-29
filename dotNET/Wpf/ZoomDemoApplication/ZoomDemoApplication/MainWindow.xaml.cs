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
using System.Text.RegularExpressions;
using System.Threading;

namespace ZoomDemoApplication
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    /// 
    /// 
    /// This appliation demonstrates how to adjust the zoom level when a Pixelink Camera is being used with a motorized lens controlled by a Navitar motor controller


    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            Mouse.OverrideCursor = Cursors.Wait;

            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (Api.IsSuccess(rc))
            {
                int numberControllerInfo = 4;
                ControllerInformation[] controllerInfo = new ControllerInformation[numberControllerInfo];

                int controllerInfoSize = System.Runtime.InteropServices.Marshal.SizeOf(new ControllerInformation());


                Api.GetNumberControllers(controllerInfo, controllerInfoSize, ref numberControllerInfo);


                rc = Api.AssignController(hCamera, controllerInfo[0].ControllerSerialNumber);

                IList<ControllerInformation> controllerInformationList = new List<ControllerInformation>();

                foreach (ControllerInformation c in controllerInfo)
                {
                    if (c.ControllerSerialNumber > 0)
                    {
                        controllerInformationList.Add(c);
                    }
                }



            }
            else
            {
                string msg = String.Format("Error initializing camera\nReturn code: {0} (0x{1:X})", rc, rc);
                MessageBox.Show(this, msg);
            }

            SetInitialValuesZoom();

            Mouse.OverrideCursor = null;
        }

        private void SetInitialValuesZoom()
        {
            FeatureFlags flags = new FeatureFlags();
            float[] parms = new float[1];
            int numParms = 1;
            ReturnCode rc = Api.GetFeature(hCamera, Feature.Zoom, ref flags, ref numParms, parms);

            CameraFeature cf = new CameraFeature();
            Api.GetCameraFeatures(hCamera, Feature.Zoom, ref cf);
            Controls.IsEnabled = true;

            sliderZoom1.Minimum = cf.parameters[0].MinimumValue;
            sliderZoom1.Maximum = cf.parameters[0].MaximumValue;
            sliderZoom1.Value = sliderZoom1.Minimum;
            zoomMinValue1.Content = sliderZoom1.Minimum.ToString();
            zoomMaxValue1.Content = sliderZoom1.Maximum.ToString();

            sliderZoom2.Minimum = cf.parameters[0].MinimumValue;
            sliderZoom2.Maximum = cf.parameters[0].MaximumValue;
            sliderZoom2.Value = sliderZoom2.Minimum;
            zoomMinValue2.Content = sliderZoom2.Minimum.ToString();
            zoomMaxValue2.Content = sliderZoom2.Maximum.ToString();

            sliderZoom3.Minimum = cf.parameters[0].MinimumValue;
            sliderZoom3.Maximum = cf.parameters[0].MaximumValue;
            sliderZoom3.Value = sliderZoom3.Minimum;
            zoomMinValue3.Content = sliderZoom3.Minimum.ToString();
            zoomMaxValue3.Content = sliderZoom3.Maximum.ToString();

            sliderZoom4.Minimum = cf.parameters[0].MinimumValue;
            sliderZoom4.Maximum = cf.parameters[0].MaximumValue;
            sliderZoom4.Value = sliderZoom4.Minimum;
            zoomMinValue4.Content = sliderZoom4.Minimum.ToString();
            zoomMaxValue4.Content = sliderZoom4.Maximum.ToString();

            sliderZoom5.Minimum = cf.parameters[0].MinimumValue;
            sliderZoom5.Maximum = cf.parameters[0].MaximumValue;
            sliderZoom5.Value = sliderZoom5.Minimum;
            zoomMinValue5.Content = sliderZoom5.Minimum.ToString();
            zoomMaxValue5.Content = sliderZoom5.Maximum.ToString();
        }

        int hCamera;

        private void sliderZoom1_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            zoomTextBox1.Text = ((int)(sliderZoom1.Value)).ToString();
        }

        private void sliderZoom2_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            zoomTextBox2.Text = ((int)(sliderZoom2.Value)).ToString();

        }

        private void sliderZoom3_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            zoomTextBox3.Text = ((int)(sliderZoom3.Value)).ToString();

        }

        void NumericTextBoxInput(object sender, TextCompositionEventArgs e)
        {
            var regex = new Regex(@"^[0-9]*(?:\.[0-9]*)?$");
            if (regex.IsMatch(e.Text) && !(e.Text == "." && ((TextBox)sender).Text.Contains(e.Text)))
                e.Handled = false;

            else
                e.Handled = true;
        }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            ReturnCode rc = Api.Uninitialize(hCamera);
        }

        private void sliderZoom4_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            zoomTextBox4.Text = ((int)(sliderZoom4.Value)).ToString();

        }

        private void sliderZoom5_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            zoomTextBox5.Text = ((int)(sliderZoom5.Value)).ToString();

        }

        private void zoomTextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Return)
            {
                SetZoomFromTextBox();
            }
        }

        private void zoomTextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            SetZoomFromTextBox();
        }

        private void SetZoomFromTextBox()
        {
            try
            {
                if (float.Parse(zoomTextBox1.Text) > sliderZoom1.Maximum)
                    zoomTextBox1.Text = sliderZoom1.Maximum.ToString();

                if (float.Parse(zoomTextBox1.Text) < sliderZoom1.Minimum)
                    zoomTextBox1.Text = sliderZoom1.Minimum.ToString();

                sliderZoom1.Value = float.Parse(zoomTextBox1.Text);
            }
            catch
            {

            }

            try
            {
                if (float.Parse(zoomTextBox2.Text) > sliderZoom2.Maximum)
                    zoomTextBox2.Text = sliderZoom2.Maximum.ToString();

                if (float.Parse(zoomTextBox2.Text) < sliderZoom2.Minimum)
                    zoomTextBox2.Text = sliderZoom2.Minimum.ToString();

                sliderZoom2.Value = float.Parse(zoomTextBox2.Text);
            }
            catch
            {

            }

            try
            {
                if (float.Parse(zoomTextBox3.Text) > sliderZoom3.Maximum)
                    zoomTextBox3.Text = sliderZoom3.Maximum.ToString();

                if (float.Parse(zoomTextBox3.Text) < sliderZoom3.Minimum)
                    zoomTextBox3.Text = sliderZoom3.Minimum.ToString();

                sliderZoom3.Value = float.Parse(zoomTextBox3.Text);
            }
            catch
            {

            }

            try
            {
                if (float.Parse(zoomTextBox4.Text) > sliderZoom4.Maximum)
                    zoomTextBox4.Text = sliderZoom4.Maximum.ToString();

                if (float.Parse(zoomTextBox4.Text) < sliderZoom4.Minimum)
                    zoomTextBox4.Text = sliderZoom4.Minimum.ToString();

                sliderZoom4.Value = float.Parse(zoomTextBox4.Text);
            }
            catch
            {

            }

            try
            {
                if (float.Parse(zoomTextBox5.Text) > sliderZoom5.Maximum)
                    zoomTextBox5.Text = sliderZoom5.Maximum.ToString();

                if (float.Parse(zoomTextBox5.Text) < sliderZoom5.Minimum)
                    zoomTextBox5.Text = sliderZoom5.Minimum.ToString();

                sliderZoom5.Value = float.Parse(zoomTextBox5.Text);
            }
            catch
            {

            }
        }

        private void checkBox3_Click(object sender, RoutedEventArgs e)
        {
            if (checkBox3.IsChecked == true)
                zoomGroupBox3.IsEnabled = true;
            else
                zoomGroupBox3.IsEnabled = false;
        }

        private void checkBox4_Click(object sender, RoutedEventArgs e)
        {
            if (checkBox4.IsChecked == true)
                zoomGroupBox4.IsEnabled = true;
            else
                zoomGroupBox4.IsEnabled = false;
        }

        private void checkBox5_Click(object sender, RoutedEventArgs e)
        {
            if (checkBox5.IsChecked == true)
                zoomGroupBox5.IsEnabled = true;
            else
                zoomGroupBox5.IsEnabled = false;
        }

        IList<ZoomPosition> zoomPositions = new List<ZoomPosition>();

        private void buttonStart_Click(object sender, RoutedEventArgs e)
        {
            Controls.IsEnabled = false;
            buttonStart.IsEnabled = false;
            buttonStop.IsEnabled = true;
            runZoomThread = true;

            zoomPositions.Clear();

            zoomPositions.Add(new ZoomPosition(1, (float)sliderZoom1.Value));
            zoomPositions.Add(new ZoomPosition(2, (float)sliderZoom2.Value));

            if (checkBox3.IsChecked==true)
                zoomPositions.Add(new ZoomPosition(3, (float)sliderZoom3.Value));

            if (checkBox4.IsChecked == true)
                zoomPositions.Add(new ZoomPosition(4, (float)sliderZoom4.Value));

            if (checkBox5.IsChecked == true)
                zoomPositions.Add(new ZoomPosition(5, (float)sliderZoom5.Value));

            zoomThread = new Thread(ZoomThread);

            if (zoomThread.IsAlive == false)
            {
                zoomThread.IsBackground = true;
                zoomThread.Start();
            }
        }

        private void buttonStop_Click(object sender, RoutedEventArgs e)
        {
            Controls.IsEnabled = true;
            buttonStart.IsEnabled = true;
            buttonStop.IsEnabled = false;

            runZoomThread = false;
            ActiveMarker1.Visibility = Visibility.Hidden;
            ActiveMarker2.Visibility = Visibility.Hidden;
            ActiveMarker3.Visibility = Visibility.Hidden;
            ActiveMarker4.Visibility = Visibility.Hidden;
            ActiveMarker5.Visibility = Visibility.Hidden;
        }

        bool runZoomThread = false;
        Thread zoomThread;

        private void ZoomThread()
        {
            int i = 0;

            while (runZoomThread == true)
            {

                int sleepTime = 0;

                Application.Current.Dispatcher.Invoke(new System.Action(() =>
                {

                    if (zoomPositions[i].PositionNumber == 1)
                        ActiveMarker1.Visibility = Visibility.Visible;
                    else
                        ActiveMarker1.Visibility = Visibility.Hidden;

                    if (zoomPositions[i].PositionNumber == 2)
                        ActiveMarker2.Visibility = Visibility.Visible;
                    else
                        ActiveMarker2.Visibility = Visibility.Hidden;

                    if (zoomPositions[i].PositionNumber == 3)
                        ActiveMarker3.Visibility = Visibility.Visible;
                    else
                        ActiveMarker3.Visibility = Visibility.Hidden;

                    if (zoomPositions[i].PositionNumber == 4)
                        ActiveMarker4.Visibility = Visibility.Visible;
                    else
                        ActiveMarker4.Visibility = Visibility.Hidden;

                    if (zoomPositions[i].PositionNumber == 5)
                        ActiveMarker5.Visibility = Visibility.Visible;
                    else
                        ActiveMarker5.Visibility = Visibility.Hidden;


                    sleepTime = (int)(double.Parse(holdTimeTextBox.Text)*1000);

                }));

                float[] parms = { zoomPositions[i].ZoomValue };
                ReturnCode rc = Api.SetFeature(hCamera, Feature.Zoom, FeatureFlags.Manual | FeatureFlags.Presence, parms.Length, parms);

                Thread.Sleep(sleepTime);

                i++;

                i = i % zoomPositions.Count;

            }
        }




    }


    public class ZoomPosition
    {
        public int PositionNumber { get; set; }
        public float ZoomValue { get; set; }
        
        public ZoomPosition(int positionNumber,float zoomValue)
        {
            PositionNumber = positionNumber;
            ZoomValue = zoomValue;
        }

    }

}
