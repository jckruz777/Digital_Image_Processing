/*********************************************************************
* Live Webcam Video features variation
*
* Digital Image Processing
* Student: Ing. Juan Carlos Cruz Naranjo
* Professor: Dr. Daniel Herrera Castro
*
* Based on:
* https://thefreecoder.wordpress.com/2012/09/11/opencv-c-video-capture/
* https://www.opencv-srf.com/2011/11/mouse-events.html
**********************************************************************/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

#define DEVVIDEO  1
#define MSKEYWAIT 5

using namespace std;
using namespace cv;

bool set_contrast = false;

//Initial values for image features
float contrast = 0.5;
float brightness = 0;
float saturation = 0.5;

/*
* Mouse click events
*/
void MouseFunc(int event, int x, int y, int flags, void* userdata)
{
  switch (event)
  {
    //Feature 1: Contrast
    case EVENT_LBUTTONDOWN:
      cout << "LEFT CLICK: Decrease Contrast" << endl;
      contrast = max(0.0f, contrast - 0.1f);
      set_contrast = true;
      break;
    case EVENT_RBUTTONDOWN:
      cout << "RIGHT CLICK: Increase Contrast" << endl;
      contrast = min(1.0f, contrast + 0.1f);
      set_contrast = true;
      break;
  }
}

/*
* Main Function
*/
int main() 
{
  // Device loaded at /dev/video[DEVVIDEO]
  VideoCapture stream(DEVVIDEO);              

  if (!stream.isOpened())
  {
    cout << "ERROR: Camera can't be opened";
    return 0;
  } 

  namedWindow("Video Display");
  setMouseCallback("Video Display", MouseFunc, NULL);

  // Set stream properties with the initial values
  stream.set(CAP_PROP_CONTRAST, contrast);
  stream.set(CV_CAP_PROP_BRIGHTNESS, brightness);
  stream.set(CV_CAP_PROP_SATURATION, saturation);
  
  bool cam_enable = true;
  while (cam_enable) 
  {
    Mat frame;
    stream.read(frame);
    imshow("Video Display", frame);
    char key = (char) waitKey(MSKEYWAIT);
    switch(key)
    { 
      //Feature 2: Brightness
      case 'w':
        cout << "UP: Increase Brightness" << endl;
        brightness = min(1.0f, brightness + 0.15f);
        stream.set(CV_CAP_PROP_BRIGHTNESS, brightness);
        break;
      case 'z':
        cout << "DOWN: Decrease Brightness" << endl;
        brightness = max(0.0f, brightness - 0.15f);
        stream.set(CV_CAP_PROP_BRIGHTNESS, brightness);
        break;
      //Feature 3: Saturation
      case 'd':
        cout << "RIGHT: Increase Saturation" << endl;
        saturation = min(1.0f,saturation + 0.15f);
        stream.set(CV_CAP_PROP_SATURATION, saturation);
        break;
      case 'a':
        cout << "LEFT: Decrease Saturation" << endl;
        saturation = max(0.0f,saturation - 0.15f);
        stream.set(CV_CAP_PROP_SATURATION, saturation);
        break;
      case 'q':
        cout << "Closing Window" << endl;
        cam_enable = false;
        break;
    }

    if (set_contrast)
    {
      stream.set(CAP_PROP_CONTRAST, contrast);
      set_contrast = false;
    }  
  }

  return 0;
}