#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "Table.hpp"
#include "Balls.hpp"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{

    Mat image;
    //image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    cv::VideoCapture cap(argv[1]);
    cap >> image; // get a new frame from camera
        namedWindow( "Display window", WINDOW_AUTOSIZE );
    cv::imshow("edges", image);
    cv::waitKey(0);
    Table t(image);
    Balls b;
    //b.getWhitePos(image,t);
    for(;;)
    {
        cap >> image; // get a new frame from camera
        cv::Mat maskROI = t.mask(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
		cv::Mat imageROI = image(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
		cv::multiply(imageROI,maskROI,imageROI);
		cv::inRange(imageROI, cv::Scalar(200, 200, 200), cv::Scalar(255, 255, 255), imageROI);
		cv::imshow("edges", imageROI);
        if(cv::waitKey(100) >= 0) break;
    }
    

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    /*
    Table t(image);
    //cv::cvtColor(image, image, cv::COLOR_BGR2GRAY);
    cv::Mat maskROI = t.mask(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
    cv::Mat imageROI = image(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
    cv::multiply(imageROI,maskROI,imageROI);
	cv::inRange(imageROI, cv::Scalar(100, 100, 100), cv::Scalar(255, 255, 255), imageROI);
    imshow( "Display window", imageROI);                   // Show our image inside it.

    waitKey(0);                                          // Wait for a keystroke in the window*/
    return 0;
}
