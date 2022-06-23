/*
 *
 *  Example by Sam Siewert 
 *
 *  Created for OpenCV 4.x for Jetson Nano 2g, based upon
 *  https://docs.opencv.org/4.1.1
 *
 *  Tested with JetPack 4.6 which installs OpenCV 4.1.1
 *  (https://developer.nvidia.com/embedded/jetpack)
 *
 *  Based upon earlier simpler-capture examples created
 *  for OpenCV 2.x and 3.x (C and C++ mixed API) which show
 *  how to use OpenCV instead of lower level V4L2 API for the
 *  Linux UVC driver.
 *
 *  Verify your hardware and OS configuration with:
 *  1) lsusb
 *  2) ls -l /dev/video*
 *  3) dmesg | grep UVC
 *
 *  Note that OpenCV 4.x only supports the C++ API
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


#define HEIGHT 480
#define WIDTH  320

#define XPOS1   50
#define YPOS1   50
#define XPOS2   600
#define YPOS2   400

using namespace cv;
using namespace std;

// See www.asciitable.com
#define ESCAPE_KEY (27)
#define SYSTEM_ERROR (-1)

int main()
{
//    VideoCapture cam0(0);

//    if (!cam0.isOpened())
//    {
//        exit(SYSTEM_ERROR);
//    }

   char winInput;
   namedWindow("Original");
   namedWindow("X");
   namedWindow("Y");
   namedWindow("Sobel XY");

   cv::resizeWindow("Original", HEIGHT,WIDTH);
   cv::resizeWindow("X", HEIGHT,WIDTH);
   cv::resizeWindow("Y", HEIGHT,WIDTH);
   cv::resizeWindow("Sobel XY", HEIGHT,WIDTH);
   std::string image_path = samples::findFile("100.jpeg");

   while (1)
   {
      Mat frame;
      Mat img_gray;
      Mat img_blur;
      Mat sobelxy;
      Mat sobelx;
      Mat sobely;
      Mat grad;
      //cam0.read(frame);

      frame = cv::imread(image_path , IMREAD_COLOR);

      cv::cvtColor(frame , img_gray , cv::COLOR_BGR2GRAY);
      cv::GaussianBlur(img_gray , img_blur, Size(3,3), 0);
      //cv::Sobel(img_blur, sobelx, CV_64F, 1, 0, 5);
      //cv::Sobel(img_blur, sobely, CV_64F, 0, 1, 5);

      Mat grad_x, grad_y;
      Mat abs_grad_x, abs_grad_y;
      Sobel(img_gray, grad_x, CV_16S, 1, 0, 1, 1, 0, BORDER_DEFAULT);
      Sobel(img_gray, grad_y, CV_16S, 0, 1, 1, 1, 0, BORDER_DEFAULT);
      // converting back to CV_8U
      convertScaleAbs(grad_x, abs_grad_x);
      convertScaleAbs(grad_y, abs_grad_y);
      addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);


      //Mat resizedImage;
      resize(frame, frame, Size(HEIGHT,WIDTH) , INTER_LINEAR);
      imshow("Original" , frame);
      moveWindow("Original" , XPOS1,YPOS1);
      resize(abs_grad_x, abs_grad_x, Size(HEIGHT,WIDTH) , INTER_LINEAR);
      imshow("X" , abs_grad_x);
      moveWindow("X" , XPOS2,YPOS1);
      resize(abs_grad_y, abs_grad_y, Size(HEIGHT,WIDTH) , INTER_LINEAR);
      imshow("Y" , abs_grad_y);
      moveWindow("Y" , XPOS1,YPOS2);
      resize(grad, grad, Size(HEIGHT,WIDTH) , INTER_LINEAR);
      imshow("Sobel XY" , grad);
      moveWindow("Sobel XY" , XPOS2,YPOS2);
      


      if ((winInput = waitKey(10)) == ESCAPE_KEY)
      {
          destroyAllWindows();
          break;
      }
      
   }

   return 0;

   
};
