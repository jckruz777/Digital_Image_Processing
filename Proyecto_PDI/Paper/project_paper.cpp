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
#include <algorithm>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <fstream>
#include <queue>
#include <deque>

using std::cout;
using std::cerr;
using std::endl;

//#define DISPLAY 1          // Show images if un-commented
#define NUM_POINTS  2      // Num of time samples
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


/*
* 1D MaxFilter
*/
pair <uint8_t, bool> OneD_Dilation(int rp, int wp, uint8_t F, int SE1, int SE2, int N, deque< pair <uint8_t, int> > &fifo)
{
  pair <uint8_t, bool> result;

  if(!fifo.empty())
  {
    
    while(fifo.back().first <= F)
    {
      //Dequeue all queued higher/smaller or equal values
      fifo.pop_back();
      if(fifo.empty())
        break;
    }
    
    //Delete too old value
    if(!fifo.empty() && (wp - fifo.front().second > SE1))
      fifo.pop_front(); 
  }

  //Enqueue the current sample
  pair <uint8_t, int> current_sample; 
  current_sample.first = F; 
  current_sample.second = rp;
  fifo.push_back(current_sample);

  if(rp == min(N, wp + SE2))
  {
    result.first = (fifo.front().first);
    result.second = true;
  }
  else
  {
    result.first = 0;
    result.second = false;
  }
  return result;
}

/*
* 1D MinFilter
*/
pair <uint8_t, bool> OneD_Erosion(int rp, int wp, uint8_t F, int SE1, int SE2, int N, deque< pair <uint8_t, int> > &fifo)
{
  pair <uint8_t, bool> result;

  if(!fifo.empty())
  {
    
    while(fifo.back().first >= F)
    {
      //Dequeue all queued higher/smaller or equal values
      fifo.pop_back();
      if(fifo.empty())
        break;
    }
    
    //Delete too old value
    if(!fifo.empty() && (wp - fifo.front().second > SE1))
      fifo.pop_front(); 
  }

  //Enqueue the current sample
  pair <uint8_t, int> current_sample; 
  current_sample.first = F; 
  current_sample.second = rp;
  fifo.push_back(current_sample);

  if(rp == min(N, wp + SE2))
  {
    result.first = (fifo.front().first);
    result.second = true;
  }
  else
  {
    result.first = 0;
    result.second = false;
  }
  return result;
}


/*
* Dilation (MaxFilter)
*/
lti::channel8 TwoD_Dilation(const lti::channel8 &in_stream, int M, int N, int SE1, int SE2, int SE3, int SE4)
{
  lti::channel8 out_stream;
  out_stream.resize(M, N, 0);

  pair <uint8_t, bool> oneDResponsex;
  pair <uint8_t, bool> oneDResponsey;

  const uint8_t PAD = 0;                        //Set the padding constant
  vector< deque< pair<uint8_t, int> > > vfifo;  //Array of N empty FIFOs for the vertical dilation part
  vfifo.resize(N);

  int line_rd = 1;                              //Read line counter
  int line_wr = 1;                              //Written line counter

  int col_rd; 
  int col_wr;

  uint8_t F;
  uint8_t dFx, dFy;

  deque< pair<uint8_t, int> > hfifo;

  //Iterate over all image lines
  while(line_wr < M)
  {
    hfifo.clear();                              //FIFO for the horizontal part
    col_rd = 0;                                 //read column counter
    col_wr = 1;                                 //written column counter

    //Iterate over all columns
    while(col_wr < N)
    {

      //Horizontal operation on the line_wr line
      if(line_rd < M)
      {
        if(col_rd < N)
          F = in_stream[col_rd][line_rd];
        else
          F = PAD;                              //Padding constant
        
        col_rd = min(col_rd + 1, N);
        oneDResponsex = OneD_Dilation(col_rd, col_wr, F, SE1, SE3, N, hfifo);
        if(oneDResponsex.second)
          dFx = oneDResponsex.first;
      }
      else
        dFx = PAD;

      //Vertical operation of the col_wr column
      if(oneDResponsex.second)                   //dFx != {}
      {
        oneDResponsey = OneD_Dilation(min(line_rd, M), line_wr, dFx, SE2, SE4, M, vfifo.at(col_wr)); 
        if(oneDResponsey.second)                 //dFy != {}
        {
          dFy = oneDResponsey.first;  
          out_stream[col_wr][line_wr] = dFy;
        }
        col_wr = col_wr + 1;
      }
    }
    line_rd = line_rd + 1;
    if(oneDResponsey.second)
      line_wr = line_wr + 1;
  }
  return out_stream;
}


