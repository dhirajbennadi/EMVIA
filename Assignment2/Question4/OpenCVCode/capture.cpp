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

int delta_t(struct timespec *stop, struct timespec *start, struct timespec *delta_t);

int main()
{
   VideoCapture cam0(0);
   //namedWindow("video_display");
   char winInput;

   if (!cam0.isOpened())
   {
       exit(SYSTEM_ERROR);
   }

   cam0.set(CAP_PROP_FRAME_WIDTH, 640);
   cam0.set(CAP_PROP_FRAME_HEIGHT, 480);

   struct timespec startTime;
   struct timespec endTime;

   bool retVal = false;
   int count = 0;

   //int total_time_in_ms = 0;
   //int average_time_in_ms = 0;
   struct timespec diffTime;

   //gettimeofday(&startTime, (struct timezone*) 0);
   clock_gettime(CLOCK_REALTIME , &startTime);
   while (count < 1800)
   {
      Mat frame;
      string name = "frame";
      string number = "";
      string extension = ".jpeg";

      retVal = cam0.read(frame);
      if(retVal == true)
      {
          retVal = false;
          count++;
          number = to_string(count);
          name = name + number + extension;

          imwrite(name, frame, {IMWRITE_JPEG_QUALITY , 50});
          name = "frame";
      }

      //imshow("video_display", frame);

    //   if ((winInput = waitKey(10)) == ESCAPE_KEY)
    //   //if ((winInput = waitKey(0)) == ESCAPE_KEY)
    //   {
    //       break;
    //   }
    //   else if(winInput == 'n')
    //   {
	//     cout << "input " << winInput << " ignored" << endl;
    //   }
      
   }
   clock_gettime(CLOCK_REALTIME , &endTime);

//    if((int)(endTime.tv_nsec - startTime.tv_nsec) / 1000000 > 0)
//    {
//        printf("Capture Difference : Sec = %d Msec = %d\n", (int)(endTime.tv_sec - startTime.tv_sec) , (int)(endTime.tv_nsec - startTime.tv_nsec) / 1000000 );
//        //diffTime.tv_sec = (int)(endTime.tv_sec - startTime.tv_sec) ;
//        //diffTime.tv_sec = (int)(endTime.tv_nsec - startTime.tv_nsec);
//        //total_time_in_ms = (diffTime.tv_sec * 1000) + (diffTime.tv_nsec / NSEC_PER_MSEC);
//        //average_time_in_ms = total_time_in_ms / count;

//    }
//    else
//    {
//        printf("Capture Difference : Sec = %d Msec = %d Msec = %d\n", (int)(endTime.tv_sec - startTime.tv_sec) , (int)(endTime.tv_nsec  / 1000000)
//        , (int)(startTime.tv_nsec  / 1000000) );
//         //diffTime.tv_sec = (int)(endTime.tv_sec - startTime.tv_sec) ;
//         //total_time_in_ms = (diffTime.tv_sec * 1000) + (endTime.tv_nsec / NSEC_PER_MSEC);
//         //average_time_in_ms = total_time_in_ms / count;
//        //diffTime.tv_sec = (int)(endTime.tv_nsec - startTime.tv_nsec);
//    }

    delta_t(&endTime, &startTime, &diffTime);
    printf("Difference Seconds = %ld Milliseconds = %ld\n", diffTime.tv_sec , diffTime.tv_nsec / NSEC_PER_MSEC);
    int total_time_in_ms = (diffTime.tv_sec * 1000) + (diffTime.tv_nsec / NSEC_PER_MSEC);
    int average_time_in_ms = total_time_in_ms / count;
    syslog(LOG_CRIT , "Difference of Capture sec = %ld , msec = %ld\n", diffTime.tv_sec , diffTime.tv_nsec / NSEC_PER_MSEC);
    syslog(LOG_CRIT , "Average time per frame in ms = %ld\n", average_time_in_ms);


   



   //destroyWindow("video_display"); 

   return 0;
};


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