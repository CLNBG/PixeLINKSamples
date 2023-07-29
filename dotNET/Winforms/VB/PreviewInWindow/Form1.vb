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
    Friend WithEvents PictureBox1 As System.Windows.Forms.PictureBox
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.PictureBox1 = New System.Windows.Forms.PictureBox
        Me.SuspendLayout()
        '
        'PictureBox1
        '
        Me.PictureBox1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle
        Me.PictureBox1.Location = New System.Drawing.Point(16, 16)
        Me.PictureBox1.Name = "PictureBox1"
        Me.PictureBox1.Size = New System.Drawing.Size(88, 72)
        Me.PictureBox1.TabIndex = 0
        Me.PictureBox1.TabStop = False
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(292, 266)
        Me.Controls.Add(Me.PictureBox1)
        Me.Name = "Form1"
        Me.Text = "Preview"
        Me.ResumeLayout(False)

    End Sub

#End Region

    Dim m_hCamera As Integer

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        Cursor.Current = Cursors.WaitCursor

        Dim rc As ReturnCode
        Dim hCamera As Integer
        m_hCamera = 0

        Try
            rc = Api.Initialize(0, hCamera)
            If Not Api.IsSuccess(rc) Then
                MessageBox.Show("ERROR: Unable to initialize a camera (Error " + rc.ToString() + ")")
                Me.Close()
                Return
            End If

            m_hCamera = hCamera
            Call ResizePreviewPictureBox()
            Call SetPreviewSettings()
            hCamera = 0

        Finally
            If (hCamera <> 0) Then
                rc = Api.Uninitialize(hCamera)
                m_hCamera = 0
                Me.Close()
            End If
            Cursor.Current = Cursors.Default
        End Try

    End Sub

    Private Sub Form1_Closed(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Closed

        If (m_hCamera <> 0) Then
            Api.Uninitialize(m_hCamera)
            m_hCamera = 0
        End If

    End Sub

    Private m_previewPercentOfClient As Single = 0.75

    Private Sub ResizePreviewPictureBox()

        PictureBox1.Height = Me.ClientSize.Height * m_previewPercentOfClient
        PictureBox1.Width = Me.ClientSize.Width * m_previewPercentOfClient

        PictureBox1.Top = (Me.ClientSize.Height - PictureBox1.Height) / 2
        PictureBox1.Left = (Me.ClientSize.Width - PictureBox1.Width) / 2

    End Sub

    Private Sub SetPreviewSettings()

        If (0 = m_hCamera) Then
            Return
        End If

        Dim rc As ReturnCode
        '// Note: When previewing into a picture window, the title is irrelevant because it's not displayed to the user
        rc = Api.SetPreviewSettings(m_hCamera, "Preview", PreviewWindowStyles.Child Or PreviewWindowStyles.Visible, 0, 0, PictureBox1.Width, PictureBox1.Height, PictureBox1.Handle.ToInt32, 0)

        rc = Api.SetStreamState(m_hCamera, StreamState.Start)
        Dim hWnd As Integer = 0
        rc = Api.SetPreviewState(m_hCamera, PreviewState.Start, hWnd)

    End Sub

    Private Sub Form1_Resize(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Resize

        If (m_hCamera <> 0) Then
            Dim hWnd As Integer
            hWnd = 0
            Api.SetPreviewState(m_hCamera, PreviewState.Stop, hWnd)
        End If

        Call ResizePreviewPictureBox()
        Call SetPreviewSettings()

    End Sub
End Class
