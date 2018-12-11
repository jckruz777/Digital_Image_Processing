/*************************************************************************************************************
* Project: Optimization of DIP Operators with SIMD Instructions
*
* Digital Image Processing
* Student: Ing. Juan Carlos Cruz Naranjo
* Professor: Dr. Daniel Herrera Castro
*
* OpenCV Implementation: Basic Reference Filters (No SIMD)
*
* Based on:
* 
**************************************************************************************************************/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>

//#define DISPLAY 1          // Show images if un-commented
#define NUM_POINTS  2      // Num of time samples
#define NUM_TIME_IT 4       // Num of measurements before compute the mean time
#define MIN_KERNEL_SIZE 5   // Min Kernel size
#define NUM_ALGORITHMS 2    // 2 Algorithms: Min and Max Filter

using namespace std;
using namespace cv;

//Global Variables
string filename = "data.dat";
string titles = "se_size\tmin\tmax";
vector< vector<double> > finalTimes(NUM_POINTS, vector<double>(2)); // Vector for timing results

// Create a file with the results for GNU-Plot
void createData()
{
  string titles_wr = "echo \"" + titles + "\" > " + filename;
  string times_wr = "echo \"";
  system(titles_wr.c_str());
  string tmp_wr = "";
  string ksize = "";
  string tmp_time = "";
  for(int i = 1; i < NUM_POINTS; i++)
  {
    ksize = std::to_string(i * MIN_KERNEL_SIZE);
    tmp_wr = ksize + "\t";
    for(int j = 0; j < NUM_ALGORITHMS; j++)
    {
      tmp_time = std::to_string(finalTimes[i][j] * 1000.0);
      (j == (NUM_ALGORITHMS - 1)) ? tmp_wr += tmp_time : tmp_wr += (tmp_time + "\t");
    }
    system((times_wr + tmp_wr + "\" >> " + filename).c_str());
    tmp_wr = "";
  }
}


double getVariance(vector<double> samples, double avg)
{
	double result = 0.0;
	for(int i = 0; i < NUM_TIME_IT; i++)
		result += pow((avg - samples[i]), 2) / NUM_TIME_IT;
	return result;
}


/*
* Main Function
*/
int main(int argc, char** argv) 
{
  cout << "Starting the Program..." << endl;
  
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
  cv::resize(src, display_srcimage, cv::Size(), 1, 1);
  imshow( "Display image", display_srcimage );   
  waitKey(0);
  #endif

  for(int i = 1; i < NUM_POINTS; i++)
  {
    cv::Mat se_kernel = getStructuringElement(cv::MORPH_RECT, Size(i * MIN_KERNEL_SIZE,i * MIN_KERNEL_SIZE), Point(0, 0));   // Structuring element (kernel)

    cv::Mat minImage;
    std::chrono::duration<double> diffA;
    double avgA = 0;
    vector<double> samplesA(NUM_TIME_IT);
    for(int j = 0; j < NUM_TIME_IT; j++)
    {
	  system("./clearCache.sh");
      auto startA = std::chrono::high_resolution_clock::now();
      cv::erode(src, minImage, se_kernel);
      auto endA = std::chrono::high_resolution_clock::now();
      diffA = endA - startA;
      samplesA[j] = diffA.count();
      avgA += (1.0 / NUM_TIME_IT) * diffA.count();
    }
    finalTimes[i][0] = avgA;
    cout << "Min Filter Variance = " << getVariance(samplesA, avgA) << endl << endl;

    #ifdef DISPLAY
    namedWindow( "Display image", WINDOW_AUTOSIZE );                          // Create a window for display.
    imshow( "Display image", minImage );   
    waitKey(0);
    #endif

    cv::Mat maxImage;
    std::chrono::duration<double> diffB;
    double avgB = 0;
    vector<double> samplesB(NUM_TIME_IT);
    for(int j = 0; j < NUM_TIME_IT; j++)
    {
	  system("./clearCache.sh");
      auto startB = std::chrono::high_resolution_clock::now();
      cv::dilate(src, maxImage, se_kernel);
      auto endB = std::chrono::high_resolution_clock::now();
      diffB = endB - startB;
      samplesB[j] = diffB.count();
      avgB += (1.0 / NUM_TIME_IT) * diffB.count();
    }
    finalTimes[i][1] = avgB;
    cout << "Max Filter Variance = " << getVariance(samplesB, avgB) << endl << endl;

    #ifdef DISPLAY
    namedWindow( "Display image", WINDOW_AUTOSIZE );                          // Create a window for display.
    imshow( "Display image", maxImage );   
    waitKey(0);
    #endif
  }

  //Generating Timing Results
  cout << "Generating the Timing Data..." << endl;;
  createData();
  //system("gnuplot -e \"load 'results.plt';pause -1\"");

  return 0;
}
