using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace DPDtest1
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            Image img = Image.FromFile(@"D:\test1.jpg");//双引号里是图片的路径

            pictureBox1.Image = img;
        }

        private void button2_Click(object sender, EventArgs e)
        {
            int wt, ht,r,g,b,t;
            Image img = Image.FromFile(@"D:\test1.jpg");//双引号里是图片的路径
            wt = img.Width;
            ht = img.Height;
            Bitmap bitmap = new Bitmap(img);
            Color myColor = new Color();
            for (int i = 0; i < wt; i++)
            {
                for (int j = 0; j < ht; j++)
                {
                    
                    myColor = bitmap.GetPixel(i, j);
                    r = myColor.R;
                    g = myColor.G;
                    b = myColor.B;
                    t = (r + g + b) / 3;
                    if (t > 128)
                    {
                        bitmap.SetPixel(i, j, Color.FromArgb(255, 255, 255));
                    }
                    else
                    { 
                        bitmap.SetPixel(i, j, Color.FromArgb(0,0,0));
                    }

                }
            }
            pictureBox1.Image = bitmap;
        }

        private void button3_Click(object sender, EventArgs e)
        {
            int wt, ht, r, g, b, t;
            Image img = Image.FromFile(@"D:\test1.jpg");//双引号里是图片的路径
            wt = img.Width;
            ht = img.Height;
            Bitmap bitmap = new Bitmap(img);
            Color myColor = new Color();
            for (int i = 0; i < wt; i++)
            {
                for (int j = 0; j < ht; j++)
                {

                    myColor = bitmap.GetPixel(i, j);
                    r = myColor.R;
                    g = myColor.G;
                    b = myColor.B;
                    int d = Convert.ToInt32(0.11 * r + 0.59 * g + 0.3 * b);//获取整个图片的灰度
                    int a = log(d);
                    bitmap.SetPixel(i, j, Color.FromArgb(a, a, a));
                }
            }
            pictureBox1.Image = bitmap;
        }
        int log(int color)
        {
            double c = 1.0;
            double v = 5.0;
            double r, s;
            int d;
            r = color / 255.0;
            s = c * Math.Log(1 + r * v) / Math.Log(v + 1);
            d = (int)(s * 255);
            if (d < 0)
            {
                d = 0;
            }
            else if (d > 255)
            {
                d = 255;
            }
            return d;
        }

        private void button4_Click(object sender, EventArgs e)
        {
            int wt, ht, r, g, b, t;
            Image img = Image.FromFile(@"D:\test1.jpg");//双引号里是图片的路径
            wt = img.Width;
            ht = img.Height;
            Bitmap bitmap = new Bitmap(img);
            Color myColor = new Color();
            for (int i = 0; i < wt; i++)
            {
                for (int j = 0; j < ht; j++)
                {

                    myColor = bitmap.GetPixel(i, j);
                    r = myColor.R;
                    g = myColor.G;
                    b = myColor.B;
                    int d = Convert.ToInt32(0.11 * r + 0.59 * g + 0.3 * b);//获取整个图片的灰度
                    int a1 = gama(d);
                    bitmap.SetPixel(i, j, Color.FromArgb(a1, a1, a1));
                }
            }
            pictureBox1.Image = bitmap;
        }
        int gama(int color)
        {
            double c = 1.0;
            double gama = 0.1;
            double r = color / 255.0;
            double s = c * Math.Pow(r, gama);
            int d;
            d = (int)(255 * s);
            if (d < 0)
            {
                d = 0;
            }
            else if (d > 255)
            {
                d = 255;
            }
            return d;
        }
    }
}
