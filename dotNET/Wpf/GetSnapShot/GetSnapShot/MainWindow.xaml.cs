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
using System.Windows.Forms;

using PixeLINK;

namespace GetSnapShot
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            directory.IsEnabled = false;
            directory.Text = System.Windows.Forms.Application.StartupPath;
        }
		
        private void getSnapShot_Click(object sender, RoutedEventArgs e)
        {
            int hCamera = 0;
            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (!Api.IsSuccess(rc))
            {
                System.Windows.MessageBox.Show(this, String.Format("Unable to initialize a camera\n({0})", rc));
                return;
            }
			
            GetSnapshot.SnapshotHelper h = new GetSnapshot.SnapshotHelper(hCamera);

            h.GetSnapshot(ImageFormat.Bmp, "snapshot.bmp", directory.Text);

            rc = Api.Uninitialize(hCamera);
        }

        private void saveTo_Click(object sender, RoutedEventArgs e)
        {

            FolderBrowserDialog folderDlg = new FolderBrowserDialog();
            folderDlg.ShowNewFolderButton = true;
            // Show the FolderBrowserDialog.
            DialogResult result = folderDlg.ShowDialog();
            if (result == System.Windows.Forms.DialogResult.OK)
            {
                directory.Text = folderDlg.SelectedPath;
                Environment.SpecialFolder root = folderDlg.RootFolder;
            }

        }
    }
}
