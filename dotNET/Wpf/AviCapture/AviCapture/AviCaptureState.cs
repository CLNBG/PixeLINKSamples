using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using PixeLINK;



namespace AviCapture
{
    public class AviCaptureState
    {
        public AviCaptureState(int hCamera, int framesPerClip, int numberOfClips, string clipFileNameTemplate, Api.ClipTerminator pCallbackFunction)
        {
            m_keepRunning = true;
            m_numberOfClips = numberOfClips;
            m_framesPerClip = framesPerClip;
            m_hCamera = hCamera;
            m_pCallbackFunction = pCallbackFunction;
            m_clipFileNameTemplate = clipFileNameTemplate;
            m_currentCaptureIndex = 0;
        }

        public AviCaptureState() { }
        public int GetCameraHandle()   { return m_hCamera; }
        public int GetNumberOfClips()  { return m_numberOfClips; }
        public bool KeepRunning()  { return m_keepRunning; }
        public void StopRunning()  { m_keepRunning = false; }

        public bool StartCapturing()
        {
            return CaptureAClip();
        }

        public bool CaptureNextClip()
        {
            UpdateCaptureIndex();
            return CaptureAClip();
        }

        public bool CaptureAClip()
        {
            ClipEncodingInfo clipInfo = new ClipEncodingInfo { DecimationFactor = 1, PlaybackBitRate = 1000000, StreamEncoding = ClipEncoding.H264, PlaybackFrameRate = MainWindow.DetermineFramesPerSecond(m_hCamera) };
            ReturnCode rc = Api.GetEncodedClip(m_hCamera, m_framesPerClip, GetCurrentClipFileName(),ref clipInfo, m_pCallbackFunction);
            return Api.IsSuccess(rc);
        }

        public void UpdateCaptureIndex()
        {
            m_currentCaptureIndex++;
            if (m_currentCaptureIndex == m_numberOfClips)
            {
                m_currentCaptureIndex = 0;
            }
        }

        public int GetCurrentCaptureIndex()  { return m_currentCaptureIndex; }

        public string GetCurrentClipFileName() 
	    {
            char[] buffer = new char[260];
            string fileName;
            fileName = m_clipFileNameTemplate + m_currentCaptureIndex + ".h264";
            return fileName;
	    }

        private bool m_keepRunning;
        private Api.ClipTerminator m_pCallbackFunction;
        private int m_hCamera;
        private int m_framesPerClip;
        private int m_numberOfClips;
        private int m_currentCaptureIndex;
        private string m_clipFileNameTemplate;
    }
}
