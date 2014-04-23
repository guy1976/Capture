using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Input;

namespace Capture
{
    class ViewModel : DependencyObject
    {
        public ViewModel()
        {
            StartRecorderingCommand = new ExtendedCommand(StartRecord, true);
        }
        public ExtendedCommand StartRecorderingCommand { get; set; }

        public void StartRecord()
        {
            StartRecorderingCommand.CanExecuteCommand = false;
        }
        public void StopRecord()
        {
            StartRecorderingCommand.CanExecuteCommand = true;
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
