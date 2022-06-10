#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>

#include <unistd.h>

using namespace std;
using namespace cv;
void drawOnImage(Mat frame, int height, int width);
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

RNG rng(12345);
int main( int argc, const char** argv )
{
    /*For now used the default path of the cascade clasifiers*/
    CommandLineParser parser(argc, argv,
                             "{help h||}"
                             "{face_cascade|haarcascade_frontalface_alt.xml|Path to face cascade.}"
                             "{eyes_cascade|haarcascade_eye_tree_eyeglasses.xml|Path to eyes cascade.}"
                             "{camera|0|Camera device number.}");
    parser.about( "\nThis program demonstrates using the cv::CascadeClassifier class to detect objects (Face + eyes) in a video stream.\n"
                  "You can use Haar or LBP features.\n\n" );
    parser.printMessage();
    String face_cascade_name = samples::findFile( parser.get<String>("face_cascade") );
    String eyes_cascade_name = samples::findFile( parser.get<String>("eyes_cascade") );
    //-- 1. Load the cascades
    if( !face_cascade.load( face_cascade_name ) )
    {
        cout << "--(!)Error loading face cascade\n";
        return -1;
    };
    if( !eyes_cascade.load( eyes_cascade_name ) )
    {
        cout << "--(!)Error loading eyes cascade\n";
        return -1;
    };

    Mat frame;



    int camera_device = parser.get<int>("camera");
    VideoCapture capture;
    //-- 2. Read the video stream
    capture.open( camera_device );

    if ( ! capture.isOpened() )
    {
        cout << "--(!)Error opening video capture\n";
        return -1;
    }


    int width;
    int height;
    


    while ( capture.read(frame) )
    {
        if( frame.empty() )
        {
            cout << "--(!) No captured frame -- Break!\n";
            break;
        }

        width = capture.get(3);
        height = capture.get(4);
        //-- 3. Apply the classifier to the frame
        drawOnImage( frame , height, width);
        if( waitKey(10) == 27 )
        {
            break; // escape
        }

        usleep(500);
    }

    return 0;
}

void drawOnImage(Mat frame, int height, int width)
{
    //imshow( "Base Image", frame );

    Mat borderOutput;
    Mat resizedImage;

    int newWidth = width / 2;
    int newHeight = height / 2;

    newWidth = 320 / 2;
    newHeight = 240 / 2;

    //cout << "Width " << newWidth << "Height " << newHeight << endl;

    Scalar value(rng.uniform(0,255), rng.uniform(0,0), rng.uniform(0,0));
    Scalar color(0, 255, 255);

    resize(frame, resizedImage, Size(320,240) , INTER_LINEAR);
    imshow( "Base Image", resizedImage );

    copyMakeBorder(resizedImage, borderOutput, 4,4,4,4, cv::BORDER_CONSTANT, value);

    drawMarker(borderOutput, Point(newWidth,newHeight) , color, MARKER_CROSS, 20,1,8);

    imshow("Processed Image", borderOutput);
}
