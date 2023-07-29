using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.IO;

namespace AviCapture
{
    public class AviConvertState
    {
        public AviConvertState()
        {
            m_keepRunning = true;
            m_currentFileNameIndex = 0;
            DirectoryInfo di = new DirectoryInfo(Directory.GetCurrentDirectory());
            m_h264Files = di.GetFiles("*.h264").Where(p => p.Extension == ".h264").ToList();
        }


        public int GetNumberOfFiles() 
	    {
		    return m_h264Files.Count;
	    }

        public FileInfo GetCurrentFileName()
	    {
		    return m_h264Files[m_currentFileNameIndex];
	    }

        public int GetCurrentFileIndex()  { return m_currentFileNameIndex; }

        public void SetCurrentIndex(int newIndex)
        {
            if (newIndex < m_h264Files.Count)
            {
                m_currentFileNameIndex = newIndex;
            }
        }

        public void StopConverting()
        {
            m_keepRunning = false;
        }

        public bool KeepRunning() { return m_keepRunning; }

        public Thread GetThread() { return m_pThread; }

        public void SetThread(Thread thread)
        {
            if (null != thread)
            {
                m_pThread = thread;
                thread.Start();
            }
        }

        protected volatile bool m_keepRunning;
        protected IList<FileInfo> m_h264Files;
        int m_currentFileNameIndex;
        Thread m_pThread;
    }
}
