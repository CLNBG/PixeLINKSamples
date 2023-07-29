'//
'// Demonstrates how to use the callback function in the call to Api.GetClip to detect when the entire clip has been captured.
'//
Imports PixeLINK

Module Module1

    Sub Main()

        Dim hCamera As Integer
        Dim rc As ReturnCode

        rc = Api.Initialize(0, hCamera)
        If Not Api.IsSuccess(rc) Then
            System.Console.WriteLine("ERROR: Unable to initialize a camera (Error " + rc.ToString() + ")")
            Return
        End If

        rc = Api.SetStreamState(hCamera, StreamState.Start)
        If Not Api.IsSuccess(rc) Then
            System.Console.WriteLine("ERROR: Unable to start streaming (Error " + rc.ToString() + ")")
            Return
        End If

        rc = GetClip(hCamera, 10, "foo.pds")

        Api.SetStreamState(hCamera, StreamState.Stop)
        Api.Uninitialize(hCamera)

        Return

    End Sub

    Dim m_syncLock As Object = New Object   '// Sync object protecting the following member data
    Dim m_isGetClipFinished As Boolean
    Dim m_getClipResult As ReturnCode

    '//
    '// Callback function for Api.GetClip
    '// Called by a thread other than that which called Api.GetClip, therefore we syncronize access to the member data modified
    '//
    Private Function GetClipCallback(ByVal hCamera As Integer, ByVal numberOfFramesCaptured As Integer, ByVal returnCode As ReturnCode) As Integer

        SyncLock m_syncLock
            m_getClipResult = returnCode
            m_isGetClipFinished = True
        End SyncLock

    End Function


    Private Function GetClip(ByVal hCamera As Integer, ByVal numFrames As Integer, ByVal fileName As String) As ReturnCode

        SyncLock m_syncLock
            m_isGetClipFinished = False
            m_getClipResult = ReturnCode.UnknownError
        End SyncLock

        Dim callbackDelegate As Api.ClipTerminator
        callbackDelegate = New Api.ClipTerminator(AddressOf GetClipCallback)

        Dim rc As ReturnCode = Api.GetClip(hCamera, numFrames, fileName, callbackDelegate)
        If (Not Api.IsSuccess(rc)) Then
            Return rc
        End If

        '// Poll here until we can see that the callback function has been called
        While (True)
            SyncLock m_syncLock
                If (m_isGetClipFinished) Then
                    Exit While
                End If
            End SyncLock
            System.Threading.Thread.Sleep(500)
        End While

        Return m_getClipResult

    End Function

End Module
