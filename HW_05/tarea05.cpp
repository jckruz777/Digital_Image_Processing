/*************************************************************************************************************
* Frequency Domain Processing
*
* Digital Image Processing
* Student: Ing. Juan Carlos Cruz Naranjo
* Professor: Dr. Daniel Herrera Castro
*
* Based on:
* https://github.com/jrdi/opencv-examples/blob/master/gradient/main.cpp
* https://docs.opencv.org/2.4/doc/tutorials/core/discrete_fourier_transform/discrete_fourier_transform.html
* https://vgg.fiit.stuba.sk/2012-05/frequency-domain-filtration/
**************************************************************************************************************/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

//#define DISPLAY 1       // Show images if un-commented
#define NUM_POINTS  20 // Num of points for the final graphic
#define NUM_TIME_IT 4  // Num of measurements before compute the mean time

using namespace std;
using namespace cv;

//Global Variables
string filename = "data.dat";
string titles = "ksize\ttarr0\ttarr1\ttarr2";                       // Titles on the "data.dat" file (gnuplot)l
vector< vector<double> > finalTimes(NUM_POINTS, vector<double>(3)); // Vector for timing results:
//Col0: Space Domain, 2D Kernel
//Col1: Space Domain, Sep Kernel
//Col2: Frequency Domain, 2D Kernel

// Comparison Metric PSNR
double getPSNR(const Mat& I1, const Mat& I2)
{
    Mat s1;
    absdiff(I1, I2, s1);       // |I1 - I2|
    s1.convertTo(s1, CV_32F);  // cannot make a square on 8 bits
    s1 = s1.mul(s1);           // |I1 - I2|^2

    Scalar s = sum(s1);        // sum elements per channel

    double sse = s.val[0] + s.val[1] + s.val[2]; // sum channels

    if( sse <= 1e-10) // for small values return zero
        return 0;
    else
    {
        double mse  = sse / (double)(I1.channels() * I1.total());
        double psnr = 10.0 * log10((255 * 255) / mse);
        return psnr;
    }
}

// Quadrants Re-ordering
Mat shift(Mat magI) 
{
    Mat result;
    magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2)); // Crop for odd number of rows or columns
 
    int cx = magI.cols/2;
    int cy = magI.rows/2;
 
    Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
    Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
    Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
    Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right
 
    Mat tmp;                            // swap quadrants (Top-Left with Bottom-Right)
    q0.copyTo(tmp);
    q3.copyTo(q0);
    tmp.copyTo(q3);
    q1.copyTo(tmp);                     // swap quadrant (Top-Right with Bottom-Left)
    q2.copyTo(q1);
    tmp.copyTo(q2);
 
    return magI;
}


// Fourier Transform
Mat computeDFT(Mat image) 
{
    Mat padded;
    int m = getOptimalDFTSize(image.rows);
    int n = getOptimalDFTSize(image.cols);
    // create output image of optimal size
    copyMakeBorder(image, padded, 0, m - image.rows, 0, n - image.cols, BORDER_CONSTANT, Scalar::all(0));
    // copy the source image, on the border add zero values
    Mat planes[] = { Mat_< float> (padded), Mat::zeros(padded.size(), CV_32F) };
    // create a complex matrix
    Mat complex;
    merge(planes, 2, complex);
    dft(complex, complex, DFT_COMPLEX_OUTPUT);  // fourier transform
    return complex;
}

// Inverse Fourier Transform
Mat computeIDFT(Mat complex)
{
    Mat result;
    idft(complex, result);
    // equivalent to:
    // dft(complex, result, DFT_INVERSE + DFT_SCALE);
    Mat planes[] = {
        Mat::zeros(complex.size(), CV_32F),
        Mat::zeros(complex.size(), CV_32F)
    };
    split(result, planes); // planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))
    magnitude(planes[0], planes[1], result); // sqrt(Re(DFT(I))^2 + Im(DFT(I))^2)
    normalize(result, result, 0, 1, NORM_MINMAX);
    return result;
}

