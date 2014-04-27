using Kaltura.CaptureEngine;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Data;
using System.Windows.Input;
using System.Windows.Threading;

namespace Capture
{
    class ViewModel : DependencyObject
    {
        DispatcherTimer m_timer;
        CaptureLogic m_captureLogic;
        List<CaptureDeviceInfo> m_deviceInfo = CaptureEngine.GetDevices();

        public ViewModel()
        {
            StartRecorderingCommand = new ExtendedCommand(StartRecord, true);
            StopRecordingCommand = new ExtendedCommand(StopRecord, false);
            PauseRecorderingCommand = new ExtendedCommand(PauseRecord, false);
            ResumeRecordingCommand = new ExtendedCommand(ResumeRecord, false);

            IsCapturingScreen = true;
        }
        public ExtendedCommand StartRecorderingCommand { get; set; }
        public ExtendedCommand StopRecordingCommand { get; set; }
        public ExtendedCommand PauseRecorderingCommand { get; set; }
        public ExtendedCommand ResumeRecordingCommand { get; set; }

        public void StartRecord()
        {

            m_captureLogic = new CaptureLogic();
            if (IsCapturingCamera)
            {
                m_captureLogic.Camera = Device.FromCaptureDevice(m_deviceInfo[1]);
            }
            if (IsCapturingMic)
            {
                m_captureLogic.Mic = Device.FromCaptureDevice(m_deviceInfo[2]);
            }
            if (IsCapturingScreen)
            {
              //  m_captureLogic.Screen=Device.FromDesktop(IntPtr.Zero)
            }
            m_captureLogic.ScreenCaptureFileName = @"c:\screen.mp4";
            m_captureLogic.CameraCaptureFileName = @"c:\camera.mp4";
            m_captureLogic.Start();


            ResumeRecord();
        }

        private void CalcCommandStates()
        {

            StartRecorderingCommand.CanExecuteCommand = (m_captureLogic==null);
            StopRecordingCommand.CanExecuteCommand = (m_captureLogic!=null);
            PauseRecorderingCommand.CanExecuteCommand = !IsRecording;
            ResumeRecordingCommand.CanExecuteCommand = IsRecording;
        }

        public void PauseRecord()
        {
            CalcCommandStates();

            m_captureLogic.Stop();
            IsRecording = false;
            m_timer.Stop();
            m_timer = null;
        }

        public void ResumeRecord()
        {
            CalcCommandStates();
            IsRecording = true;
            m_timer = new DispatcherTimer();
            m_timer.Interval = TimeSpan.FromSeconds(1);
            m_timer.Tick += m_timer_Tick;
            m_timer.Start();
        }

        void m_timer_Tick(object sender, EventArgs e)
        {
            RecordingTime = m_captureLogic.RecordingTime; 
        }
        public void StopRecord()
        {
            PauseRecord();

            m_captureLogic.Stop();

            m_captureLogic.Dispose();

            m_captureLogic = null;
            CalcCommandStates();
        }



        public bool IsCapturingScreen
        {
            get { return (bool)GetValue(IsCapturingScreenProperty); }
            set { SetValue(IsCapturingScreenProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IsCapturingScreen.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsCapturingScreenProperty =
            DependencyProperty.Register("IsCapturingScreen", typeof(bool), typeof(ViewModel), new PropertyMetadata(true));




        public bool IsCapturingCamera
        {
            get { return (bool)GetValue(IsCapturingCameraProperty); }
            set { SetValue(IsCapturingCameraProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IsCapturingCamera.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsCapturingCameraProperty =
            DependencyProperty.Register("IsCapturingCamera", typeof(bool), typeof(ViewModel), new PropertyMetadata(true));




        public bool IsCapturingMic
        {
            get { return (bool)GetValue(IsCapturingMicProperty); }
            set { SetValue(IsCapturingMicProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IsCapturingMic.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsCapturingMicProperty =
            DependencyProperty.Register("IsCapturingMic", typeof(bool), typeof(ViewModel), new PropertyMetadata(true));

        
        

        public bool IsRecording
        {
            get { return (bool)GetValue(IsRecordingProperty); }
            set { SetValue(IsRecordingProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IsRecording.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsRecordingProperty =
            DependencyProperty.Register("IsRecording", typeof(bool), typeof(ViewModel), new PropertyMetadata(false));




        public TimeSpan RecordingTime
        {
            get { return (TimeSpan)GetValue(RecordingTimeProperty); }
            set { SetValue(RecordingTimeProperty, value); }
        }

        // Using a DependencyProperty as the backing store for RecordingTime.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty RecordingTimeProperty =
            DependencyProperty.Register("RecordingTime", typeof(TimeSpan), typeof(ViewModel), new PropertyMetadata(TimeSpan.Zero));

        
        
    }
    public class BooleanConverter<T> : IValueConverter
    {
        public BooleanConverter(T trueValue, T falseValue)
        {
            True = trueValue;
            False = falseValue;
        }

        public T True { get; set; }
        public T False { get; set; }

        public virtual object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value is bool && ((bool)value) ? True : False;
        }

        public virtual object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            return value is T && EqualityComparer<T>.Default.Equals((T)value, True);
        }
    }
    public sealed class BooleanToVisibilityConverter : BooleanConverter<Visibility>
    {
        public BooleanToVisibilityConverter() :
            base(Visibility.Visible, Visibility.Collapsed) { }
    }
    public sealed class BooleanConverter : BooleanConverter<bool>
    {
        public BooleanConverter() :
            base(true, false)
        {
        }
    }
    public class ExtendedCommand : ICommand, INotifyPropertyChanged
    {
        private Action m_executer;
        private bool m_canExecute;
        private string m_commandName = String.Empty;

        public ExtendedCommand(Action executer, bool canExecute)
        {
            m_executer = executer;
            m_canExecute = canExecute;
        }

        public ExtendedCommand(Action executer, bool canExecute, string commandName)
            : this(executer, canExecute)
        {
            CommandName = commandName;
        }

        public bool CanExecute(object parameter)
        {
            return m_canExecute;
        }

        public event EventHandler CanExecuteChanged;

        public void Execute(object parameter)
        {
            if (m_executer != null)
            {
                m_executer();
            }
        }

        public bool CanExecuteCommand
        {
            get
            {
                return m_canExecute;
            }
            set
            {
                if (m_canExecute != value)
                {
                    m_canExecute = value;
                    if (CanExecuteChanged != null)
                        CanExecuteChanged(this, EventArgs.Empty);
                    NotifyPropertyChanged("CanExecuteCommand");
                }
            }
        }

        public string CommandName
        {
            get
            {
                return m_commandName;
            }
            set
            {
                if (m_commandName != value)
                {
                    m_commandName = value;
                    NotifyPropertyChanged("CommandName");
                }
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;
        protected void NotifyPropertyChanged(string propertyName)
        {
            try
            {
                if (PropertyChanged != null)
                    PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
            catch (Exception e)
            {
            }
        }
    }
}
