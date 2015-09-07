#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "Table.hpp"
#include "Balls.hpp"
#include "Matcher.hpp"

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
    Matcher m;
    int cnt = 0;
    cv::Point2i lastPos;
    for(;;)
    {
        cap >> image; // get a new frame from camera
        cv::Point2i whitePos = b.getWhitePos(image,t);
        cv::Mat maskROI = t.mask(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
		cv::Mat imageROI = image(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
		cv::multiply(imageROI,maskROI,imageROI);
		if(whitePos.x == 0){
			cv::inRange(imageROI, cv::Scalar(140, 140, 140), cv::Scalar(255, 255, 255), imageROI);
			std::vector<cv::Point2i> whiteAreas = b.getWhiteAreas(image,t);
			/*for( std::vector<cv::Point2i>::iterator it = whiteAreas.begin() ; it<whiteAreas.end(); it++ ){
				cv::circle(imageROI,*it,5,cv::Scalar(128,128,128),3);
			}*/
			if(cnt < 3){
				m.store(whiteAreas);
				cnt++;
			}else{
				m.store(whiteAreas);
				std::vector<std::vector<cv::Point2i>> temp = m.getUnmatchingPoints();
				std::vector<cv::Point2i> matchings = m.getCollinearPointsToInitialPos(temp,lastPos); 
				for( int i=0;i<temp.size();i++){
					for( int j=0;j<temp[i].size();j++ ){
						cv::circle(imageROI,temp[i][j],5,cv::Scalar(128,128,128),3);
					}
				}
				for( int i=0;i<matchings.size();i++ ){
					cv::circle(imageROI,matchings[i],5,cv::Scalar(200,200,200),3);
				}
				m.reset();
				cnt = 0;
			}
    		cv::imshow("edges", imageROI);
        	cv::waitKey(0);
    	}else{
    		lastPos = whitePos;
    		cv::inRange(imageROI, cv::Scalar(200, 240, 200), cv::Scalar(255, 255, 255), imageROI);
    		cv::circle(imageROI,whitePos,5,cv::Scalar(128,128,128),3);
    	}
		cv::imshow("edges", imageROI);
        if(cv::waitKey(30) >= 0) break;
    }
    

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    return 0;
}