// Plot Magnitude - Frquency Domain
void displayMag(Mat complex) 
{
  Mat magI, display_magnitude;
  Mat planes[] = {
      Mat::zeros(complex.size(), CV_32F),
      Mat::zeros(complex.size(), CV_32F)
  };
  split(complex, planes); // planes[0] = Re(DFT(I)), planes[1] = Im(DFT(I))
  magnitude(planes[0], planes[1], magI); // sqrt(Re(DFT(I))^2 + Im(DFT(I))^2)
  // switch to logarithmic scale: log(1 + magnitude)
  magI += Scalar::all(1);
  log(magI, magI);
  magI = shift(magI); // rearrage quadrants
  // Transform the magnitude matrix into a viewable image (float values 0-1)
  normalize(magI, magI, 1, 0, NORM_INF);

  namedWindow( "Spectrum Magnitude", WINDOW_AUTOSIZE ); 
  cv::resize(magI, display_magnitude, cv::Size(), 0.25, 0.25);
  imshow( "Spectrum Magnitude", display_magnitude );

  waitKey(0);
}

// Create a file with the results for GNU-Plot
void createData()
{
  string titles_wr = "echo \"" + titles + "\" > " + filename;
  string times_wr = "echo \"";
  system(titles_wr.c_str());
  string tmp_wr = "";
  string ksize = "";
  string tmp_time = "";
  for(int i = 0; i < NUM_POINTS; i++)
  {
    ksize = std::to_string((10 * i) + 9);
    tmp_wr = ksize + "\t";
    for(int j = 0; j < NUM_TIME_IT; j++)
    {
      tmp_time = std::to_string(finalTimes[i][j]);
      (j == (NUM_TIME_IT - 1)) ? tmp_wr += tmp_time : tmp_wr += (tmp_time + "\t");
    }
    system((times_wr + tmp_wr + + "\" >> " + filename).c_str());
    tmp_wr = "";
  }
}


// Padding for a Kernel by considering optimal dimensions
Mat padded2DKernel(Mat Kernel, int maxHeight, int maxWidth)
{
  Mat result(cv::Size(maxWidth, maxHeight), CV_32F);
  result = 0;
  int krows = Kernel.rows;
  int kcols = Kernel.cols;
  bool inRangeH = false;
  bool inRangeW = false;
  int origInImagex = ((maxHeight / 2) - (krows / 2));
  int origInImagey = ((maxWidth / 2) - (kcols / 2));
  int k = 0, l = 0;
  for(int i = 0; i < maxHeight; i++)
  { 
    for(int j = 0; j < maxWidth; j++)
    {
      inRangeH = (i >= origInImagey) && (i < (origInImagey + krows));
      inRangeW = (j >= origInImagex) && (j < (origInImagex + kcols));
      if(inRangeH && inRangeW)
      {
        result.at<float>(Point(i, j)) = Kernel.at<float>(Point(k, l));
        l++;
        if(l == kcols)
        {
          l = 0;
          k++;
        }
      }
    }
  }
  return result;
}


