using Kaltura.CaptureEngine;
using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Runtime.InteropServices;
using System.Windows;

namespace Capture
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        static App()
        {
         /*   
            var devices = CaptureEngine.GetDevices();
            try
            {
                Device cam = Device.FromCaptureDevice(devices[1]);
                Device mic = Device.FromCaptureDevice(devices[2]);
                CaptureEngine cap = new CaptureEngine();
                cap.SetOutputFile(@"c:\1.mp4");
                cap.AddVideoSource(cam);
                cap.AddAudioSource(mic);
                cap.Start();
                cam.ShowPreview();
                System.Threading.Thread.Sleep(5000);
                cap.Stop();
            }
            catch (Exception e)
            {
                e = e;
            }*/
        }
        private void Button_Click(object sender, RoutedEventArgs e)
        {
            MainWindow.Close();
        }
    }
}
