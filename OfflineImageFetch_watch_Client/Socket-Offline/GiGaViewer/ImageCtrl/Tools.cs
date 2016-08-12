using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Runtime.InteropServices;
using System.IO;
using System.Runtime.Serialization.Formatters.Binary;
using System.Reflection;
using System.Diagnostics;

namespace ImageCtrl
{
    public class ImageData<T>
    {
        public T[] Data;
        public int Stride;
        public int Width;
        public int Height;
        public int Channel;

        public T this[int x, int y, int c]
        {
            get
            {
                return Data[y * Stride + x * Channel + c];
            }
            set
            {
                Data[y * Stride + x * Channel + c] = value;
            }
        }
    }



    class Tools
    {
        public static Bitmap ResizeAndGray(string file, int newWidth, int newHeight)
        {
            if (!File.Exists(file))
            {
                return null;
            }

            Bitmap bmp = Tools.OpenBitmap(file);
            Bitmap resizedBmp = Tools.ResizeImage(bmp, newWidth, newHeight) as Bitmap;
            bmp.Dispose();

            ImageData<byte> imgData = Tools.Bitmap2ImageData(resizedBmp);
            resizedBmp.Dispose();

            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    byte Y = (byte)GetY(imgData, x, y);
                    imgData[x, y, 0] = Y;
                    imgData[x, y, 1] = Y;
                    imgData[x, y, 2] = Y;
                }
            }

