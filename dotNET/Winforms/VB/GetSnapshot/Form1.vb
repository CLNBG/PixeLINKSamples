
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
    Friend WithEvents Button1 As System.Windows.Forms.Button
    <System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.Button1 = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'Button1
        '
        Me.Button1.Location = New System.Drawing.Point(78, 64)
        Me.Button1.Name = "Button1"
        Me.Button1.Size = New System.Drawing.Size(136, 32)
        Me.Button1.TabIndex = 0
        Me.Button1.Text = "Take Snapshot"
        '
        'Form1
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(5, 13)
        Me.ClientSize = New System.Drawing.Size(292, 174)
        Me.Controls.Add(Me.Button1)
        Me.Name = "Form1"
        Me.Text = "Get Snapshot Demo"
        Me.ResumeLayout(False)

    End Sub

#End Region

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click

        Cursor.Current = Cursors.WaitCursor

        Dim hCamera As Integer
        Dim rc As ReturnCode

        Try
            rc = Api.Initialize(0, hCamera)
            If Not Api.IsSuccess(rc) Then
                MessageBox.Show("ERROR: Unable to initialize a camera (Error " + rc.ToString() + ")")
                Return
            End If

            Dim helper As New SnapshotHelper(hCamera)

            helper.GetSnapshot(ImageFormat.Bmp, "test.bmp")

        Finally
            If (hCamera <> 0) Then
                rc = Api.Uninitialize(hCamera)
            End If
            Cursor.Current = Cursors.Default
        End Try

    End Sub
End Class
