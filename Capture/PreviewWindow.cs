using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Interop;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Capture
{

    public class PreviewWindow : HwndHost
    {
        protected override HandleRef BuildWindowCore(System.Runtime.InteropServices.HandleRef hwndParent)
        {
            return new HandleRef(this, IntPtr.Zero);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
        }
    }
}
