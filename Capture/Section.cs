using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace Capture
{
    public class Section : ContentControl
    {
        static Section()
        {
            DefaultStyleKeyProperty.OverrideMetadata(typeof(Section), new FrameworkPropertyMetadata(typeof(Section)));
        }



        public string Title
        {
            get { return (string)GetValue(TitleProperty); }
            set { SetValue(TitleProperty, value); }
        }



        public static UIElement GetBorderContent(DependencyObject obj)
        {
            return (UIElement)obj.GetValue(BorderContentProperty);
        }

        public static void SetBorderContent(DependencyObject obj, UIElement value)
        {
            obj.SetValue(BorderContentProperty, value);
        }

        // Using a DependencyProperty as the backing store for BorderContent.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty BorderContentProperty =
            DependencyProperty.RegisterAttached("BorderContent", typeof(UIElement), typeof(Section), new PropertyMetadata(null));

        
        // Using a DependencyProperty as the backing store for Title.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty TitleProperty =
            DependencyProperty.Register("Title", typeof(string), typeof(Section), new PropertyMetadata(string.Empty));

       
    }
}
