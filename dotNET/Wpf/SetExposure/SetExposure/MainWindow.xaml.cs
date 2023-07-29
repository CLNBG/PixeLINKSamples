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

namespace SetExposure
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

        int hCamera = 0;

        private void button_Click(object sender, RoutedEventArgs e)
        {

            ReturnCode rc = Api.Initialize(0, ref hCamera);

            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show(rc.ToString());
                return;
            }

            FeatureFlags flag = FeatureFlags.Manual;

            float[] newExposure = new float[1];

            try
            {
                newExposure[0] = ((float)Convert.ToDouble(textBox.Text)) / 1000;

                rc = Api.SetFeature(hCamera, Feature.Shutter, flag, 1, newExposure);

                MessageBox.Show(rc.ToString());
            }
            catch
            {
                Api.Uninitialize(hCamera);
                return;
            }

            Api.Uninitialize(hCamera);

        }
    }
}
