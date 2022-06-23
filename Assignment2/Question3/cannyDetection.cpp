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

#define HEIGHT 640
#define WIDTH  480

#define XPOS1   50
#define YPOS1   50
#define XPOS2   750
#define YPOS2   400

using namespace cv;
using namespace std;

// See www.asciitable.com
#define ESCAPE_KEY (27)
#define SYSTEM_ERROR (-1)

int main(int argc, char** argv )
{
    CommandLineParser parser(argc, argv,
                             "{t1|10|Threshold Value1}"
                             "{t2|50|Threshold Value2}");

    parser.about( "\nThis program performs Canny Edge Detection");
    parser.printMessage();

    std::string image_path = samples::findFile("200.jpeg");

   namedWindow("Original");
   namedWindow("Canny");
   //namedWindow("Y");
   //namedWindow("Sobel XY");

   cv::resizeWindow("Original", HEIGHT,WIDTH);
   cv::resizeWindow("Canny", HEIGHT,WIDTH);
   //cv::resizeWindow("Y", HEIGHT,WIDTH);
   //cv::resizeWindow("Sobel XY", HEIGHT,WIDTH);

   //VideoCapture cam0(0);
   //namedWindow("video_display");
   char winInput;

  //  if (!cam0.isOpened())
  //  {
  //      exit(SYSTEM_ERROR);
  //  }

   //cout << argv[0] << argv[1] <<  argv[2] << argv[3] << endl;

   cout << argc << endl;

   int threshold1 = parser.get<int>("t1");

   int threshold2 = parser.get<int>("t2");

   cout << "Threshold 1 " << threshold1 << "Threshold 2 " << threshold2 << endl;

   //cam0.set(CAP_PROP_FRAME_WIDTH, 640);
   //cam0.set(CAP_PROP_FRAME_HEIGHT, 480);

   while (1)
   {
      Mat frame;
      Mat img_gray;
      Mat img_blur;
      Mat canny_frame;

      frame = imread(image_path , IMREAD_COLOR);

      //cam0.read(frame);
      imshow("Original", frame);

      cv::cvtColor(frame , img_gray , cv::COLOR_BGR2GRAY);
      cv::GaussianBlur(img_gray , img_blur, Size(3,3), 0);

      cv::Canny(img_blur, canny_frame , threshold1 , threshold2 , 3);

      resize(frame, frame, Size(HEIGHT,WIDTH) , INTER_LINEAR);
      imshow("Original" , frame);
      moveWindow("Original" , XPOS1,YPOS1);
      resize(canny_frame, canny_frame, Size(HEIGHT,WIDTH) , INTER_LINEAR);
      imshow("Canny" , canny_frame);
      moveWindow("Canny" , XPOS2,YPOS1);

      //imshow("Original", frame);
      //imshow("Canny", canny_frame);

      if ((winInput = waitKey(10)) == ESCAPE_KEY)
      {
        destroyAllWindows();
        break;
      }
      else if(winInput == 'n')
      {
	    cout << "input " << winInput << " ignored" << endl;
      }
      
   }

   
};
