using Capture;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Kaltura.CaptureEngine
{
    internal  enum CaptureDeviceType
    {
	    None,
	    Video,
        Screen,
	    Audio
    };

    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    internal struct CaptureDeviceInfo
    {
        public CaptureDeviceType DeviceType;
        [MarshalAs(UnmanagedType.ByValTStr,SizeConst = 100)]
        public string FriendlyName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
	    public string FFMpegInput;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
	    public string FFMpegDevice;
    };

    internal class Device : IDisposable 
    {
        public IntPtr Handle {get; set;}

        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateDevice(ref CaptureDeviceInfo devicee);
        
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreateDesktopCapture(IntPtr handle);

        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void DestroyDevice(IntPtr device);

        
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr StartDevice( IntPtr handle);
        
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr StopDevice( IntPtr handle);




        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr ShowPreviewWindow( IntPtr handle);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void HidePreviewWindow(IntPtr handle);

        public static Device FromCaptureDevice(CaptureDeviceInfo deviceInfo)
        {
            var dev=new Device();
            dev.Handle = CreateDevice(ref deviceInfo);
            return dev;
        }
        public static Device FromDesktop(IntPtr handle)
        {
            var dev = new Device();
            dev.Handle = CreateDesktopCapture(handle);
            return dev;
        }


        ~Device()
        {

            Dispose(false);
        }

        public void Start()
        {
            StartDevice(Handle);
        }
        public void Stop()
        {
            StopDevice(Handle);
        }

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }
        internal void ShowPreview()
        {
            ShowPreviewWindow(Handle);
        }

        void Dispose(bool bDisposing)
        {
            if (Handle != IntPtr.Zero)
            {
                DestroyDevice(Handle);
                Handle = IntPtr.Zero;
            }
        }
    }
    internal  class CaptureEngine
    {
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention=CallingConvention.Cdecl)]
        private static extern int EnumDevices(IntPtr dev);

        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreatePipeline();
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void DestroyPipeline(IntPtr pipeLine);

        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void AddVideoSource(IntPtr pipeLine,IntPtr device);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void AddAudioSource(IntPtr pipeLine, IntPtr device);

        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetOutputFile(IntPtr pipeLine,[MarshalAs(UnmanagedType.LPStr)] string fileName);
        
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Init(IntPtr pipeLine);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Close(IntPtr pipeLine);


        private IntPtr m_pipeLine;

        public static List<CaptureDeviceInfo> GetDevices()
        {
            List<CaptureDeviceInfo> devices = new List<CaptureDeviceInfo>();
            IntPtr temp = IntPtr.Zero;
            try
            {

                int size = Marshal.SizeOf(typeof(CaptureDeviceInfo));
                var handle=Marshal.AllocHGlobal(size*32);
                EnumDevices(handle);
                for (int i = 0; i < 32; i++)
                {
                    CaptureDeviceInfo dev = Marshal.PtrToStructure<CaptureDeviceInfo>(handle);
                    handle += size;
                    if (dev.DeviceType!=CaptureDeviceType.None)
                        devices.Add(dev);
                }

            }
            catch (Exception e)
            {
                e = e;
            }
            finally
            {
                Marshal.FreeHGlobal(temp);

            }
            return devices;
         }

        public CaptureEngine()
        {
            m_pipeLine = CreatePipeline();
        }

        public void Init()
        {
            Init(m_pipeLine);
        }
        public void Close()
        {
            if (m_pipeLine != IntPtr.Zero)
            {
                Close(m_pipeLine);
                DestroyPipeline(m_pipeLine);
                m_pipeLine = IntPtr.Zero;
            }
        }
        public void AddVideoSource(Device dev)
        {
            AddVideoSource(m_pipeLine, dev.Handle);
        }
        public void AddAudioSource(Device dev)
        {
            AddAudioSource(m_pipeLine, dev.Handle);
        }
        public void SetOutputFile(string outputFileName)
        {
            SetOutputFile(m_pipeLine, outputFileName);
        }
        public PreviewWindow PreviewSource(CaptureDeviceInfo device)
        {
            
            return new PreviewWindow();
        }

        internal void Dispose()
        {
            Close();
        }
    }
}
