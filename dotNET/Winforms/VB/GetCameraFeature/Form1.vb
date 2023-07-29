
Imports PixeLINK


Public Class Form1
    Inherits System.Windows.Forms.Form

#Region " Windows Form Designer generated code "

    Public Sub New()
        MyBase.New()

        'This call is required by the Windows Form Designer.
        InitializeComponent()

        'Add any initialization after the InitializeComponent() call

    End Sub

    'Form overrides dispose to clean up the component list.
    Protected Overloads Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing Then
            If Not (components Is Nothing) Then
                components.Dispose()
            End If
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    Friend WithEvents lstRoiInfo As System.Windows.Forms.ListBox
    Friend WithEvents label5 As System.Windows.Forms.Label
    Friend WithEvents txtMaxGain As System.Windows.Forms.TextBox
    Friend WithEvents label4 As System.Windows.Forms.Label
    Friend WithEvents txtMinGain As System.Windows.Forms.TextBox
    Friend WithEvents lblMinGain As System.Windows.Forms.Label
    Friend WithEvents txtRoi As System.Windows.Forms.TextBox
    Friend WithEvents label3 As System.Windows.Forms.Label
    Friend WithEvents txtExposure As System.Windows.Forms.TextBox
    Friend WithEvents label2 As System.Windows.Forms.Label
    Friend WithEvents txtGain As System.Windows.Forms.TextBox
    Friend WithEvents label1 As System.Windows.Forms.Label
    Friend WithEvents btnGetFeatures As System.Windows.Forms.Button
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.lstRoiInfo = New System.Windows.Forms.ListBox
        Me.label5 = New System.Windows.Forms.Label
        Me.txtMaxGain = New System.Windows.Forms.TextBox
        Me.label4 = New System.Windows.Forms.Label
        Me.txtMinGain = New System.Windows.Forms.TextBox
        Me.lblMinGain = New System.Windows.Forms.Label
        Me.txtRoi = New System.Windows.Forms.TextBox
        Me.label3 = New System.Windows.Forms.Label
        Me.txtExposure = New System.Windows.Forms.TextBox
        Me.label2 = New System.Windows.Forms.Label
        Me.txtGain = New System.Windows.Forms.TextBox
        Me.label1 = New System.Windows.Forms.Label
        Me.btnGetFeatures = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'lstRoiInfo
        '
        Me.lstRoiInfo.Location = New System.Drawing.Point(16, 296)
        Me.lstRoiInfo.Name = "lstRoiInfo"
        Me.lstRoiInfo.Size = New System.Drawing.Size(224, 69)
        Me.lstRoiInfo.TabIndex = 25
        '
        'label5
        '
        Me.label5.Location = New System.Drawing.Point(72, 272)
        Me.label5.Name = "label5"
        Me.label5.Size = New System.Drawing.Size(120, 16)
        Me.label5.TabIndex = 24
        Me.label5.Text = "ROI Parameter Info"
        '
        'txtMaxGain
        '
        Me.txtMaxGain.Location = New System.Drawing.Point(112, 184)
        Me.txtMaxGain.Name = "txtMaxGain"
        Me.txtMaxGain.Size = New System.Drawing.Size(120, 20)
        Me.txtMaxGain.TabIndex = 23
        Me.txtMaxGain.Text = ""
        '
        'label4
        '
        Me.label4.Location = New System.Drawing.Point(16, 184)
        Me.label4.Name = "label4"
        Me.label4.Size = New System.Drawing.Size(80, 16)
        Me.label4.TabIndex = 22
        Me.label4.Text = "Max Gain (dB)"
        '
        'txtMinGain
        '
        Me.txtMinGain.Location = New System.Drawing.Point(112, 144)
        Me.txtMinGain.Name = "txtMinGain"
        Me.txtMinGain.Size = New System.Drawing.Size(120, 20)
        Me.txtMinGain.TabIndex = 21
        Me.txtMinGain.Text = ""
        '
        'lblMinGain
        '
        Me.lblMinGain.Location = New System.Drawing.Point(16, 144)
        Me.lblMinGain.Name = "lblMinGain"
        Me.lblMinGain.Size = New System.Drawing.Size(80, 16)
        Me.lblMinGain.TabIndex = 20
        Me.lblMinGain.Text = "Min Gain (dB)"
        '
        'txtRoi
        '
        Me.txtRoi.Location = New System.Drawing.Point(112, 224)
        Me.txtRoi.Name = "txtRoi"
        Me.txtRoi.Size = New System.Drawing.Size(120, 20)
        Me.txtRoi.TabIndex = 19
        Me.txtRoi.Text = ""
        '
        'label3
        '
        Me.label3.Location = New System.Drawing.Point(16, 224)
        Me.label3.Name = "label3"
        Me.label3.Size = New System.Drawing.Size(80, 16)
        Me.label3.TabIndex = 18
        Me.label3.Text = "ROI"
        '
        'txtExposure
        '
        Me.txtExposure.Location = New System.Drawing.Point(112, 72)
        Me.txtExposure.Name = "txtExposure"
        Me.txtExposure.Size = New System.Drawing.Size(120, 20)
        Me.txtExposure.TabIndex = 17
        Me.txtExposure.Text = ""
        '
        'label2
        '
        Me.label2.Location = New System.Drawing.Point(16, 72)
        Me.label2.Name = "label2"
        Me.label2.Size = New System.Drawing.Size(80, 16)
        Me.label2.TabIndex = 16
        Me.label2.Text = "Exposure (ms)"
        '
        'txtGain
        '
        Me.txtGain.Location = New System.Drawing.Point(112, 104)
        Me.txtGain.Name = "txtGain"
        Me.txtGain.Size = New System.Drawing.Size(120, 20)
        Me.txtGain.TabIndex = 15
        Me.txtGain.Text = ""
        '
        'label1
        '
        Me.label1.Location = New System.Drawing.Point(16, 104)
        Me.label1.Name = "label1"
        Me.label1.Size = New System.Drawing.Size(80, 16)
        Me.label1.TabIndex = 14
        Me.label1.Text = "Gain (dB)"
        '
        'btnGetFeatures
        '
        Me.btnGetFeatures.Location = New System.Drawing.Point(72, 16)
        Me.btnGetFeatures.Name = "btnGetFeatures"
        Me.btnGetFeatures.Size = New System.Drawing.Size(112, 32)
        Me.btnGetFeatures.TabIndex = 13
        Me.btnGetFeatures.Text = "Get Features"
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(256, 382)
        Me.Controls.Add(Me.lstRoiInfo)
        Me.Controls.Add(Me.label5)
        Me.Controls.Add(Me.txtMaxGain)
        Me.Controls.Add(Me.label4)
        Me.Controls.Add(Me.txtMinGain)
        Me.Controls.Add(Me.lblMinGain)
        Me.Controls.Add(Me.txtRoi)
        Me.Controls.Add(Me.label3)
        Me.Controls.Add(Me.txtExposure)
        Me.Controls.Add(Me.label2)
        Me.Controls.Add(Me.txtGain)
        Me.Controls.Add(Me.label1)
        Me.Controls.Add(Me.btnGetFeatures)
        Me.Name = "Form1"
        Me.Text = "Form1"
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private Sub btnGetFeatures_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnGetFeatures.Click
        Dim hCamera As Integer = 0
        Dim rc As ReturnCode = Api.Initialize(0, hCamera)
        If Not Api.IsSuccess(rc) Then
            MessageBox.Show("Unable to initialize a camera")
            Return
        End If

        txtExposure.Text = System.Convert.ToString(GetSimpleFeature(hCamera, Feature.Shutter) * 1000)
        txtGain.Text = System.Convert.ToString(GetSimpleFeature(hCamera, Feature.Gain))

        Call GetGainLimits(hCamera)

        Dim top As Integer = 0
        Dim left As Integer = 0
        Dim width As Integer = 0
        Dim height As Integer = 0
        Call GetRoi(hCamera, top, left, width, height)
        txtRoi.Text = "(" + left.ToString() + "," + top.ToString() + ") -> (" + (left + width).ToString() + "," + (top + height).ToString() + ")"

        Call GetRoiInfo(hCamera)

        Api.Uninitialize(hCamera)

    End Sub

    '// A 'simple' feature is any feature that has only one parameter
    Private Function GetSimpleFeature(ByVal hCamera As Integer, ByVal featureId As Feature) As Single
        Dim flags As FeatureFlags = 0
        Dim numParams As Integer = 1
        Dim params(numParams - 1) As Single
        Dim rc As ReturnCode = Api.GetFeature(hCamera, featureId, flags, numParams, params)
        Return params(0)

    End Function

    Private Sub GetGainLimits(ByVal hCamera As Integer)
        Dim featureInfo As CameraFeature = New CameraFeature
        If (Not Api.IsSuccess(Api.GetCameraFeatures(hCamera, Feature.Gain, featureInfo))) Then
            lstRoiInfo.Items.Add("Unable to read Gain information.")
        Else
            txtMinGain.Text = System.Convert.ToString(featureInfo.parameters(0).MinimumValue)
            txtMaxGain.Text = System.Convert.ToString(featureInfo.parameters(0).MaximumValue)
        End If

    End Sub

    Private Sub GetRoiInfo(ByVal hCamera As Integer)
        lstRoiInfo.Items.Clear()

        Dim featureInfo As CameraFeature = New CameraFeature
        If (Not Api.IsSuccess(Api.GetCameraFeatures(hCamera, Feature.Roi, featureInfo))) Then
            lstRoiInfo.Items.Add("Unable to read Gain information.")
        Else
            Debug.Assert(featureInfo.numberOfParameters = featureInfo.parameters.Length)
            lstRoiInfo.Items.Add("Num params: " + featureInfo.numberOfParameters.ToString())
            Dim i As Integer
            For i = 0 To featureInfo.parameters.Length - 1
                Dim p As PixeLINK.FeatureParameter = featureInfo.parameters(i)
                lstRoiInfo.Items.Add(String.Format("Param {0}: min={1}, max={2}", i, p.MinimumValue, p.MaximumValue))
            Next
        End If
    End Sub

    Private Sub GetRoi(ByVal hCamera As Integer, ByRef top As Integer, ByRef left As Integer, ByRef width As Integer, ByRef height As Integer)
        Dim flags As FeatureFlags = 0
        Dim numParams As Integer = 4
        Dim params(numParams - 1) As Single
        Dim rc As ReturnCode = Api.GetFeature(hCamera, Feature.Roi, flags, numParams, params)
        top = System.Convert.ToInt32(params(FeatureParameterIndex.RoiTop))
        left = System.Convert.ToInt32(params(FeatureParameterIndex.RoiLeft))
        width = System.Convert.ToInt32(params(FeatureParameterIndex.RoiWidth))
        height = System.Convert.ToInt32(params(FeatureParameterIndex.RoiHeight))
    End Sub
End Class
