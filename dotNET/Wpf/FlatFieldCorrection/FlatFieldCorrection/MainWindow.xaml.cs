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

namespace FlatFieldCorrection
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            hCamera = 0;
            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (!Api.IsSuccess(rc))
            {
                Console.WriteLine("Failed to initialize a camera (" + rc.ToString() + ")");
                return;
            }
            EnableFfcButton.IsEnabled = true;
            DisableFfcButton.IsEnabled = false;
        }

        private int hCamera;
        private ReturnCode rc;

        private void EnableFfcButton_Click(object sender, RoutedEventArgs e)
        {
            rc = FlatFieldCorrection.FFCControl.Enable(hCamera, true);
            if (Api.IsSuccess(rc))
            {
                MessageBox.Show("FFC successfully enabled", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
                EnableFfcButton.IsEnabled = false;
                DisableFfcButton.IsEnabled = true;
            }
            else
            {
                MessageBox.Show("Failed to enabled FFC (" + rc.ToString() + ")", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }

        private void DisableFfcButton_Click(object sender, RoutedEventArgs e)
        {
            // Disable FFC
            rc = FlatFieldCorrection.FFCControl.Enable(hCamera, false);
            if (Api.IsSuccess(rc))
            {
                MessageBox.Show("FFC successfully disabled", "Success", MessageBoxButton.OK, MessageBoxImage.Information);
                EnableFfcButton.IsEnabled = true;
                DisableFfcButton.IsEnabled = false;
            }
            else
            {
                MessageBox.Show("Failed to disable FFC (" + rc.ToString() + ")", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }
}
