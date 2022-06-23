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


#include <time.h>
#include <syslog.h>

using namespace cv;
using namespace std;

#define NSEC_PER_SEC (1000000000)
#define NSEC_PER_MSEC (1000000)
#define NSEC_PER_USEC (1000)
#define ERROR (-1)
#define OK (0)
#define TEST_SECONDS (0)
#define TEST_NANOSECONDS (NSEC_PER_MSEC * 10)

// See www.asciitable.com
#define ESCAPE_KEY (27)
#define SYSTEM_ERROR (-1)

#define HEIGHT 480
#define WIDTH  320

#define XPOS1   50
#define YPOS1   50
#define XPOS2   800
#define YPOS2   600

int lowThreshold = 0;
Mat frame;

void CannyEdgeDetection(int, void*);
void SobelEdgeDetection(Mat frame);
int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t);

int main(int argc, char **argv)
{

    CommandLineParser parser(argc, argv, "{operation|None|Sobel Or Canny}");

    parser.about( "\nThis program performs Timing Analysis of Canny or Sobel Edge Detection");
    parser.printMessage();

   VideoCapture cam0(0);
   //namedWindow("video_display");
   char winInput;

   if (!cam0.isOpened())
   {
       exit(SYSTEM_ERROR);
   }

   int threshold1 = 50;
   int threshold2 = 50;

   cam0.set(CAP_PROP_FRAME_WIDTH, 640);
   cam0.set(CAP_PROP_FRAME_HEIGHT, 480);

   string operation = parser.get<string>("operation");

   struct timespec startTime;
   struct timespec endTime;

   bool retVal = false;
   int count = 0;
   struct timespec diffTime;

    if(operation == "Canny")
    {
        namedWindow("Input", WINDOW_AUTOSIZE);
        createTrackbar("Min Threshold" , "Input", &lowThreshold , 100, CannyEdgeDetection);
    }
   clock_gettime(CLOCK_REALTIME , &startTime);
   while (count < 1800)
   {
      
      retVal = cam0.read(frame);

      if(retVal == true)
      {
          count++;
      }

      if(operation == "Sobel")
      {
          SobelEdgeDetection(frame);
      }
      else if(operation == "Canny")
      {
          CannyEdgeDetection(0,0);
      }

    winInput = waitKey(10);

    if(winInput == ESCAPE_KEY)
    {
        break;
    }

      
   }
   clock_gettime(CLOCK_REALTIME , &endTime);
    delta_t(&endTime, &startTime, &diffTime);
    printf("Difference Seconds = %ld Milliseconds = %ld\n", diffTime.tv_sec , diffTime.tv_nsec / NSEC_PER_MSEC);
    int total_time_in_ms = (diffTime.tv_sec * 1000) + (diffTime.tv_nsec / NSEC_PER_MSEC);
    int average_time_in_ms = total_time_in_ms / count;
    syslog(LOG_CRIT , "Difference of Capture sec = %ld , msec = %ld\n", diffTime.tv_sec , diffTime.tv_nsec / NSEC_PER_MSEC);
    syslog(LOG_CRIT , "Average time per frame in ms = %ld\n", average_time_in_ms);

   destroyAllWindows();
   cam0.release();
   return 0;



   
};

void CannyEdgeDetection(int, void*)
{
      //Mat frame;
      Mat img_gray;
      Mat img_blur;
      Mat canny_frame;

      //cam0.read(frame);
      //imshow("Original", frame);

      cv::cvtColor(frame , img_gray , cv::COLOR_BGR2GRAY);
      cv::GaussianBlur(img_gray , img_blur, Size(3,3), 0);

      cv::Canny(img_blur, canny_frame , lowThreshold , lowThreshold*3 , 3);

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


#if 1
int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t)
{
  int dt_sec=stop->tv_sec - start->tv_sec;
  int dt_nsec=stop->tv_nsec - start->tv_nsec;

  //printf("\ndt calcuation\n");

  // case 1 - less than a second of change
  if(dt_sec == 0)
  {
	  //printf("dt less than 1 second\n");

	  if(dt_nsec >= 0 && dt_nsec < NSEC_PER_SEC)
	  {
	          //printf("nanosec greater at stop than start\n");
		  delta_t->tv_sec = 0;
		  delta_t->tv_nsec = dt_nsec;
	  }

	  else if(dt_nsec > NSEC_PER_SEC)
	  {
	          //printf("nanosec overflow\n");
		  delta_t->tv_sec = 1;
		  delta_t->tv_nsec = dt_nsec-NSEC_PER_SEC;
	  }

	  else // dt_nsec < 0 means stop is earlier than start
	  {
	         printf("stop is earlier than start\n");
		 return(ERROR);  
	  }
  }

  // case 2 - more than a second of change, check for roll-over
  else if(dt_sec > 0)
  {
	  //printf("dt more than 1 second\n");

	  if(dt_nsec >= 0 && dt_nsec < NSEC_PER_SEC)
	  {
	          //printf("nanosec greater at stop than start\n");
		  delta_t->tv_sec = dt_sec;
		  delta_t->tv_nsec = dt_nsec;
	  }

	  else if(dt_nsec > NSEC_PER_SEC)
	  {
	          //printf("nanosec overflow\n");
		  delta_t->tv_sec = delta_t->tv_sec + 1;
		  delta_t->tv_nsec = dt_nsec-NSEC_PER_SEC;
	  }

	  else // dt_nsec < 0 means roll over
	  {
	          //printf("nanosec roll over\n");
		  delta_t->tv_sec = dt_sec-1;
		  delta_t->tv_nsec = NSEC_PER_SEC + dt_nsec;
	  }
  }

  return(OK);
}
#endif