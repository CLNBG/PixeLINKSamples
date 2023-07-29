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

namespace Initialize
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            this.Title = "Initialize";
        }

        private void initialize_Click(object sender, RoutedEventArgs e)
        {
            // Declare the camera handle we'll use to interact with the camera
            int hCamera = 0;

            // By passing in a serial number of 0, we're saying to the API that 
            // we want ANY camera. If there are 2 or more cameras, we can't be 
            // certain a priori which camera we'll get, but we will get one of them.
            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (Api.IsSuccess(rc))
            {
                MessageBox.Show(this, "Successfully initialized camera. hCamera = " + hCamera);

                // Tell the API that we're done interacting with the camera.
                rc = Api.Uninitialize(hCamera);
                MessageBox.Show(this, "Uninitialize return code = " + rc);
            }
            else
            {
                string msg = String.Format("Error initializing camera\nReturn code: {0} (0x{1:X})", rc, rc);
                MessageBox.Show(this, msg);
            }
        }
    }
}