            return Tools.Data2Bitmap(imgData.Data, imgData.Width, imgData.Height);
        }

        public static string GetCurrentDir()
        {
            return Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
        }

        public static void Save(string fileName, object obj)
        {
            try
            {
                FileStream fss = new FileStream(fileName, FileMode.Create);
                BinaryFormatter formatter = new BinaryFormatter();
                formatter.Serialize(fss, obj);
                fss.Close();
            }
            catch (Exception ex)
            {
                throw (ex);
            }
        }

        public static object Open(string fileName)
        {
            try
            {
                FileStream fss = new FileStream(fileName, FileMode.Open);
                BinaryFormatter formatter = new BinaryFormatter();
                object obj = formatter.Deserialize(fss);
                fss.Close();

                return obj;
            }
            catch (Exception ex)
            {
                throw (ex);
            }
        }

        public static Random RandomInit()
        {
            return new Random(Guid.NewGuid().GetHashCode());
        }

        public static void GetAllFiles(string dir, List<string> fileList, string pattern)
        {
            if (!Directory.Exists(dir))
            {
                return;
            }

            string[] files = Directory.GetFiles(dir, pattern);
            fileList.AddRange(files);

            string[] dirs = Directory.GetDirectories(dir);

            foreach (string d in dirs)
            {
                GetAllFiles(d, fileList, pattern);
            }
        }

        public static Bitmap ToGray(Bitmap bmp)
        {
            ImageData<byte> bmpData = Tools.Bitmap2ImageData(bmp);
            Bitmap gray = bmp.Clone() as Bitmap;
            ImageData<byte> grayData = Tools.Bitmap2ImageData(gray);
            gray.Dispose();

            for (int y = 0; y < bmpData.Height; y++)
            {
                for (int x = 0; x < bmpData.Width; x++)
                {
                    byte Y = (byte)GetY(bmpData, x, y);
                    grayData[x, y, 0] = Y;
                    grayData[x, y, 1] = Y;
                    grayData[x, y, 2] = Y;
                }
            }

            return Tools.Data2Bitmap(grayData.Data, bmpData.Width, bmpData.Height);
        }

        static int GetY(ImageData<byte> imgData, int x, int y)
        {
            int r = imgData[x, y, 2];
            int g = imgData[x, y, 1];
            int b = imgData[x, y, 0];

            int Y = (int)(0.299f * r + 0.587f * g + 0.114f * b + 0.5f);

            Y = Y > 255 ? 255 : Y;
            Y = Y < 0 ? 0 : Y;

            return Y;
        }

        public static Bitmap GetRectImage(Bitmap bmp, Rectangle rect)
        {
            ImageData<byte> bmpData = Tools.Bitmap2ImageData(bmp);
            Bitmap subBmp = new Bitmap(rect.Width, rect.Height, PixelFormat.Format24bppRgb);
            subBmp.SetResolution(96, 96);
            ImageData<byte> subData = Tools.Bitmap2ImageData(subBmp);

            rect.Intersect(new Rectangle(0, 0, bmp.Width, bmp.Height));

            for (int y = rect.Top; y < rect.Bottom; y++)
            {
                for (int x = rect.Left; x < rect.Right; x++)
                {
                    subData[x - rect.Left, y - rect.Top, 0] = bmpData[x, y, 0];
                    subData[x - rect.Left, y - rect.Top, 1] = bmpData[x, y, 1];
                    subData[x - rect.Left, y - rect.Top, 2] = bmpData[x, y, 2];
                }
            }

            subBmp.Dispose();
            return Tools.Data2Bitmap(subData.Data, subData.Width, subData.Height);
        }

        public static ImageData<byte> MirrorH(ImageData<byte> bmp)
        {
            ImageData<byte> result = new ImageData<byte>();
            result.Channel = bmp.Channel;
            result.Data = new byte[bmp.Data.Length];
            result.Width = bmp.Width;
            result.Height = bmp.Height;
            result.Stride = bmp.Stride;

            for (int y = 0; y < bmp.Height; y++)
            {
                for (int x = 0; x < bmp.Width; x++)
                {
                    result[bmp.Width - 1 - x, y, 0] = bmp[x, y, 0];
                    result[bmp.Width - 1 - x, y, 1] = bmp[x, y, 1];
                    result[bmp.Width - 1 - x, y, 2] = bmp[x, y, 2];
                }
            }

            return result;
        }

        public static Bitmap AddMap(ImageData<byte> bmp1, ImageData<byte> bmp2)
        {
            ImageData<int> ret = new ImageData<int>();
            ret.Channel = 1;
            ret.Data = new int[bmp1.Width * bmp1.Height];
            ret.Width = bmp1.Width;
            ret.Height = bmp1.Height;
            ret.Stride = bmp1.Width;

            for (int y = 0; y < bmp1.Height; y++)
            {
                for (int x = 0; x < bmp1.Width; x++)
                {
                    ret[x, y, 0] = bmp1[x, y, 0] + bmp2[x, y, 0];
                }
            }

            return Tools.ImageData2Bitmap(ret);
        }

        public static Bitmap MultiplyMap(ImageData<byte> bmp1, ImageData<byte> bmp2)
        {
            ImageData<int> ret = new ImageData<int>();
            ret.Channel = 1;
            ret.Data = new int[bmp1.Width * bmp1.Height];
            ret.Width = bmp1.Width;
            ret.Height = bmp1.Height;
            ret.Stride = bmp1.Width;

            for (int y = 0; y < bmp1.Height; y++)
            {
                for (int x = 0; x < bmp1.Width; x++)
                {
                    ret[x, y, 0] = bmp1[x, y, 0] * bmp2[x, y, 0];
                }
            }

            return Tools.ImageData2Bitmap(ret);
        }

        public static void HSV2XYZ(float H, float S, float V, out float X, out float Y, out float Z)
        {
            X = (float)(S * Math.Cos(H * Math.PI / 180.0));
            Y = (float)(S * Math.Sin(H * Math.PI / 180.0));
            Z = V;
        }
     
        public static void XYZ2HSV(float X, float Y, float Z, out float H, out float S, out float V)
        {
            float s = (float)(Math.Sqrt(X * X + Y * Y));
            float v = Z;

            float angle = (float)(Math.Atan2(Y, X) * 180.0 / Math.PI);
            if (angle < 0)
            {
                angle = 360f + angle;
            }

            H = angle;
            S = s;
            V = v;
        }

        public static ImageData<float> RGB2XYZ(ImageData<byte> imgData)
        {
            ImageData<float> xyz = new ImageData<float>();
            xyz.Channel = 3;
            xyz.Data = new float[imgData.Width * imgData.Height * 3];
            xyz.Width = imgData.Width;
            xyz.Height = imgData.Height;
            xyz.Stride = imgData.Width * 3;

            int h, s, v;
            float xx, yy, zz;
            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    RGB2HSV(imgData[x, y, 2], imgData[x, y, 1], imgData[x, y, 0], out h, out s, out v);
                    HSV2XYZ(h, s, v, out xx, out yy, out zz);
                    xyz[x, y, 0] = xx;
                    xyz[x, y, 1] = yy;
                    xyz[x, y, 2] = zz;
                }
            }

            return xyz;
        }

        public static ImageData<int> RGB2HSV(ImageData<byte> imgData)
        {
            ImageData<int> hsv = new ImageData<int>();
            hsv.Channel = 3;
            hsv.Data = new int[imgData.Width * imgData.Height * 3];
            hsv.Width = imgData.Width;
            hsv.Height = imgData.Height;
            hsv.Stride = imgData.Width * 3;

            int h, s, v;

            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    RGB2HSV(imgData[x, y, 2], imgData[x, y, 1], imgData[x, y, 0], out h, out s, out v);
                    hsv[x, y, 0] = h;
                    hsv[x, y, 1] = s;
                    hsv[x, y, 2] = v;
                }
            }

            return hsv;
        }

        public static void RGB2HSV(int r, int g, int b, out int h, out int s, out int v)
        {
            int max, min, diff;
            float hf = 0;

            min = Math.Min(Math.Min(b, g), r);
            max = Math.Max(Math.Max(b, g), r);
            diff = max - min;

            if (diff == 0)
            {
                hf = 0.0f;
            }
            else
            {
                if (r == max) { hf = (g - b) / (float)diff; }
                if (g == max) { hf = 2 + (b - r) / (float)diff; }
                if (b == max) { hf = 4 + (r - g) / (float)diff; }
                hf = hf * 60.0f;
                if (hf < 0) { hf = hf + 360.0f; }
            }
            h = (int)(hf + 0.5);
 
            if (max == 0)
            {
                s = 0;
            }
            else
            {
                s = diff * 100 / max;
            }
 
            v = 100 * max / 255;
        }



        public static ImageData<byte> YCC2RGB(ImageData<byte> imgData)
        {
            float r, g, b;
            ImageData<byte> newData = new ImageData<byte>();
            newData.Channel = 3;
            newData.Data = new byte[imgData.Data.Length];
            newData.Width = imgData.Width;
            newData.Height = imgData.Height;
            newData.Stride = imgData.Stride;

            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    byte Cr = imgData[x, y, 2];
                    byte Cb = imgData[x, y, 1];
                    byte Y = imgData[x, y, 0];

                    r = Y + 1.402f * (Cr - 128f);
                    g = Y - 0.344f * (Cb - 128f) - 0.714f * (Cr - 128f);
                    b = Y + 1.772f * (Cb - 128f);

                    if (r > 255)
                    {
                        r = 255;
                    }
                    if (r < 0)
                    {
                        r = 0;
                    }

                    if (g > 255)
                    {
                        g = 255;
                    }
                    if (g < 0)
                    {
                        g = 0;
                    }

                    if (b > 255)
                    {
                        b = 255;
                    }
                    if (b < 0)
                    {
                        b = 0;
                    }

                    newData[x, y, 0] = (byte)b;
                    newData[x, y, 1] = (byte)g;
                    newData[x, y, 2] = (byte)r;
                }
            }

            return newData;
        }

        public static ImageData<byte> RGB2YCC(ImageData<byte> imgData)
        {
            float Y, Cb, Cr;
            ImageData<byte> newData = new ImageData<byte>();
            newData.Channel = 3;
            newData.Data = new byte[imgData.Data.Length];
            newData.Width = imgData.Width;
            newData.Height = imgData.Height;
            newData.Stride = imgData.Stride;

            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    byte r = imgData[x, y, 2];
                    byte g = imgData[x, y, 1];
                    byte b = imgData[x, y, 0];

                    Y = 0.299f * r + 0.587f * g + 0.114f * b;
                    Cb = -0.168736f * r - 0.331264f * g + 0.5f * b + 128;
                    Cr = 0.5f * r - 0.418688f * g - 0.081312f * b + 128;

                    if (Y > 255)
                    {
                        Y = 255;
                    }
                    if (Y < 0)
                    {
                        Y = 0;
                    }

                    if (Cb > 255)
                    {
                        Cb = 255;
                    }
                    if (Cb < 0)
                    {
                        Cb = 0;
                    }

                    if (Cr > 255)
                    {
                        Cr = 255;
                    }
                    if (Cr < 0)
                    {
                        Cr = 0;
                    }

                    newData[x, y, 0] = (byte)Y;
                    newData[x, y, 1] = (byte)Cb;
                    newData[x, y, 2] = (byte)Cr;
                }
            }

            return newData;
        }

        public static ImageData<float> RGB2YCC2(ImageData<byte> imgData)
        {
            float Y, Cb, Cr;
            ImageData<float> newData = new ImageData<float>();
            newData.Channel = 3;
            newData.Data = new float[imgData.Data.Length];
            newData.Width = imgData.Width;
            newData.Height = imgData.Height;
            newData.Stride = imgData.Stride;

            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    byte r = imgData[x, y, 2];
                    byte g = imgData[x, y, 1];
                    byte b = imgData[x, y, 0];

                    Y = 0.299f * r + 0.587f * g + 0.114f * b;
                    Cb = -0.168736f * r - 0.331264f * g + 0.5f * b + 128;
                    Cr = 0.5f * r - 0.418688f * g - 0.081312f * b + 128;

                    if (Y > 255)
                    {
                        Y = 255;
                    }
                    if (Y < 0)
                    {
                        Y = 0;
                    }

                    if (Cb > 255)
                    {
                        Cb = 255;
                    }
                    if (Cb < 0)
                    {
                        Cb = 0;
                    }

                    if (Cr > 255)
                    {
                        Cr = 255;
                    }
                    if (Cr < 0)
                    {
                        Cr = 0;
                    }

                    newData[x, y, 0] = Y;
                    newData[x, y, 1] = Cb;
                    newData[x, y, 2] = Cr;
                }
            }

            return newData;
        }

        public static Bitmap ImageData2Bitmap(ImageData<int> imgData)
        {
            ImageData<byte> bmpData = new ImageData<byte>();
            List<int> floatData = new List<int>();

            bmpData.Width = imgData.Width;
            bmpData.Height = imgData.Height;
            bmpData.Stride = (imgData.Width * 3 * 8 + 31) / 32 * 4;
            bmpData.Channel = 3;
            bmpData.Data = new byte[bmpData.Stride * bmpData.Height];

            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    floatData.Add(imgData[x, y, 0]);
                }
            }

            int maxInfo = int.MinValue;
            int minInfo = int.MaxValue;

            for (int i = 0; i < floatData.Count; i++)
            {
                if (minInfo > floatData[i])
                {
                    minInfo = floatData[i];
                }
                if (maxInfo < floatData[i])
                {
                    maxInfo = floatData[i];
                }
            }

            int index = 0;
            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    int val = (int)((floatData[index] - minInfo) * 255f / (maxInfo - minInfo) + 0.5f);
                    if (val < 0)
                    {
                        val = 0;
                    }
                    if (val > 255)
                    {
                        val = 255;
                    }
                    bmpData[x, y, 0] = (byte)val;
                    bmpData[x, y, 1] = (byte)val;
                    bmpData[x, y, 2] = (byte)val;

                    index++;
                }
            }

            Bitmap bmp = Data2Bitmap(bmpData.Data, bmpData.Width, bmpData.Height);

            return bmp;
        }

        public static Bitmap ImageData2Bitmap(ImageData<float> imgData)
        {
            ImageData<byte> bmpData = new ImageData<byte>();
            List<float> floatData = new List<float>();

            bmpData.Width = imgData.Width;
            bmpData.Height = imgData.Height;
            bmpData.Stride = (imgData.Width * 3 * 8 + 31) / 32 * 4;
            bmpData.Channel = 3;
            bmpData.Data = new byte[bmpData.Stride * bmpData.Height];

            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    floatData.Add(imgData[x, y, 0]);
                }
            }

            float maxInfo = float.MinValue;
            float minInfo = float.MaxValue;

            for (int i = 0; i < floatData.Count; i++)
            {
                if (minInfo > floatData[i])
                {
                    minInfo = floatData[i];
                }
                if (maxInfo < floatData[i])
                {
                    maxInfo = floatData[i];
                }
            }

            int index = 0;
            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    int val = (int)((floatData[index] - minInfo) * 255f / (maxInfo - minInfo) + 0.5f);
                    if (val < 0)
                    {
                        val = 0;
                    }
                    if (val > 255)
                    {
                        val = 255;
                    }
                    bmpData[x, y, 0] = (byte)val;
                    bmpData[x, y, 1] = (byte)val;
                    bmpData[x, y, 2] = (byte)val;

                    index++;
                }
            }

            Bitmap bmp = Data2Bitmap(bmpData.Data, bmpData.Width, bmpData.Height);

            return bmp;
        }

        public static Bitmap ImageData2Bitmap_NoNormalize(ImageData<float> imgData)
        {
            ImageData<byte> bmpData = new ImageData<byte>();
            List<float> floatData = new List<float>();

            bmpData.Width = imgData.Width;
            bmpData.Height = imgData.Height;
            bmpData.Stride = (imgData.Width * 3 * 8 + 31) / 32 * 4;
            bmpData.Channel = 3;
            bmpData.Data = new byte[bmpData.Stride * bmpData.Height];

            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    floatData.Add(imgData[x, y, 0]);
                }
            }

            int index = 0;
            for (int y = 0; y < imgData.Height; y++)
            {
                for (int x = 0; x < imgData.Width; x++)
                {
                    int val = (int)(floatData[index] + 0.5f);
                    if (val < 0)
                    {
                        //val = 0;
                        val = -val;
                    }
                    if (val > 255)
                    {
                        val = 255;
                    }
                    bmpData[x, y, 0] = (byte)val;
                    bmpData[x, y, 1] = (byte)val;
                    bmpData[x, y, 2] = (byte)val;

                    index++;
                }
            }
            Bitmap bmp = Data2Bitmap(bmpData.Data, bmpData.Width, bmpData.Height);
            return bmp;
        }

        public static ImageData<byte> Bitmap2ImageData(Bitmap bmp)
        {
            ImageData<byte> img = new ImageData<byte>();

            img.Data = Bitmap2Data(bmp, out img.Stride);
            img.Width = bmp.Width;
            img.Height = bmp.Height;
            img.Channel = 3;

            return img;
        }

        public static void Data2Bitmap(byte[] imageData, Bitmap dstBmp)
        {
            Rectangle rect = new Rectangle(0, 0, dstBmp.Width, dstBmp.Height);
            BitmapData bmpData = dstBmp.LockBits(rect, ImageLockMode.ReadOnly, dstBmp.PixelFormat);

            int bytes = Math.Abs(bmpData.Stride) * dstBmp.Height;

            IntPtr ptr = bmpData.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(imageData, 0, ptr, bytes);

            dstBmp.UnlockBits(bmpData);
        }


        public static Bitmap Data2Bitmap(byte[] imageData, int width, int height)
        {
            Bitmap bmp = new Bitmap(width, height, PixelFormat.Format24bppRgb);

            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            BitmapData bmpData = bmp.LockBits(rect, ImageLockMode.ReadOnly, bmp.PixelFormat);

            int bytes = Math.Abs(bmpData.Stride) * bmp.Height;

            IntPtr ptr = bmpData.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(imageData, 0, ptr, bytes);

            bmp.UnlockBits(bmpData);

            return bmp;
        }

        public static Bitmap OneChannelData2Bitmap(byte[] imageData, int width, int height)
        {
            Bitmap bmp = new Bitmap(width, height, PixelFormat.Format24bppRgb);

            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            BitmapData bmpData = bmp.LockBits(rect, ImageLockMode.ReadOnly, bmp.PixelFormat);
            int stride = bmpData.Stride;
            int bytes = Math.Abs(stride) * bmp.Height;

            byte[] newData = new byte[bytes];
            for (int y = 0; y < height; y++)
            {
                for (int x = 0; x < width; x++)
                {
                    int oldIndex = y * width + x;
                    int newIndex = y * stride + x * 3;

                    newData[newIndex + 0] = imageData[oldIndex];
                    newData[newIndex + 1] = imageData[oldIndex];
                    newData[newIndex + 2] = imageData[oldIndex];
                }
            }

            IntPtr ptr = bmpData.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(newData, 0, ptr, bytes);

            bmp.UnlockBits(bmpData);

            return bmp;
        }

        public static byte[] Bitmap2Data(Bitmap bmp)
        {
            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            BitmapData bmpData = bmp.LockBits(rect, ImageLockMode.ReadOnly, bmp.PixelFormat);
            int bytes = Math.Abs(bmpData.Stride) * bmp.Height;
            byte[] data = new byte[bytes];

            IntPtr ptr = bmpData.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(ptr, data, 0, bytes);

            bmp.UnlockBits(bmpData);

            return data;
        }

        public static byte[] Bitmap2Data(Bitmap bmp, out int stride)
        {
            Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
            BitmapData bmpData = bmp.LockBits(rect, ImageLockMode.ReadOnly, bmp.PixelFormat);
            int bytes = Math.Abs(bmpData.Stride) * bmp.Height;
            byte[] data = new byte[bytes];
            stride = bmpData.Stride;

            IntPtr ptr = bmpData.Scan0;
            System.Runtime.InteropServices.Marshal.Copy(ptr, data, 0, bytes);

            bmp.UnlockBits(bmpData);

            return data;
        }

        public static Bitmap OpenBitmap(string file)
        {
            Bitmap original = new Bitmap(file);
            original.SetResolution(96, 96);
            Bitmap bmp = new Bitmap(original.Width, original.Height, PixelFormat.Format24bppRgb);
            bmp.SetResolution(96, 96);
            Graphics graphic = Graphics.FromImage((Image)bmp);
            graphic.DrawImage(original, 0, 0, original.Width, original.Height);
            graphic.Dispose();
            original.Dispose();

            return bmp;
        }

        public static Image ResizeImage(Image img, int newWidth, int newHeight)
        {
            //get the height and width of the image
            int originalW = img.Width;
            int originalH = img.Height;

            //get the new size based on the percentage change
            int resizedW = newWidth;
            int resizedH = newHeight;

            //create a new Bitmap the size of the new image
            Bitmap bmp = new Bitmap(resizedW, resizedH, PixelFormat.Format24bppRgb);
            bmp.SetResolution(96, 96);
            //create a new graphic from the Bitmap
            Graphics graphic = Graphics.FromImage((Image)bmp);
            graphic.InterpolationMode = InterpolationMode.HighQualityBicubic;
            //draw the newly resized image
            graphic.DrawImage(img, 0, 0, resizedW, resizedH);
            //dispose and free up the resources
            graphic.Dispose();
            //return the image
            return (Image)bmp;
        }

        public static Image ResizeImageBilinear(Image img, int newWidth, int newHeight)
        {
            //get the height and width of the image
            int originalW = img.Width;
            int originalH = img.Height;

            //get the new size based on the percentage change
            int resizedW = newWidth;
            int resizedH = newHeight;

            //create a new Bitmap the size of the new image
            Bitmap bmp = new Bitmap(resizedW, resizedH, PixelFormat.Format24bppRgb);
            bmp.SetResolution(96, 96);
            //create a new graphic from the Bitmap
            Graphics graphic = Graphics.FromImage((Image)bmp);
            graphic.InterpolationMode = InterpolationMode.Bilinear;
            //draw the newly resized image
            graphic.DrawImage(img, 0, 0, resizedW, resizedH);
            //dispose and free up the resources
            graphic.Dispose();
            //return the image
            return (Image)bmp;
        }

        public static Bitmap ResizeImageSimply(Bitmap img, int newWidth, int newHeight)
        {
            int originalW = img.Width;
            int originalH = img.Height;

            Bitmap bmp = new Bitmap(newWidth, newHeight, PixelFormat.Format24bppRgb);
            ImageData<byte> newData = Bitmap2ImageData(bmp);
            ImageData<byte> oldData = Bitmap2ImageData(img);

            for (int y = 0; y < newData.Height; y++)
            {
                for (int x = 0; x < newData.Width; x++)
                {
                    int old_x = (int)(x * (float)originalW / (float)newWidth);
                    int old_y = (int)(y * (float)originalH / (float)newHeight);
                    newData[x, y, 0] = oldData[old_x, old_y, 0];
                    newData[x, y, 1] = oldData[old_x, old_y, 1];
                    newData[x, y, 2] = oldData[old_x, old_y, 2];
                }
            }

            Data2Bitmap(newData.Data, bmp);

            return bmp;
        }

        static System.Drawing.Imaging.ImageCodecInfo GetEncoderInfo(String mimeType)
        {
            int i;
            System.Drawing.Imaging.ImageCodecInfo[] encoders;
            encoders = System.Drawing.Imaging.ImageCodecInfo.GetImageEncoders();
            for (i = 0; i < encoders.Length; ++i)
            {
                if (encoders[i].MimeType == mimeType)
                    return encoders[i];
            }
            return null;
        }

        public static void SaveToJpg(Image bmp, string fileName)
        {
            byte quality = 100;
            System.Drawing.Imaging.EncoderParameters encoderParams = new System.Drawing.Imaging.EncoderParameters(1);
            System.Drawing.Imaging.EncoderParameter encoderParam = new System.Drawing.Imaging.EncoderParameter(System.Drawing.Imaging.Encoder.Quality, (long)quality);
            encoderParams.Param[0] = encoderParam;
            bmp.Save(fileName, GetEncoderInfo("image/jpeg"), encoderParams);
        }

        public static Bitmap ChangeColor(Bitmap bmp, byte old_r, byte old_g, byte old_b, byte new_r, byte new_g, byte new_b)
        {
            ImageData<byte> bmpData = Bitmap2ImageData(bmp);

            for (int y = 0; y < bmpData.Height; y++)
            {
                for (int x = 0; x < bmpData.Width; x++)
                {
                    if (bmpData[x, y, 0] == old_b &&
                        bmpData[x, y, 1] == old_g &&
                        bmpData[x, y, 2] == old_r)
                    {
                        bmpData[x, y, 0] = new_b;
                        bmpData[x, y, 1] = new_g;
                        bmpData[x, y, 2] = new_r;
                    }
                }
            }

            return Data2Bitmap(bmpData.Data, bmpData.Width, bmpData.Height);
        }

        public static Bitmap ChangeColor(Bitmap bmp, byte old_r, byte old_g, byte old_b, byte new_r, byte new_g, byte new_b, out int sameColorCount)
        {
            sameColorCount = 0;
            ImageData<byte> bmpData = Bitmap2ImageData(bmp);

            for (int y = 0; y < bmpData.Height; y++)
            {
                for (int x = 0; x < bmpData.Width; x++)
                {
                    if (bmpData[x, y, 0] == old_b &&
                        bmpData[x, y, 1] == old_g &&
                        bmpData[x, y, 2] == old_r)
                    {
                        bmpData[x, y, 0] = new_b;
                        bmpData[x, y, 1] = new_g;
                        bmpData[x, y, 2] = new_r;

                        sameColorCount++;
                    }
                }
            }

            return Data2Bitmap(bmpData.Data, bmpData.Width, bmpData.Height);
        }

        public static Bitmap ShowDistanceBitmap(Bitmap bmp, byte rr, byte gg, byte bb)
        {
            float xx;
            float yy;
            float zz;
            int hh;
            int ss;
            int vv;
            Tools.RGB2HSV(rr, gg, bb, out hh, out ss, out vv);
            Tools.HSV2XYZ(hh, ss, vv, out xx, out yy, out zz);
            ImageData<int> hsvImg = RGB2HSV(Bitmap2ImageData(bmp));
            ImageData<float> diffImg = new ImageData<float>();
            diffImg.Channel = 1;
            diffImg.Width = hsvImg.Width;
            diffImg.Height = hsvImg.Height;
            diffImg.Stride = diffImg.Width;
            diffImg.Data = new float[diffImg.Stride * diffImg.Height];

            float para_y = 100f;
            float para_x = 0.1f;

            for (int y = 0; y < hsvImg.Height; y++)
            {
                for (int x = 0; x < hsvImg.Width; x++)
                {
                    float H = hsvImg[x, y, 0];
                    float S = hsvImg[x, y, 1];
                    float V = hsvImg[x, y, 2];

                    float X;
                    float Y;
                    float Z;

                    Tools.HSV2XYZ(H, S, V, out X, out Y, out Z);

                    if (x == 236 && y == 224)
                    {
                        int xxxx = 0;
                        xxxx++;
                    }

                    float dist2 = (xx - X) * (xx - X) + (yy - Y) * (yy - Y) + (zz - Z) * (zz - Z);
                    float diff = para_y * para_y / (dist2 * para_x + para_y);

                    diffImg[x, y, 0] = diff;
                }
            }

            return ImageData2Bitmap(diffImg);
        }
    }
}