/*
* Main Function
*/
int main(int argc, char** argv) 
{

  cout << "Starting the Program..." << endl;
  cout << "This could take a few minutes." << endl;
  cout << "Please be patient." << endl << endl;

  Mat src;
  Mat display_srcimage;
  src = imread(argv[1], CV_LOAD_IMAGE_GRAYSCALE);                           // Read the file

  if(! src.data )                                                           // Check for invalid input
  {
      cout <<  "Could not open or find the image" << std::endl ;
      return -1;
  }

  #ifdef DISPLAY
  //Display input image resized:
  namedWindow( "Display image", WINDOW_AUTOSIZE );                          // Create a window for display.
  cv::resize(src, display_srcimage, cv::Size(), 0.25, 0.25);
  imshow( "Display image", display_srcimage );   
  waitKey(0);
  #endif

  Mat complexI = computeDFT(src);                                           // Input image in frequency domain

  #ifdef DISPLAY
  displayMag(complexI);                                                   // Plotting the Magnitude
  #endif

  int filterSize = 0;
  float sigma = 0;

  /**************
  * SPACE DOMAIN
  ***************/
  Mat filter2D_result;
  Mat sepfilter2D_result;
  Mat display_2Dresult;
  Mat display_sep2Dresult;         

  for(int i = 0; i < NUM_POINTS; i++)
  {
    filterSize = (10 * i) + 9;                                                //From 9 to [10*(NUM_POINTS-1) + 9]
    sigma = ((filterSize + 2) / 6);
    Mat gaussian_filter = cv::getGaussianKernel(filterSize, sigma, CV_32F);   // Gaussian Kernel (Filter)

    // Unified Kernel
    Mat unifiedKernel = gaussian_filter * gaussian_filter.t();

    std::chrono::duration<double> diffA;
    double avgA = 0;
    for(int j = 0; j < NUM_TIME_IT; j++)
    {
      auto startA = std::chrono::high_resolution_clock::now();
      filter2D(src, filter2D_result, -1 , unifiedKernel, Point(-1, -1), 0, BORDER_DEFAULT );
      auto endA = std::chrono::high_resolution_clock::now();
      diffA = endA - startA;
      avgA += (1.0 / NUM_TIME_IT) * diffA.count();
    }
    finalTimes[i][0] = avgA;

    #ifdef DISPLAY
    namedWindow( "Display result 2D - Space Domain: Unified Kernel", WINDOW_AUTOSIZE ); 
    cv::resize(filter2D_result, display_2Dresult, cv::Size(), 0.25, 0.25);
    imshow( "Display result 2D - Space Domain: Unified Kernel", display_2Dresult ); 
    waitKey(0);
    #endif


    // Separated Kernel
    std::chrono::duration<double> diffB;
    double avgB = 0;
    for(int j = 0; j < NUM_TIME_IT; j++)
    {
      auto startB = std::chrono::high_resolution_clock::now();
      sepFilter2D(src, sepfilter2D_result, -1, gaussian_filter, gaussian_filter.t(), Point(-1, -1), 0, BORDER_DEFAULT );
      auto endB = std::chrono::high_resolution_clock::now();
      diffB = endB - startB;
      avgB += (1.0 / NUM_TIME_IT) * diffB.count();
    }
    finalTimes[i][1] = avgB;

    #ifdef DISPLAY
    namedWindow( "Display result 2D - Space Domain: Sep Kernel", WINDOW_AUTOSIZE ); 
    cv::resize(sepfilter2D_result, display_sep2Dresult, cv::Size(), 0.25, 0.25);
    imshow( "Display result 2D - Space Domain: Sep Kernel", display_sep2Dresult );  
    waitKey(0);
    #endif

    /*****************
    * FREQUENCY DOMAIN
    ******************/
    Mat padded;                            //expand input image to optimal size
    Mat filter2D_Fresult;
    Mat display_2DFresult;
    Mat padded_kernel;

    padded_kernel = padded2DKernel(unifiedKernel, complexI.rows, complexI.cols);
    /*namedWindow( "Display result 2Dk", WINDOW_AUTOSIZE ); 
    cv::resize(padded_kernel, padded_kernel, cv::Size(), 0.25, 0.25);
    imshow( "Display result 2Dk", padded_kernel );
    waitKey(0);*/
    Mat complexKernel = computeDFT(padded_kernel);                            // Gaussian Kernel in Frequency

    // Unified Kernel
    std::chrono::duration<double> diffC;
    double avgC = 0;
    for(int j = 0; j < NUM_TIME_IT; j++)
    {
      auto startC = std::chrono::high_resolution_clock::now();
      mulSpectrums(complexI, complexKernel, filter2D_Fresult, 0);      
      filter2D_Fresult = computeIDFT(filter2D_Fresult);

      auto endC = std::chrono::high_resolution_clock::now();
      diffC = endC - startC;
      avgC += (1.0 / NUM_TIME_IT) * diffC.count();
    }
    finalTimes[i][2] = avgC;

    filter2D_Fresult = shift(filter2D_Fresult);

    #ifdef DISPLAY
    namedWindow( "Display result 2D - Frequency Domain", WINDOW_AUTOSIZE ); 
    cv::resize(filter2D_Fresult, display_2DFresult, cv::Size(), 0.25, 0.25);
    imshow( "Display result 2D - Frequency Domain", display_2DFresult );
    waitKey(0);
    #endif

  }

  //Plotting Timing Results: GNU-Plot
  cout << "Generating the Timing Plot..." << endl;
  cout << "INFO: Press Ctrl+C to finish the program." << endl;
  createData();
  system("gnuplot -e \"load 'results.plt';pause -1\"");

  return 0;
}
