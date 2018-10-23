/*
 * Copyright (C) 2009-2015 by Pablo Alvarado
 *
 * This file is part of the course MP-6123 Digital Image Processing,
 * at the Costa Rica Institute of Technology.
 *
 * The LTI-Lib-2 is free software; you can redistribute it and/or
 * modify it under the terms of the BSD License.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the authors nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file   tarea02.cpp
 *         Homework for first contact with the LTI-Lib-2
 * \author Pablo Alvarado
 * \date   26.05.2012
 * revisions ..: $Id: pwc.cpp,v 1.2 2009/06/05 12:59:33 alvarado Exp $
 */

// Standard Headers: from ANSI C and GNU C Library
#include <cstdlib>  // Standard Library for C++
#include <getopt.h> // Functions to parse the command line arguments

// Standard Headers: STL
#include <iostream>
#include <string>
#include <fstream>

// LTI-Lib Headers
#include <ltiObject.h>
#include <ltiMath.h>     // General lti:: math and <cmath> functionality
#include <ltiTimer.h>    // To measure time

#include <ltiV4l2.h>
#include <ltiIOImage.h> // To read/write images from files (jpg, png, and bmp)
#include <ltiViewer2D.h>

#include <ltiLispStreamHandler.h>

// Ensure that the STL streaming is used.
using std::cout;
using std::cerr;
using std::endl;

/*
 * Help
 */
void usage()  {


  cout <<
    "usage: ./Tarea_03 [options] [<file>]\n\n" \
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
void drawBresenhamLine(lti::image& img, int pixColorID, int x0, int y0, int x1, int y1)
{
    int dx, dy, sx, sy, err, e2, currentX, currentY;
    lti::rgbaPixel px;

    //Create pixels according to the specified color ID
    switch(pixColorID)
    {
      case 0: //Black
        px = lti::rgbaPixel(0,0,0);
        break;
      case 1: //White
        px = lti::rgbaPixel(255,255,255);
        break;
      case 2: //Red
        px = lti::rgbaPixel(255,0,0);
        break;
      case 3: //Green
        px = lti::rgbaPixel(0,255,0);
        break;
      case 4: //Blue
        px = lti::rgbaPixel(0,0,255);
        break;
      case 5: //Yellow
        px = lti::rgbaPixel(255,255,0);
        break;
      case 6: //Orange
        px = lti::rgbaPixel(255,165,0);
        break;
      case 7: //Cyan
        px = lti::rgbaPixel(0,255,255);
        break;
      case 8: //Magenta
        px = lti::rgbaPixel(255,0,255);
        break;
      default: //Black (default)
        px = lti::rgbaPixel(0,0,0);
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
    
    while(true) {
      img.at(currentY,currentX) = px; //Replacing pixel
      
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
 * Main method
 */
int main(int argc, char* argv[]) 
{
  // check if the user gave the required arguments
  std::string filename;
  int lineColorID = 0, x0, y0, x1, y1;
  int imgWidth, imgHeight;
  std::string colorNames[9] = {"Black", "White", "Red", "Green", "Blue", "Yellow", "Orange", "Cyan", "Magenta"};

  parse(argc,argv,filename);

  if (!filename.empty()) 
  {
    // we need an object to load images
    lti::ioImage loader;

    // we also need image objects
    lti::image img;
    lti::image orig;

    // load the image
    if (loader.load(filename,img) && loader.load(filename,orig)) 
    {
      // image successfully loaded

      // we need a viewer
      static lti::viewer2D view;
      //lti::viewer2D::interaction action;

      // and get its parameters
      lti::viewer2D::parameters vpar(view.getParameters());
      vpar.title = filename; // set the image name at the window's title bar
      view.setParameters(vpar);

      /**/
      // Loaded image info
      imgWidth = img.columns();
      imgHeight = img.rows();

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

      cout << endl << "Point Summary:" << endl;
      printSummary(x0, y0, x1, y1);

      // draw the line on the image
      drawBresenhamLine(img, lineColorID, x0, y0, x1, y1);

      view.show(img); // show the image

      // wait for the user to close the window or to indicate
      bool theEnd = false;
      lti::viewer2D::interaction action;
      lti::ipoint pos;

      do {
        view.waitInteraction(action,pos); // wait for something to happen
        if (action.action == lti::viewer2D::KeyPressed)
        {
          switch(action.key)
          {
            case lti::viewer2D::UpKey:
              y1--;
              cout << "UP: End point Pix_PosY - 1" << endl;
              isOnImage(imgWidth, imgHeight, x1, y1);
              printSummary(x0, y0, x1, y1);
              break;
            case lti::viewer2D::DownKey:
              y1++;
              cout << "DOWN: End point Pix_PosY + 1" << endl;
              isOnImage(imgWidth, imgHeight, x1, y1);
              printSummary(x0, y0, x1, y1);
              break;
            case lti::viewer2D::LeftKey:
              x1--;
              cout << "LEFT: End point Pix_PosX - 1" << endl;
              isOnImage(imgWidth, imgHeight, x1, y1);
              printSummary(x0, y0, x1, y1);
              break;
            case lti::viewer2D::RightKey:
              x1++;
              cout << "RIGHT: End point Pix_PosX + 1" << endl;
              isOnImage(imgWidth, imgHeight, x1, y1);
              printSummary(x0, y0, x1, y1);
              break;
            case 'w':
            case 'W':
              y0--;
              cout << "UP (W): Starting point Pix_PosY - 1" << endl;
              isOnImage(imgWidth, imgHeight, x0, y0);
              printSummary(x0, y0, x1, y1);
              break;
            case 'z':
            case 'Z':
              y0++;
              cout << "DOWN (Z): Starting point Pix_PosY + 1" << endl;
              isOnImage(imgWidth, imgHeight, x0, y0);
              printSummary(x0, y0, x1, y1);
              break;
            case 'a':
            case 'A':
              x0--;
              cout << "LEFT (A): Starting point Pix_PosX - 1" << endl;
              isOnImage(imgWidth, imgHeight, x0, y0);
              printSummary(x0, y0, x1, y1);
              break;
            case 'd':
            case 'D':
              x0++;
              cout << "RIGHT (D): Starting point Pix_PosX + 1" << endl;
              isOnImage(imgWidth, imgHeight, x0, y0);
              printSummary(x0, y0, x1, y1);
              break;
            case int('x'): //x
            case int('X'): //X
              cout << "Finishing program..." << endl;
              theEnd = true;
              break;
          }

          // draw the line on the image
          drawBresenhamLine(img, lineColorID, x0, y0, x1, y1);

          view.show(img); // show the image

          img = orig;
        }
        if (action == lti::viewer2D::Closed) // window closed?
          theEnd = true; // we are ready here!
      } while(!theEnd);

      return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;

  }
  cout << "ERROR: You need to specify an image name as an input argument." << endl;
  return EXIT_FAILURE;
}
