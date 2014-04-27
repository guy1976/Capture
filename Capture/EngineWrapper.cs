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
	    Audio
    };

    [StructLayout(LayoutKind.Sequential, Pack = 1, CharSet = CharSet.Unicode)]
    internal struct CaptureDevice
    {
        public CaptureDeviceType DeviceType;
        [MarshalAs(UnmanagedType.ByValTStr,SizeConst = 100)]
        public string FriendlyName;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
	    public string FFMpegInput;
        [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 100)]
	    public string FFMpegDevice;
    };
    internal  class CaptureEngine
    {
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention=CallingConvention.Cdecl)]
        private static extern int EnumDevices(IntPtr dev);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr CreatePipeline();
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void AddSource(IntPtr pipeLine,ref CaptureDevice device);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void AddScreenSource(IntPtr pipeLine,IntPtr handle);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void SetOutputFile(IntPtr pipeLine,[MarshalAs(UnmanagedType.LPStr)] string fileName);
        
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Start(IntPtr pipeLine);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void Stop(IntPtr pipeLine);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern IntPtr ShowPreviewWindow(ref CaptureDevice device);
        [DllImport(@"Kaltura.CaptureEngine.dll", CallingConvention = CallingConvention.Cdecl)]
        private static extern void HidePreviewWindow(IntPtr t);
        private IntPtr m_pipeLine;

        public static List<CaptureDevice> GetDevices()
        {
            List<CaptureDevice> devices = new List<CaptureDevice>();
            IntPtr temp = IntPtr.Zero;
            try
            {

                int size = Marshal.SizeOf(typeof(CaptureDevice));
                var handle=Marshal.AllocHGlobal(size*32);
                EnumDevices(handle);
                for (int i = 0; i < 32; i++)
                {
                    CaptureDevice dev = Marshal.PtrToStructure<CaptureDevice>(handle);
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

        public void Start()
        {
            Start(m_pipeLine);
        }
        public void Stop()
        {
            Stop(m_pipeLine);
        }
        public void AddSource(CaptureDevice device)
        {
            AddSource(m_pipeLine, ref device);
        }
        public void SetOutputFile(string outputFileName)
        {
            SetOutputFile(m_pipeLine, outputFileName);
        }
        public PreviewWindow PreviewSource(CaptureDevice device)
        {
            
            return new PreviewWindow();
        }
    }
}
