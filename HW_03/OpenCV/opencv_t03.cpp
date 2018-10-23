/*********************************************************************
* Drawing Lines with Bresenham's Algorithm
*
* Digital Image Processing
* Student: Ing. Juan Carlos Cruz Naranjo
* Professor: Dr. Daniel Herrera Castro
*
* Based on: https://github.com/fragkakis/bresenham/blob/master/src/main/java/org/fragkakis/Bresenham.java
**********************************************************************/

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <getopt.h>

#define MSKEYWAIT 5

using namespace std;
using namespace cv;

/*
 * Help
 */
void usage()  {


  cout <<
    "usage: ./opencv_t03 [options] [<file>]\n\n" \
    "       -h|--help        show this help\n"\
    "       <file>           input image" << std::endl;
}

/*
 * Parse the line command arguments
 *
 * Set the filename with the given parameter, or an empty string otherwise
 */
void parse(int argc, char*argv[],std::string& filename) {

  int c;

  // We use the standard getopt.h functions here to parse the arguments.
  // Check the documentation of getopt.h for more information on this

  // structure for the long options.
  static struct option lopts[] = {
    {"help",no_argument,0,'h'},
    {0,0,0,0}
  };

  int optionIdx;

  while ((c = getopt_long(argc, argv, "h", lopts,&optionIdx)) != -1) {
    switch (c) {
    case 'h':
      usage();
      exit(EXIT_SUCCESS);
    default:
      cerr << "Option '-" << static_cast<char>(c) << "' not recognized."
           << endl;
    }
  }

  // Now try to read the image name
  if (optind < argc) {           // if there are still some arguments left...
    filename = argv[optind];  // with the given image file
  } else {
    filename = "";
  }
}

/*
* Validation of the input co-ordinates
*/ 
void isOnImage(int width, int height, int &x, int &y)
{
  bool negX = (x < 0);
  bool negY = (y < 0);
  bool overWidth = (x >= width);
  bool overHeight = (y >= height);
  bool outImage = (negX || negY || overWidth || overHeight);
  if(outImage)
  {
    cout << "WARNING: That point is out of the image." << endl;
    if(negX) // Case: x < 0;
    {
      x = 0;
      cout << "The X value has been set to 0." << endl;
    }
    if(negY) // Case: y < 0;
    {
      y = 0;
      cout << "The Y value has been set to 0." << endl;
    }
    if(overWidth) // Case: x >= width;
    {
      x = (width - 1);
      cout << "The X value has been set to " << (width - 1) << "." << endl;
    }
    if(overHeight) // Case: y >= height;
    {
      y = (height - 1);
      cout << "The Y value has been set to " << (height - 1) << "." << endl;
    }
    else
      cout << "Valid point." << endl;
  }
}

/*
* Draws the line by using the Bresenham Algorithm 
*/
void drawBresenhamLine(Mat& img, int pixColorID, int x0, int y0, int x1, int y1)
{
    int dx, dy, sx, sy, err, e2, currentX, currentY;
    Vec3b px;

    //Create pixels according to the specified color ID
    switch(pixColorID)
    {
      case 0: //Black
        px = Vec3b(0,0,0);
        break;
      case 1: //White
        px = Vec3b(255,255,255);
        break;
      case 2: //Red
        px = Vec3b(255,0,0);
        break;
      case 3: //Green
        px = Vec3b(0,255,0);
        break;
      case 4: //Blue
        px = Vec3b(0,0,255);
        break;
      case 5: //Yellow
        px = Vec3b(255,255,0);
        break;
      case 6: //Orange
        px = Vec3b(255,165,0);
        break;
      case 7: //Cyan
        px = Vec3b(0,255,255);
        break;
      case 8: //Magenta
        px = Vec3b(255,0,255);
        break;
      default: //Black (default)
        px = Vec3b(0,0,0);
        break;
    }
 
    //Bresenham Algorithm
    dx = abs(x1 - x0);
    dy = abs(y1 - y0);
    
    sx = x0 < x1 ? 1 : -1; 
    sy = y0 < y1 ? 1 : -1; 
    
    err = dx-dy;
    currentX = x0;
    currentY = y0;
    
    while(true) 
    {
      img.at<Vec3b>(currentY, currentX) = px; //Replacing pixel
      
      if(currentX == x1 && currentY == y1) {
        break;
      }
      
      e2 = 2*err;
      if(e2 > -1 * dy) {
        err = err - dy;
        currentX = currentX + sx;
      }
      
      if(e2 < dx) {
        err = err + dx;
        currentY = currentY + sy;
      }
    }
}


/*
* Returns the distance between two points
*/
float getDistance(int x0, int y0, int x1, int y1)
{
  int dx = x1 - x0;
  int dy = y1 - y0;
  int dx_2 = dx * dx;
  int dy_2 = dy * dy;
  float distance = sqrt(dx_2 + dy_2);
  return distance;
}

/*
* Prints a summary about a line features
*/
void printSummary(int x0, int y0, int x1, int y1)
{
  cout << "(x0,y0)=(" << x0 << "," << y0 << ")" << endl;
  cout << "(x1,y1)=(" << x1 << "," << y1 << ")" << endl;
  cout << "Distance: " << getDistance(x0, y0, x1, y1) << endl << endl;
}

