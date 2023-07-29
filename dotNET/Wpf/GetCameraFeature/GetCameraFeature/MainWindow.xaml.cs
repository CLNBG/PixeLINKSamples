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



using System.Diagnostics;
using PixeLINK;

namespace GetCameraFeature
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            this.Title = "Demo App - Get Camera Features";
            this.ResizeMode = ResizeMode.NoResize;

            exposure.Text = string.Empty;
            gain.Text = string.Empty;
            minGain.Text = string.Empty;
            maxGain.Text = string.Empty;
            roi.Text = string.Empty;
            roiParameterInfo.Items.Clear();

            exposure.IsEnabled = false;
            gain.IsEnabled = false;
            minGain.IsEnabled = false;
            maxGain.IsEnabled = false;
            roi.IsEnabled = false;
            roiParameterInfo.IsEnabled = false;


        }

        private void getCameraFeaturesButton_Click(object sender, RoutedEventArgs e)
        {
            int hCamera = 0;
            ReturnCode rc = Api.Initialize(0, ref hCamera);
            if (!Api.IsSuccess(rc))
            {
                MessageBox.Show("Unable to initialize a camera");
                return;
            }

            exposure.Text = System.Convert.ToString(GetSimpleFeature(hCamera, Feature.Shutter) * 1000); // convert from s to ms
            gain.Text = System.Convert.ToString(GetSimpleFeature(hCamera, Feature.Gain));

            GetGainLimits(hCamera);

            int top = 0;
            int left = 0;
            int width = 0;
            int height = 0;
            GetRoi(hCamera, ref top, ref left, ref width, ref height);
            roi.Text = "(" + left + "," + top + ") -> (" + (left + width) + "," + (top + height) + ")";

            GetRoiInfo(hCamera);

            Api.Uninitialize(hCamera);
        }

        // A 'simple' feature is any feature that has only one parameter
        private float GetSimpleFeature(int hCamera, Feature featureId)
        {
            FeatureFlags flags = 0;
            int numParms = 1;
            float[] parms = new float[numParms];
            ReturnCode rc = Api.GetFeature(hCamera, featureId, ref flags, ref numParms, parms);
            return parms[0];
        }

        private void GetGainLimits(int hCamera)
        {
            CameraFeature featureInfo = new CameraFeature();
            if (!Api.IsSuccess(Api.GetCameraFeatures(hCamera, Feature.Gain, ref featureInfo)))
            {
                roiParameterInfo.Items.Add("Unable to read Gain info");
            }
            else
            {
                minGain.Text = System.Convert.ToString(featureInfo.parameters[0].MinimumValue);
                maxGain.Text = System.Convert.ToString(featureInfo.parameters[0].MaximumValue);
            }

        }

        private void GetRoiInfo(int hCamera)
        {
            roiParameterInfo.Items.Clear();

            CameraFeature featureInfo = new CameraFeature();
            if (!Api.IsSuccess(Api.GetCameraFeatures(hCamera, Feature.Roi, ref featureInfo)))
            {
                roiParameterInfo.Items.Add("Unable to read ROI info");
            }
            else
            {
                roiParameterInfo.Items.Add("Num params: " + featureInfo.numberOfParameters);
                Debug.Assert(featureInfo.numberOfParameters == featureInfo.parameters.Length);
                for (int i = 0; i < featureInfo.parameters.Length; i++)
                {
                    roiParameterInfo.Items.Add(String.Format("Param {0}: min={1}, max={2}", i, featureInfo.parameters[i].MinimumValue, featureInfo.parameters[i].MaximumValue));
                }
            }

        }

        // We assume a priori that we know that the ROI feature has 4 parameters.
        private void GetRoi(int hCamera, ref int top, ref int left, ref int width, ref int height)
        {
            FeatureFlags flags = 0;
            int numParms = 4;
            float[] parms = new float[numParms];
            ReturnCode rc = Api.GetFeature(hCamera, Feature.Roi, ref flags, ref numParms, parms);
            top = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.RoiTop]);
            left = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.RoiLeft]);
            width = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.RoiWidth]);
            height = System.Convert.ToInt32(parms[(int)FeatureParameterIndex.RoiHeight]);

        }

    }
}
