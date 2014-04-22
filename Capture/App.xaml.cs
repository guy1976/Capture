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
            devices = devices;
            try
            {
                CaptureEngine cap = new CaptureEngine();
                cap.SetOutputFile(@"c:\1.mp4");
                cap.AddSource(devices[1]);
                cap.AddSource(devices[2]);
                cap.Start();
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
