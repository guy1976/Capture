using MahApps.Metro.Controls;
using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Forms;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Capture
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : MetroWindow
    {
        NotifyIcon m_notifyIcon;
        ViewModel m_viewModel = new ViewModel();

        public MainWindow()
        {
            InitializeComponent();
            m_notifyIcon = new NotifyIcon();
            m_notifyIcon.Icon = new System.Drawing.Icon(@"D:\Dev\Capture\Capture\Images\Kaltura-Sun.ico");
            m_notifyIcon.Visible = true;
            m_notifyIcon.Click += m_notifyIcon_Click;
            DataContext = m_viewModel;

        }

        void m_notifyIcon_Click(object sender, EventArgs e)
        {
            
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            m_notifyIcon.Dispose();
            Close();
        }

        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.LeftButton == MouseButtonState.Pressed)
            {
                e.Handled = true;
                this.DragMove();
            }
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            var cm = this.FindResource("Options2") as System.Windows.Controls.ContextMenu;
            cm.PlacementTarget = sender as System.Windows.Controls.Button;
            cm.Placement = System.Windows.Controls.Primitives.PlacementMode.Bottom;
            cm.IsOpen = true;
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            AvailableInputsPopup.IsOpen = true;

        }

        private void Button_Click_3(object sender, RoutedEventArgs e)
        {

        }

        private void Button_Click_4(object sender, RoutedEventArgs e)
        {
            var cm = this.FindResource("Options2") as System.Windows.Controls.ContextMenu;
            cm.PlacementTarget = sender as System.Windows.Controls.Button;
            cm.Placement = System.Windows.Controls.Primitives.PlacementMode.Bottom;
            cm.IsOpen = true;

        }
    }
}
 