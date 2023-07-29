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
using System.Runtime.InteropServices;
using PixeLINK;

namespace GetCameraInfo
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            this.Title = "Demo App - Get Camera Info";
            this.ResizeMode = ResizeMode.NoResize;

            cameraName.Text = string.Empty;
            description.Text = string.Empty;
            firmwareVersion.Text = string.Empty;
            fpgaVersion.Text = string.Empty;
            modelName.Text = string.Empty;
            serialNumber.Text = string.Empty;
            vendorName.Text = string.Empty;
            xmlVersion.Text = string.Empty;

            cameraName.IsEnabled = false;
            description.IsEnabled = false;
            firmwareVersion.IsEnabled = false;
            fpgaVersion.IsEnabled = false;
            modelName.IsEnabled = false;
            serialNumber.IsEnabled = false;
            vendorName.IsEnabled = false;
            xmlVersion.IsEnabled = false;

        }

        private void button_Click(object sender, RoutedEventArgs e)
        {

            // Ask the API to initialize a camera for us
            int hCamera = 0;
            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show("Error " + rc + " when initializing the camera");
                return;
            }

            // 
            // Ask the API for camera info, and then display it to to the window
            // 
            CameraInformation info = new CameraInformation();

            int size = Marshal.SizeOf(info);

            rc = Api.GetCameraInformation(hCamera, ref info, size);
            if (Api.IsSuccess(rc))
            {
                cameraName.Text = info.CameraName;
                description.Text = info.Description;
                firmwareVersion.Text = info.FirmwareVersion;
                fpgaVersion.Text = info.FpgaVersion;
                modelName.Text = info.ModelName;
                serialNumber.Text = info.SerialNumber;
                vendorName.Text = info.VendorName;
                xmlVersion.Text = info.XmlVersion;
            }
            else
            {
                MessageBox.Show("Error " + rc + " when getting the camera info");
            }

            Api.Uninitialize(hCamera);


        }
    }
}
