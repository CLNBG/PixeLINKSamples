'//
'// SetIpAddress
'//
'// Demonstrates how to find a a GigE camera and assign an IP address.
'//
'// This example assumes:
'// - there is only one PixeLINK camera detectable, and it's a GigE camera;
'// - the GigE camera does not have a persistent static IP address;
'// - the GigE camera will not be given an IP address by a DHCP server;
'// - the GigE camera will therefore fail over to acquiring a link-local address (169.254.x.x);
'// - the GigE camera is connected to a network card, 192.168.1.1.
'// 
Module Module1

    Sub Main()

        Dim rc As PixeLINK.ReturnCode = SetCameraIpAddress()
        If (PixeLINK.Api.IsSuccess(rc)) Then
            MsgBox("Successfully set IP address!")
        End If

    End Sub

    Private Sub ReportError(ByVal msg As String)

        Dim errMsg As String = "Error: " + msg
        System.Console.WriteLine(errMsg)
        MsgBox(errMsg, MsgBoxStyle.OkOnly + MsgBoxStyle.Critical)


    End Sub

    Private Sub ReportError(ByVal msg As String, ByVal rc As PixeLINK.ReturnCode)

        Dim errMsg As String = String.Format("{0}\nReturnCode: {1}", msg, rc)
        ReportError(errMsg)

    End Sub

    Public Function SetCameraIpAddress() As PixeLINK.ReturnCode

        Dim cameraIdInfo As PixeLINK.CameraIdInformation = New PixeLINK.CameraIdInformation
        Dim rc As PixeLINK.ReturnCode
        rc = DetectIpCamera(cameraIdInfo)
        If (Not PixeLINK.Api.IsSuccess(rc)) Then
            Return rc
        End If

        Dim ipAddress As New PixeLINK.IPAddress(192, 168, 1, 2)
        Dim ipSubnetMask As New PixeLINK.IPAddress(255, 255, 255, 0)
        Dim ipDefaultGateway As New PixeLINK.IPAddress(0, 0, 0, 0)
        Dim persistent As Integer = 0


        System.Console.WriteLine(String.Format("Setting the IP address to {0}", ipAddress.ToString()))
        rc = PixeLINK.Api.SetCameraIPAddress(cameraIdInfo.MacAddress, ipAddress, ipSubnetMask, ipDefaultGateway, persistent)
        If (Not PixeLINK.Api.IsSuccess(rc)) Then
            ReportError("Unable to set the IP address.", rc)
            Return rc
        End If

        Return PixeLINK.ReturnCode.Success

    End Function

    Public Function IsIpCamera(ByRef cameraIdInfo As PixeLINK.CameraIdInformation) As Boolean

        Dim total As Integer = 0
        Dim i As Integer
        For i = 0 To 5
            total = total + cameraIdInfo.MacAddress(i)
        Next
        Return (total <> 0)

    End Function


    Public Function DetectIpCamera(ByRef cameraIdInfo As PixeLINK.CameraIdInformation) As PixeLINK.ReturnCode

        Dim rc As PixeLINK.ReturnCode
        Dim numberOfCameras As Integer = 0

        '// Try a few times because a camera recently plugged in may be still trying to acquire an 
        '// address via DHCP before failing over to LLA.
        System.Console.Write("Detecting IP camera..")
        Dim i As Integer
        For i = 1 To 30

            System.Console.Write(".")
            rc = PixeLINK.Api.GetNumberCameras2(Nothing, numberOfCameras)
            If (Not PixeLINK.Api.IsSuccess(rc)) Then
                ReportError("An error occurred while detecting cameras", rc)
                Return rc
            End If

            If (numberOfCameras >= 1) Then
                Exit For
            End If

            System.Threading.Thread.Sleep(1000)

        Next i

        System.Console.WriteLine()
        If (numberOfCameras < 1) Then
            ReportError("No cameras detected.")
            Return PixeLINK.ReturnCode.NoCameraAvailableError
        ElseIf (numberOfCameras > 1) Then
            ReportError("Too many cameras detected.")
            Return PixeLINK.ReturnCode.UnknownError
        End If

        Dim cameraIdInfoArray(0) As PixeLINK.CameraIdInformation
        rc = PixeLINK.Api.GetNumberCameras2(cameraIdInfoArray, numberOfCameras)
        If (Not PixeLINK.Api.IsSuccess(rc)) Then
            ReportError("Unable to read camera id information", rc)
            Return rc
        End If

        System.Diagnostics.Debug.Assert(numberOfCameras = 1)
        System.Console.WriteLine("Camera detected: {0}", cameraIdInfoArray(0).MacAddress.ToString())


        If (Not IsIpCamera(cameraIdInfoArray(0))) Then
            ReportError("The camera is not a GigE camera.")
            Return PixeLINK.ReturnCode.NoCameraAvailableError
        End If

        cameraIdInfo = cameraIdInfoArray(0)

        Return rc

    End Function

End Module
