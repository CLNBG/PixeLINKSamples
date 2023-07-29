using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using PixeLINK;
using System.IO;
using OpenCvSharp;
using System.Drawing;
using OpenCvSharp.Extensions;
using System.Diagnostics;

namespace FocusStacking
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow
    {
        public MainWindow()
        {
            InitializeComponent();

            foreach (string sFile in System.IO.Directory.GetFiles(Directory.GetCurrentDirectory(), "*.bmp"))
            {
                System.IO.File.Delete(sFile);
            }


            List<float> focusValues = new List<float>();
            List<Mat> alteredImages = new List<Mat>();
            List<Mat> capturedImages = new List<Mat>();
            List<Bitmap> finalBitmaps = new List<Bitmap>();
            List<Bitmap> initialBitmaps = new List<Bitmap>();

            focusValues.Add(36208);
            focusValues.Add(25043);


            int gridWidth = 32;


            int hCamera = 0;

            Api.Initialize(0, ref hCamera);

            Api.SetStreamState(hCamera, StreamState.Start);

            FeatureFlags roiFlag = new FeatureFlags();
            int roiNumParms = 4;
            float[] roi = new float[4];
            Api.GetFeature(hCamera, Feature.Roi, ref roiFlag, ref roiNumParms, roi);






            Stopwatch st = new Stopwatch();
            st.Start();

            focusValues.Sort();

            Api.SetStreamState(hCamera, StreamState.Start);

            foreach (float focusValue in focusValues)
            {
                float[] f = { focusValue };
                Api.SetFeature(hCamera, Feature.Focus, FeatureFlags.Manual, 1, f);


                int bufferSize = 5000 * 5000 * 3;//sufficiently large buffer
                byte[] buffer = new byte[bufferSize];
                FrameDescriptor frameDesc = new FrameDescriptor();
                Api.GetNextFrame(hCamera, bufferSize, buffer, ref frameDesc);



                int outPutBufferSize = 0;
                ReturnCode rc = Api.FormatImage(buffer, ref frameDesc, ImageFormat.Bmp, null, ref outPutBufferSize);


                byte[] outputBuffer = new byte[outPutBufferSize];
                rc = Api.FormatImage(buffer, ref frameDesc, ImageFormat.Bmp, outputBuffer, ref outPutBufferSize);


                Bitmap bmp = new Bitmap(new MemoryStream(outputBuffer));

                Mat altered;


                if (bmp.PixelFormat == System.Drawing.Imaging.PixelFormat.Format24bppRgb)
                {
                    altered = new Mat(frameDesc.RoiHeight / frameDesc.PixelAddressingValueVertical, frameDesc.RoiWidth / frameDesc.PixelAddressingValueHorizontal, MatType.CV_8U);
                    Bitmap bmpGrey = MakeGrayscale(bmp);

                    byte[] grey = new byte[(frameDesc.RoiHeight / frameDesc.PixelAddressingValueVertical) * (frameDesc.RoiWidth / frameDesc.PixelAddressingValueHorizontal)];

                    for (int i = 0; i < grey.Length; i++)
                    {
                        grey[i] = 100;
                    }


                    var indexer = altered.GetGenericIndexer<byte>();

                    for (int y = 0; y < altered.Height; y++)
                    {
                        for (int x = 0; x < altered.Width; x++)
                        {
                            indexer[y, x] = bmpGrey.GetPixel(x, y).R;
                        }
                    }

                }
                else
                {
                    altered = BitmapConverter.ToMat(bmp);
                }

                alteredImages.Add(altered);

                //BitmapConverter.ToBitmap(altered).Save(focusValue + ".bmp");

                initialBitmaps.Add(bmp);

            }

            Api.SetStreamState(hCamera, StreamState.Stop);


            //Get Leplacian
            List<Bitmap> laplacianBitmaps = new List<Bitmap>();

            for (int i = 0; i < alteredImages.Count; i++)
            {
                //make greyscale
               // alteredImages[i] = alteredImages[i].CvtColor(ColorConversionCodes.BGR2GRAY);

                alteredImages[i] = alteredImages[i].Laplacian(MatType.CV_8U, ksize: 7);

                //blur image
                alteredImages[i] = alteredImages[i].MedianBlur(15);

                laplacianBitmaps.Add(BitmapConverter.ToBitmap(alteredImages[i]));
                //laplacianBitmaps[i].Save("laplacian" + i + ".bmp");
            }

            List<Bitmap> superPixeledPitmaps = new List<Bitmap>();


            List<Bitmap> gridBitmaps = new List<Bitmap>();

            //get grid
            for (int i = 0; i < laplacianBitmaps.Count; i++)
            {

                Mat imageToSuperPixel = BitmapConverter.ToMat(laplacianBitmaps[i]);

                Mat.Indexer<Vec3b> imageIndexer = imageToSuperPixel.GetGenericIndexer<Vec3b>();

                GridGroup[,] grid = new GridGroup[gridWidth, gridWidth];

                double widthDiv = imageToSuperPixel.Width / gridWidth;
                double heightDiv = imageToSuperPixel.Height / gridWidth;


                for (int y = 0; y < gridWidth; y++)
                {
                    for (int x = 0; x < gridWidth; x++)
                    {
                        grid[x, y] = new GridGroup();
                    }
                }



                for (int y = 0; y < imageToSuperPixel.Height; y++)
                {
                    for (int x = 0; x < imageToSuperPixel.Width; x++)
                    {
                        System.Drawing.Color color = laplacianBitmaps[i].GetPixel(x, y);

                        int xIndex = (int)Math.Floor(x / widthDiv);       
                        int yIndex = (int)Math.Floor(y / heightDiv);

                        if (xIndex>= gridWidth)
                        {
                            xIndex = gridWidth - 1;
                        }

                        if (yIndex >= gridWidth)
                        {
                            yIndex = gridWidth - 1;
                        }

                        grid[xIndex, yIndex].average = grid[xIndex, yIndex].average + color.R;
                        grid[xIndex, yIndex].count++;

                    }
                }

                gridBitmaps.Add(new Bitmap(gridWidth,gridWidth,System.Drawing.Imaging.PixelFormat.Format32bppArgb));

                for (int y = 0; y < gridWidth; y++)
                {
                    for (int x = 0; x < gridWidth; x++)
                    {
                        grid[x, y].average = grid[x, y].average / grid[x, y].count;
                        gridBitmaps[i].SetPixel(x, y, System.Drawing.Color.FromArgb(grid[x, y].average, grid[x, y].average, grid[x, y].average));
                    }
                }

            }

            Bitmap stackedHolder = new Bitmap(gridBitmaps[0].Width, gridBitmaps[0].Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            Bitmap stackedMap = new Bitmap(gridBitmaps[0].Width, gridBitmaps[0].Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
            Bitmap stackedTest = new Bitmap(gridBitmaps[0].Width, gridBitmaps[0].Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);



            for (int i = 0; i < gridBitmaps.Count; i++)
            {
                for (int x = 0; x < gridBitmaps[i].Width; x++)
                {
                    for (int y = 0; y < gridBitmaps[i].Height; y++)
                    {
                        if (gridBitmaps[i].GetPixel(x, y).R > stackedHolder.GetPixel(x, y).R)
                        {
                            stackedHolder.SetPixel(x, y, gridBitmaps[i].GetPixel(x, y));
                            stackedMap.SetPixel(x, y, System.Drawing.Color.FromArgb(i, i, i));
                        }
                    }
                }
            }



            Bitmap denoisedBitmap = new Bitmap(stackedMap.Width, stackedMap.Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            int numberOfPasses = 1;


            for (int k = 0; k < numberOfPasses; k++)
            {
 

                for (int x = 0; x < stackedMap.Width; x++)
                    {
                        for (int y = 0; y < stackedMap.Height; y++)
                        {

                            int[] counter = new int[gridBitmaps.Count];


                            if ((x-1)>=0 && (y-1) >= 0)
                            {
                            counter[(int)stackedMap.GetPixel(x - 1, y - 1).R]++;
                            }

                            if ((y - 1) >= 0)
                            {
                                counter[(int)stackedMap.GetPixel(x, y - 1).R]++;
                            }

                            if ( (x+1)< gridWidth && (y - 1) >= 0)
                            {
                                counter[(int)stackedMap.GetPixel(x+1, y - 1).R]++;
                            }

                            if ((x - 1) >= 0 )
                            {
                                counter[(int)stackedMap.GetPixel(x - 1, y).R]++;
                            }

                            counter[(int)stackedMap.GetPixel(x, y).R]++;

                            if ((x + 1) < gridWidth )
                            {
                                counter[(int)stackedMap.GetPixel(x + 1, y).R]++;
                            }

                            if ((x - 1) >= 0 && (y + 1) < gridWidth)
                            {
                                counter[(int)stackedMap.GetPixel(x - 1, y + 1).R]++;
                            }

                            if ( (y + 1) < gridWidth)
                            {
                                counter[(int)stackedMap.GetPixel(x , y + 1).R]++;
                            }

                            if ((x + 1) < gridWidth  && (y + 1) < gridWidth)
                            {
                                counter[(int)stackedMap.GetPixel(x + 1, y + 1).R]++;
                            }

                            int q = counter.ToList().IndexOf(counter.Max());

                            denoisedBitmap.SetPixel(x, y, System.Drawing.Color.FromArgb(q, q, q));
                            

                    }
                }

                stackedMap = denoisedBitmap;

            }


            Bitmap resized = new Bitmap(stackedMap, new System.Drawing.Size(initialBitmaps[0].Width, initialBitmaps[0].Height ));
            Bitmap resizedTest = new Bitmap(stackedMap, new System.Drawing.Size(initialBitmaps[0].Width, initialBitmaps[0].Height ));

 
            Bitmap finalImage = new Bitmap(initialBitmaps[0].Width,initialBitmaps[0].Height, System.Drawing.Imaging.PixelFormat.Format32bppArgb);

            for (int x = 0; x < resized.Width; x++)
            {
                for (int y = 0; y < resized.Height; y++)
                {
                    finalImage.SetPixel(x, y, initialBitmaps[0].GetPixel(x, y));

                    if (resized.GetPixel(x, y).R < initialBitmaps.Count)
                    {
                        finalImage.SetPixel(x, y, initialBitmaps[resized.GetPixel(x, y).R].GetPixel(x, y));
                    }
                }
            }



            finalImage.Save("finalImage.bmp");



            st.Stop();

            time.Content=  st.ElapsedMilliseconds/1000 + " s";

            Api.SetStreamState(hCamera, StreamState.Stop);
            Api.Uninitialize(hCamera);

        }

        public static Bitmap MakeGrayscale(Bitmap original)
        {
            //create a blank bitmap the same size as original
            Bitmap newBitmap = new Bitmap(original.Width, original.Height);

            //get a graphics object from the new image
            using (Graphics g = Graphics.FromImage(newBitmap))
            {

                //create the grayscale ColorMatrix
                System.Drawing.Imaging.ColorMatrix colorMatrix = new System.Drawing.Imaging.ColorMatrix(
                   new float[][]
                   {
             new float[] {.3f, .3f, .3f, 0, 0},
             new float[] {.59f, .59f, .59f, 0, 0},
             new float[] {.11f, .11f, .11f, 0, 0},
             new float[] {0, 0, 0, 1, 0},
             new float[] {0, 0, 0, 0, 1}
                   });

                //create some image attributes
                using (System.Drawing.Imaging.ImageAttributes attributes = new System.Drawing.Imaging.ImageAttributes())
                {

                    //set the color matrix attribute
                    attributes.SetColorMatrix(colorMatrix);

                    //draw the original image on the new image
                    //using the grayscale color matrix
                    g.DrawImage(original, new System.Drawing.Rectangle(0, 0, original.Width, original.Height),
                                0, 0, original.Width, original.Height, GraphicsUnit.Pixel, attributes);
                }
            }
            return newBitmap;
        }


    }

    public class LabelGroup
    {
        public double labelValue { get; set; }
        public int count { get; set; }
        public int average { get; set; }

        public LabelGroup()
        {

        }

    }


    public class GridGroup
    {
        public int count { get; set; }
        public int average { get; set; }

        public GridGroup()
        {

        }

    }

}

