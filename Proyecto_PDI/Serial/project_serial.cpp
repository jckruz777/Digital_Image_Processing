/*
 * Copyright (C) 2007 by Pablo Alvarado
 * 
 * This file is part of the LTI-Computer Vision Library 2 (LTI-Lib-2)
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
 * \file   surfLocalDescriptor.cpp
 *         Contains an example of use for the class lti::surfLocalDescriptor
 * \author Pablo Alvarado
 * \date   04.11.2007
 * revisions ..: $Id: matrixTransform.cpp,v 1.5 2011-08-29 14:17:33 alvarado Exp $
 */

// LTI-Lib Headers
#include "ltiObject.h"
#include "ltiIOImage.h"
#include "ltiMath.h"
#include "ltiPassiveWait.h"

#include "ltiLispStreamHandler.h"

#include "ltiViewer2D.h" // The normal viewer
typedef lti::viewer2D viewer_type;

// Standard Headers
#include <cstdlib>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;

//#define DISPLAY 1          // Show images if un-commented
#define NUM_POINTS  2       // Num of time samples 2 -> 5x5
#define NUM_TIME_IT 4       // Num of measurements before compute the mean time
#define MIN_KERNEL_SIZE 5   // Min Kernel size
#define NUM_ALGORITHMS 2    // 2 Algorithms: Min and Max Filter

using namespace std;


//Global Variables
string filename = "data.dat";
string titles = "se_size\tmin\tmax";
vector< vector<double> > finalTimes(NUM_POINTS, vector<double>(2)); // Vector for timing results

/*
 * Help 
 */
void usage() {
  cout << "Usage: matrixTransform [image] [-h]" << endl;
  cout << "  -h show this help." << endl;
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


/*
 * Parse the line command arguments
 */
void parseArgs(int argc, char*argv[], 
               std::string& filename) {
  
  filename.clear();
  // check each argument of the command line
  for (int i=1; i<argc; i++) {
    if (*argv[i] == '-') {
      switch (argv[i][1]) {
        case 'h':
          usage();
          exit(EXIT_SUCCESS);
          break;
        default:
          break;
      }
    } else {
      filename = argv[i]; // guess that this is the filename
    }
  }
}


void maxFilterTrivial(const lti::channel8 &src, lti::channel8 &dst, const int se_size)
{
    int width = src.columns();
    int height = src.rows();
    int se_mid = (se_size - 1) / 2;
    int limAi, limBi;
    int limAf, limBf;
    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            uint8_t max = src[j][i];
            limAi = i - se_mid;
            limAf = i + se_mid;
            for(int a = limAi; a < limAf; a++)
            {
                limBi = j - se_mid;
                limBf = j + se_mid;
                for(int32_t b = limBi; b < limBf; b++)
                {
                    uint8_t value = max;
                    if( (a >= 0) && (a < width) && (b >= 0) && (b < height) )
                        value = src[b][a];
                    if(value > max)
                        max = value;
                }
            }
            dst[j][i] = max;
        }
    }

}