/*
* Main Function
*/
int main( int argc, char** argv )
{
  // check if the user gave the required arguments
  std::string filename;
  int lineColorID = 0, x0, y0, x1, y1;
  int imgWidth, imgHeight;
  std::string colorNames[9] = {"Black", "White", "Red", "Green", "Blue", "Yellow", "Orange", "Cyan", "Magenta"};

  parse(argc,argv,filename);

  if (!filename.empty()) 
  {
    
    Mat img;
    Mat orig;
    img = imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file
    orig = imread(filename, CV_LOAD_IMAGE_COLOR);   // Read the file

    if (img.data && orig.data) 
    {
      // image successfully loaded

      // we need a viewer
      namedWindow( "Loaded Image", WINDOW_AUTOSIZE );      // Create a window for display.

      /**/
      // Loaded image info
      imgWidth = img.size().width;
      imgHeight = img.size().height;

      //Color selection menu
      cout << "Enter the color ID for the line (between 0 and 8). " << endl;
      cout << "Otherwise the default color will be Black. " << endl;
      cout << "Choose one of the following color options: " << endl;
      cout << "======================" << endl;
      cout << "| ID |     Color     |" << endl;
      cout << "======================" << endl;
      cout << "| 0  |     Black     |" << endl;
      cout << "----------------------" << endl;
      cout << "| 1  |     White     |" << endl;
      cout << "----------------------" << endl;
      cout << "| 2  |     Red       |" << endl;
      cout << "----------------------" << endl;
      cout << "| 3  |     Green     |" << endl;
      cout << "----------------------" << endl;
      cout << "| 4  |     Blue      |" << endl;
      cout << "----------------------" << endl;
      cout << "| 5  |     Yellow    |" << endl;
      cout << "----------------------" << endl;
      cout << "| 6  |     Orange    |" << endl;
      cout << "----------------------" << endl;
      cout << "| 7  |     Cyan      |" << endl;
      cout << "----------------------" << endl;
      cout << "| 8  |     Magenta   |" << endl;
      cout << "======================" << endl << endl;
      cout << "Enter the color ID: ";
      std::cin >> lineColorID;
      if(lineColorID > 8) lineColorID = 0; //Black (default)
      std::cout << "Selected color: " << colorNames[lineColorID] << endl << endl;

      cout << "Enter the co-ordinates of the first point..." << endl;
      cout << "x0: ";
      std::cin >> x0; 
      cout << "y0: ";
      std::cin >> y0;
      cout << endl;

      isOnImage(imgWidth, imgHeight, x0, y0);
   
      cout<<"Enter co-ordinates of the second point..." << endl;
      cout << "x1: ";
      std::cin >> x1; 
      cout << "y1: ";
      std::cin >> y1;
      cout << endl;

      isOnImage(imgWidth, imgHeight, x0, y0);

      cout << endl << "Line Summary:" << endl;
      printSummary(x0, y0, x1, y1);

      // draw the line on the image
      drawBresenhamLine(img, lineColorID, x0, y0, x1, y1);

      imshow( "Loaded Image", img );     // show the image

      // wait for the user to close the window or to indicate
      bool theEnd = false;

      do {
        char key = (char) waitKey(MSKEYWAIT); // wait for something to happen
        //cout << "key: " << key << endl;
        switch(key)
        {
          case (char)1113938:
            y1--;
            cout << "UP: End point Pix_PosY - 1" << endl;
            isOnImage(imgWidth, imgHeight, x1, y1);
            printSummary(x0, y0, x1, y1);
            break;
          case (char)1113940:
            y1++;
            cout << "DOWN: End point Pix_PosY + 1" << endl;
            isOnImage(imgWidth, imgHeight, x1, y1);
            printSummary(x0, y0, x1, y1);
            break;
          case (char)1113937:
            x1--;
            cout << "LEFT: End point Pix_PosX - 1" << endl;
            isOnImage(imgWidth, imgHeight, x1, y1);
            printSummary(x0, y0, x1, y1);
            break;
          case (char)1113939:
            x1++;
            cout << "RIGHT: End point Pix_PosX + 1" << endl;
            isOnImage(imgWidth, imgHeight, x1, y1);
            printSummary(x0, y0, x1, y1);
            break;
          case 'w': //w
          case 'W':  //W
            y0--;
            cout << "UP (W): Starting point Pix_PosY - 1" << endl;
            isOnImage(imgWidth, imgHeight, x0, y0);
            printSummary(x0, y0, x1, y1);
            break;
          case 'z': //z
          case 'Z':  //Z
            y0++;
            cout << "DOWN (Z): Starting point Pix_PosY + 1" << endl;
            isOnImage(imgWidth, imgHeight, x0, y0);
            printSummary(x0, y0, x1, y1);
            break;
          case 'a': //a
          case 'A': //A
            x0--;
            cout << "LEFT (A): Starting point Pix_PosX - 1" << endl;
            isOnImage(imgWidth, imgHeight, x0, y0);
            printSummary(x0, y0, x1, y1);
            break;
          case 'd': //d
          case 'D': //D
            x0++;
            cout << "RIGHT (D): Starting point Pix_PosX + 1" << endl;
            isOnImage(imgWidth, imgHeight, x0, y0);
            printSummary(x0, y0, x1, y1);
            break;
          case 'x': //x
          case 'X': //X
            cout << "Finishing program..." << endl;
            theEnd = true;
            break;
        }

          // draw the line on the image
          drawBresenhamLine(img, lineColorID, x0, y0, x1, y1);

          imshow( "Loaded Image", img);     // show the image

          img = orig.clone();
      } while(!theEnd);

      return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;

  }
  cout << "ERROR: You need to specify an image name as an input argument." << endl;
  return EXIT_FAILURE;
}