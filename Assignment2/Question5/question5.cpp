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
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;

// See www.asciitable.com
#define ESCAPE_KEY (27)
#define SYSTEM_ERROR (-1)

#define HEIGHT 480
#define WIDTH  320

#define XPOS1   50
#define YPOS1   50
#define XPOS2   800
#define YPOS2   600

void CannyEdgeDetection(Mat frame, int threshold1, int threshold2);
void SobelEdgeDetection(Mat frame);

int main()
{

   VideoCapture cam0(0);
   //namedWindow("video_display");
   char winInput;

   if (!cam0.isOpened())
   {
       exit(SYSTEM_ERROR);
   }

   int threshold1 = 10;

   int threshold2 = 10;

   cam0.set(CAP_PROP_FRAME_WIDTH, 640);
   cam0.set(CAP_PROP_FRAME_HEIGHT, 480);

   char currentInput = 0;
   char previousInput = 0;

   char checkInput;
   bool valueUpdated = false;

   int currentMode = 0;
   char noInput;

   currentInput = 'n';

   while (1)
   {
      Mat frame;
      cam0.read(frame);

      if(currentInput == 'c')
      {
          currentMode = 1;
      }
      else if(currentInput == 's')
      {
          currentMode = 2;
      }
      else if(currentInput == 'n')
      {
          currentMode = 3;
      }
      else if(currentInput == ESCAPE_KEY)
      {
          break;
      }

      if(currentMode == 1)
      {
          //destroyAllWindows();
          CannyEdgeDetection(frame, threshold1, threshold2);
      }
      else if(currentMode == 2)
      {
        //destroyAllWindows();
	    SobelEdgeDetection(frame);
      }
      else if(currentMode == 3)
      {
          imshow("Input", frame);
          moveWindow("Input" , XPOS1,YPOS1);
          imshow("Output", frame);
          moveWindow("Output" , XPOS2,YPOS1);
      }

      currentInput = waitKey(1000);
      
   }

   destroyAllWindows();
   cam0.release();
   return 0;



   
};

void CannyEdgeDetection(Mat frame, int threshold1, int threshold2)
{
      //Mat frame;
      Mat img_gray;
      Mat img_blur;
      Mat canny_frame;

      //cam0.read(frame);
      //imshow("Original", frame);

      cv::cvtColor(frame , img_gray , cv::COLOR_BGR2GRAY);
      cv::GaussianBlur(img_gray , img_blur, Size(3,3), 0);

      cv::Canny(img_blur, canny_frame , threshold1 , threshold2 , 3);

    //   imshow("OriginalCanny", frame);
    //   moveWindow("OriginalCanny" , XPOS1,YPOS1);
    //   imshow("Canny", canny_frame);
    //   moveWindow("Canny" , XPOS2,YPOS1);


      imshow("Input", frame);
      moveWindow("Input" , XPOS1,YPOS1);
      imshow("Output", canny_frame);
      moveWindow("Output" , XPOS2,YPOS1);
}


void SobelEdgeDetection(Mat frame)
{
      //Mat frame;
      Mat img_gray;
      Mat img_blur;
      Mat sobelxy;
      Mat sobelx;
      Mat sobely;
      Mat grad;
      //cam0.read(frame);

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



    //   imshow("OriginalSobel", frame);
    //   moveWindow("OriginalSobel" , XPOS1,YPOS1);
    //   imshow("X", abs_grad_x);
    //   moveWindow("X" , XPOS2,YPOS1);
    //   imshow("Y", abs_grad_y);
    //   moveWindow("Y" , XPOS1,YPOS2);
    //   imshow("Sobel XY", grad);
    //   moveWindow("Sobel XY" , XPOS2,YPOS2);

      imshow("Input", frame);
      moveWindow("Input" , XPOS1,YPOS1);
      imshow("Output", grad);
      moveWindow("Output" , XPOS2,YPOS1);
}