void minFilterTrivial(const lti::channel8 &src, lti::channel8 &dst, const int se_size)
{
    int width = src.columns();
    int height = src.rows();
    int se_mid = (se_size - 1) / 2;
    int limAi, limBi;
    int limAf, limBf;
    for(int j = 0; j < height; j++)
    {
        for(int i = 0; i < width; i++)
        {
            uint8_t min = src[j][i];
            limAi = i - se_mid;
            limAf = i + se_mid;
            for(int a = limAi; a < limAf; a++)
            {
                limBi = j - se_mid;
                limBf = j + se_mid;
                for(int32_t b = limBi; b < limBf; b++)
                {
                    uint8_t value = min;
                    if( (a >= 0) && (a < width) && (b >= 0) && (b < height) )
                        value = src[b][a];
                    if(value < min)
                        min = value;
                }
            }
            dst[j][i] = min;
        }
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
 * Main method
 */
int main(int argc, char* argv[]) 
{

  std::string imgFile;
  parseArgs(argc,argv,imgFile);

  lti::ioImage loader; // used to load an image file

  lti::image imgRgba;
  if (!loader.load(imgFile,imgRgba)) {
    std::cerr << "Could not read " << imgFile << ": "
              << loader.getStatusString()
              << std::endl;
    usage();
    exit(EXIT_FAILURE);
  }

  // Image size
  int width = imgRgba.columns();
  int height = imgRgba.rows();

  // Convert to grayscale
  lti::channel8 gray;
  gray.resize(height, width, 0);
  gray.castFrom(imgRgba);

  #ifdef DISPLAY
  bool theEnd = false;
  lti::viewer2D view("Original Image");
  lti::viewer2D::interaction action;
  view.show(gray);
  lti::ipoint pos;
  do {
        view.waitInteraction(action,pos); // wait for something to happen
        if (action == lti::viewer2D::Closed) { // window closed?
          theEnd = true; // we are ready here!
        } 
      } while(!theEnd);
  theEnd = false;
  #endif

  
  for(int i = 1; i < NUM_POINTS; i++)
  {

    // Apply algorithm;
    lti::channel8 minImg;
    minImg.resize(height, width, 0);
    std::chrono::duration<double> diffA;
    double avgA = 0;
    vector<double> samplesA(NUM_TIME_IT);
    for(int j = 0; j < NUM_TIME_IT; j++)
    {
      system("./clearCache.sh");
      auto startA = std::chrono::high_resolution_clock::now();
      minFilterTrivial(gray, minImg, i * MIN_KERNEL_SIZE);
      auto endA = std::chrono::high_resolution_clock::now();
      diffA = endA - startA;
      samplesA[j] = diffA.count();
      avgA += (1.0 / NUM_TIME_IT) * diffA.count();
    }
    finalTimes[i][0] = avgA;
    cout << "Min Filter Variance = " << getVariance(samplesA, avgA) << endl << endl;

    #ifdef DISPLAY
    lti::viewer2D view2("Min Image");
    lti::viewer2D::interaction action2;
    view2.show(minImg);
    lti::ipoint pos2;
    do {
          view2.waitInteraction(action2,pos2); // wait for something to happen
          if (action2 == lti::viewer2D::Closed) { // window closed?
            theEnd = true; // we are ready here!
          } 
        } while(!theEnd);
    theEnd = false;
    #endif

    lti::channel8 maxImg;
    maxImg.resize(height, width, 0);
    std::chrono::duration<double> diffB;
    double avgB = 0;
    vector<double> samplesB(NUM_TIME_IT);
    for(int j = 0; j < NUM_TIME_IT; j++)
    {
      system("./clearCache.sh");
      auto startB = std::chrono::high_resolution_clock::now();
      maxFilterTrivial(gray, maxImg, i * MIN_KERNEL_SIZE);
      auto endB = std::chrono::high_resolution_clock::now();
      diffB = endB - startB;
      samplesB[j] = diffB.count();
      avgB += (1.0 / NUM_TIME_IT) * diffB.count();
    }
    finalTimes[i][1] = avgB;
    cout << "Max Filter Variance = " << getVariance(samplesB, avgB) << endl << endl;

    #ifdef DISPLAY
    lti::viewer2D view3("Max Image");
    lti::viewer2D::interaction action3;
    view3.show(maxImg);
    lti::ipoint pos3;
    do {
          view3.waitInteraction(action3,pos3); // wait for something to happen
          if (action3 == lti::viewer2D::Closed) { // window closed?
            theEnd = true; // we are ready here!
          } 
        } while(!theEnd);
    theEnd = false;
    #endif
  }

  //Generating Timing Results
  cout << "Generating the Timing Data..." << endl;;
  createData();
  
  return EXIT_SUCCESS;
}

