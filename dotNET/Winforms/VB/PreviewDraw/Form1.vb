
Imports PixeLINK

Public Class Form1

    Inherits System.Windows.Forms.Form

    Private s_hCamera As Integer
    Private s_callbackDelegate As Api.Callback

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
    Friend WithEvents btnStartPreview As System.Windows.Forms.Button
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.btnStartPreview = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'btnStartPreview
        '
        Me.btnStartPreview.Location = New System.Drawing.Point(62, 29)
        Me.btnStartPreview.Name = "btnStartPreview"
        Me.btnStartPreview.Size = New System.Drawing.Size(160, 56)
        Me.btnStartPreview.TabIndex = 0
        Me.btnStartPreview.Text = "StartPreview"
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(284, 114)
        Me.Controls.Add(Me.btnStartPreview)
        Me.Name = "Form1"
        Me.Text = "Form1"
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private Function IsMonoCamera(ByRef frameDesc As FrameDescriptor) As Boolean
        Select Case frameDesc.PixelFormat
            Case PixelFormat.Mono8, PixelFormat.Mono16
                Return True
            Case Else
                Return False
        End Select

    End Function

    '//
    '// If you want to do a raster operation such as invert a pixel, you 
    '// can use System.Runtime.InteropServices.Marshal.ReadByte
    '//
    '// A note on the way to determine the offset to a pixels:
    '// With mono, it's a simple arrangement. See the calculation below.
    '// With color, remember that the layout of the pixels in a preview callback image is the same as a BMP.
    '// i.e. each pixel is a 24-bit triplet of three values, B, G, and R, in that value, and,
    '// the image is row inverted. i.e. the last row appears in memory first, then the second-last row, etc.
    '// http://en.wikipedia.org/wiki/BMP_file_format
    '//
    Private Function PreviewOverlayCallback(ByVal cameraHandle As Integer, ByVal pImageData As System.IntPtr, ByVal dataFormat As PixeLINK.PixelFormat, ByRef frameDescriptor As PixeLINK.FrameDescriptor, ByVal context As Integer) As Integer

        Dim pixelAddressingValue As Integer
        pixelAddressingValue = frameDescriptor.PixelAddressingValue

        Dim numRows As Integer
        Dim numCols As Integer
        numRows = frameDescriptor.RoiHeight / pixelAddressingValue
        numCols = frameDescriptor.RoiWidth / pixelAddressingValue

        If IsMonoCamera(frameDescriptor) Then

            Dim crossHairColor As Byte = 255
            Dim r As Integer
            Dim c As Integer
            Dim pixelOffset As Integer
            Dim bytesPerPixel As Integer = 1
            Dim bytesPerRow As Integer = numCols * bytesPerPixel

            '// Draw the vertical line
            c = numCols / 2
            For r = 0 To (numRows - 1)
                pixelOffset = (r * bytesPerRow) + (c * bytesPerPixel)
                System.Runtime.InteropServices.Marshal.WriteByte(pImageData, pixelOffset, crossHairColor)
            Next

            '// Draw the horizontal line
            r = numRows / 2
            For c = 0 To (numCols - 1)
                pixelOffset = (r * bytesPerRow) + (c * bytesPerPixel)
                System.Runtime.InteropServices.Marshal.WriteByte(pImageData, pixelOffset, crossHairColor)
            Next

        Else

            Dim crossHairRed As Byte = 255
            Dim crossHairGreen As Byte = 255
            Dim crossHairBlue As Byte = 255
            Dim r As Integer
            Dim c As Integer
            Dim pixelOffset As Integer
            Dim bytesPerPixel As Integer = 3
            Dim bytesPerRow As Integer = numCols * bytesPerPixel

            '// Draw the vertical line
            c = numCols / 2
            For r = 0 To (numRows - 1)
                pixelOffset = ((numRows - r - 1) * bytesPerRow) + (c * bytesPerPixel)
                System.Runtime.InteropServices.Marshal.WriteByte(pImageData, pixelOffset + 0, crossHairBlue)
                System.Runtime.InteropServices.Marshal.WriteByte(pImageData, pixelOffset + 1, crossHairGreen)
                System.Runtime.InteropServices.Marshal.WriteByte(pImageData, pixelOffset + 2, crossHairRed)
            Next

            '// Draw the horizontal line
            r = numRows / 2
            For c = 0 To (numCols - 1)
                pixelOffset = ((numRows - r - 1) * bytesPerRow) + (c * bytesPerPixel)
                System.Runtime.InteropServices.Marshal.WriteByte(pImageData, pixelOffset + 0, crossHairBlue)
                System.Runtime.InteropServices.Marshal.WriteByte(pImageData, pixelOffset + 1, crossHairGreen)
                System.Runtime.InteropServices.Marshal.WriteByte(pImageData, pixelOffset + 2, crossHairRed)
            Next
        End If



    End Function



    Private Sub btnStartPreview_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnStartPreview.Click

        Dim rc As ReturnCode
        Dim hCamera As Integer
        rc = Api.Initialize(0, hCamera)
        If (Not Api.IsSuccess(rc)) Then
            MessageBox.Show("Unable to initialize a camera", "Show Preview", MessageBoxButtons.OK, MessageBoxIcon.Error)
            Return
        End If
        s_hCamera = hCamera

        '// This is declared as a form private to ensure it doesn't get garbage collected.
        s_callbackDelegate = New Api.Callback(AddressOf PreviewOverlayCallback)
        Api.SetCallback2(hCamera, Callbacks.Preview, 0, s_callbackDelegate)

        rc = Api.SetStreamState(hCamera, StreamState.Start)
        Dim hWnd As Integer
        rc = Api.SetPreviewState(hCamera, PreviewState.Start, hWnd)

        btnStartPreview.Enabled = False

    End Sub

    Protected Overrides Sub OnClosed(ByVal e As System.EventArgs)
        If s_hCamera <> 0 Then
            Dim hWnd As Integer
            Api.SetPreviewState(s_hCamera, PreviewState.Stop, hWnd)
            Api.SetStreamState(s_hCamera, StreamState.Stop)
            Api.Uninitialize(s_hCamera)
        End If

    End Sub
End Class