/*
* Erosion (MinFilter)
*/
lti::channel8 TwoD_Erosion(const lti::channel8 &in_stream, int M, int N, int SE1, int SE2, int SE3, int SE4)
{
  lti::channel8 out_stream;
  out_stream.resize(M, N, 0);

  pair <uint8_t, bool> oneDResponsex;
  pair <uint8_t, bool> oneDResponsey;

  const uint8_t PAD = 0;                        //Set the padding constant
  vector< deque< pair<uint8_t, int> > > vfifo;  //Array of N empty FIFOs for the vertical dilation part
  vfifo.resize(N);

  int line_rd = 1;                              //Read line counter
  int line_wr = 1;                              //Written line counter

  int col_rd; 
  int col_wr;

  uint8_t F;
  uint8_t dFx, dFy;

  deque< pair<uint8_t, int> > hfifo;

  //Iterate over all image lines
  while(line_wr < M)
  {
    hfifo.clear();                              //FIFO for the horizontal part
    col_rd = 0;                                 //read column counter
    col_wr = 1;                                 //written column counter

    //Iterate over all columns
    while(col_wr < N)
    {

      //Horizontal operation on the line_wr line
      if(line_rd < M)
      {
        if(col_rd < N)
          F = in_stream[col_rd][line_rd];
        else
          F = PAD;                              //Padding constant
        
        col_rd = min(col_rd + 1, N);
        oneDResponsex = OneD_Erosion(col_rd, col_wr, F, SE1, SE3, N, hfifo);
        if(oneDResponsex.second)
          dFx = oneDResponsex.first;
      }
      else
        dFx = PAD;

      //Vertical operation of the col_wr column
      if(oneDResponsex.second)                   //dFx != {}
      {
        oneDResponsey = OneD_Erosion(min(line_rd, M), line_wr, dFx, SE2, SE4, M, vfifo.at(col_wr)); 
        if(oneDResponsey.second)                 //dFy != {}
        {
          dFy = oneDResponsey.first;  
          out_stream[col_wr][line_wr] = dFy;
        }
        col_wr = col_wr + 1;
      }
    }
    line_rd = line_rd + 1;
    if(oneDResponsey.second)
      line_wr = line_wr + 1;
  }
  return out_stream;
}


void maxFilterDokladal(const lti::channel8 &src, lti::channel8 &dst, const int se_size)
{
  int width = src.columns();
  int height = src.rows();
  int se_mid = (se_size - 1) / 2;

  dst = TwoD_Dilation(src, height, width, se_mid, se_mid, se_mid, se_mid);
}

void minFilterDokladal(const lti::channel8 &src, lti::channel8 &dst, const int se_size)
{
  int width = src.columns();
  int height = src.rows();
  int se_mid = (se_size - 1) / 2;

  dst = TwoD_Erosion(src, height, width, se_mid, se_mid, se_mid, se_mid);
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
      minFilterDokladal(gray, minImg, i * MIN_KERNEL_SIZE);
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
      maxFilterDokladal(gray, maxImg, i * MIN_KERNEL_SIZE);
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
  cout << "Generating the Timing Data..." << endl;
  createData();
  
  return EXIT_SUCCESS;
}

