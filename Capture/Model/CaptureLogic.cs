using Kaltura.CaptureEngine;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Kaltura.CaptureEngine
{
    enum CaptureLogicState
    {
        Uninitialized,
        Paused,
        Capturing,
    }
    class CaptureLogic
    {
        DateTime m_recordingStartTime;
        CaptureEngine m_cameraEngine, m_screenEngine;
        TimeSpan m_totalRecordingTime = TimeSpan.Zero;

        public bool IsRecording { get; private set; }

        public string CameraCaptureFileName { get; set; }
        public string ScreenCaptureFileName { get; set; }

        public Device Camera { get; set; }
        public Device Screen { get; set; }
        public Device Mic { get; set; }

        private void ForEachDevice(Action<Device> action)
        {
            if (Camera != null)
                action(Camera);
            if (Screen != null)
                action(Screen);
            if (Mic != null)
                action(Mic);
        }
        public CaptureLogic()
        {
        }

        public TimeSpan RecordingTime
        {
            get
            {
                if (IsRecording)
                    return m_totalRecordingTime + (DateTime.UtcNow - m_recordingStartTime);
                else
                    return m_totalRecordingTime;
            }
        }
        public CaptureLogicState State
        {
            get
            {
                if (IsRecording)
                {
                    return CaptureLogicState.Capturing;
                }
                else
                {
                    if (m_screenEngine != null || m_cameraEngine != null)
                    {
                        return CaptureLogicState.Paused; 
                    }
                    return CaptureLogicState.Uninitialized;
                }
            }
        }
        
        public void Start()
        {
            if (Camera != null)
            {
                m_cameraEngine = new CaptureEngine();
                m_cameraEngine.SetOutputFile(CameraCaptureFileName);
                m_cameraEngine.AddVideoSource(Camera);
                if (Mic != null)
                    m_cameraEngine.AddAudioSource(Mic);

            }
            if (Screen != null)
            {
                m_screenEngine = new CaptureEngine();
                m_screenEngine.SetOutputFile(ScreenCaptureFileName);
                m_screenEngine.AddVideoSource(Screen);
                if (Mic != null && m_cameraEngine == null)
                    m_screenEngine.AddAudioSource(Mic);
            }

            if (m_cameraEngine != null)
                m_cameraEngine.Init();

            if (m_screenEngine != null)
                m_screenEngine.Init();

            ForEachDevice(d => d.Start());
            Resume();
        }

        public void Pause()
        {
            m_totalRecordingTime += RecordingTime;
            IsRecording = false;
        }

        public void Resume()
        {
            m_recordingStartTime = DateTime.UtcNow;
            IsRecording = true;
        }

        public void Stop()
        {
            Pause();

            if (m_screenEngine != null)
            {
                m_screenEngine.Dispose();
                m_screenEngine = null;
            }
            if (m_cameraEngine != null)
            {
                m_cameraEngine.Dispose();
                m_cameraEngine = null;
            }

            ForEachDevice(d => d.Stop());

        }

        internal void Dispose()
        {
        }
    }
}
