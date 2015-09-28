#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "Table.hpp"
#include "Balls.hpp"
#include "Matcher.hpp"

using namespace cv;
using namespace std;

class LoggedPosition{
	public:
		cv::Point2i  pos;
		long frame;
		
		LoggedPosition( cv::Point2i position , long frameCnt ){
			pos = position;
			frame = frameCnt;
		}
};

int main( int argc, char** argv )
{
	bool showROI = true;
	bool whiteMoved = false;
	Mat whiteTempl;
	Mat redTempl;
	Mat yellowTempl;
	Mat greenTempl;
	Mat brownTempl;
	Mat blueTempl;
	Mat pinkTempl;
	Mat blackTempl;
    Mat image;
    //image = imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    cv::VideoCapture cap(argv[1]);
    cap >> image; // get a new frame from camera
    //image = imread("../data/Vegas1.jpg");
    Table t(image);
    std::cout << "Table segmented" << std::endl;
    cv::Mat templ = imread("../data/blackPattern.png");
    cv::Mat result;
    Balls b;
    b.setTable(t);
    Matcher m;
    int cnt = 0;
    cv::Point2i lastPos;
    std::vector<LoggedPosition> whitePositions;
    long frameCnt = 0;
    //speed calculation
    float dt = 1.0 / cap.get(CV_CAP_PROP_FPS);
    int videoLength = cap.get(CV_CAP_PROP_FRAME_COUNT);
    cv::Mat speedImage( 500, videoLength*5, CV_8UC1, cv::Scalar( 0 ) );
    for(;;)
    {
    	frameCnt++;
        cap >> image; // get a new frame from camera
        cv::Point2i whitePos = b.getWhitePos(image);
        
        
        cv::Mat maskROI = t.mask(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
		cv::Mat imageROI = image(cv::Rect(t.bl.x,t.ul.y,(t.br.x-t.bl.x),(t.br.y-t.ur.y)));
		cv::multiply(imageROI,maskROI,imageROI);
		std::vector<cv::Point2i> whiteAreas = b.getWhiteAreas(image);
		cv::Mat dst;
		//cv::medianBlur ( imageROI, dst, 7 );
		//cv::GaussianBlur(imageROI,dst, cv::Size(7,7),0,0);
        for( int i=0;i<whiteAreas.size();i++ ){
			int color = b.getBallColor(imageROI,whiteAreas[i]);
        	cv::circle(imageROI,whiteAreas[i],3,cv::Scalar(Balls::colorTable[color][0],Balls::colorTable[color][1],Balls::colorTable[color][2]),2);
        }
		if(showROI){
			showROI = false;
			//b.findTemplates(imageROI,whiteTempl,redTempl,yellowTempl,greenTempl,brownTempl,blueTempl,pinkTempl,blackTempl);
			//cv::imshow("edges", imageROI);
        	//cv::waitKey(0);
        	//cv::matchTemplate(imageROI,templ,result, CV_TM_CCOEFF_NORMED);
        	//normalize( result, result, 0, 255, NORM_MINMAX, -1, Mat() );
        	//double minVal, maxVal;
  			//cv::Point2i  minLoc, maxLoc, matchLoc;
			//cv::minMaxLoc( result, &minVal, &maxVal, &minLoc, &maxLoc, Mat() );
			//cv::inRange(result, cv::Scalar(0.99*maxVal), cv::Scalar(maxVal), result);
        	//cv::imshow("edges", result);
        	//cv::waitKey(0);
		}
		if(lastPos.x == 0){
			lastPos = whitePos;
			whitePositions.push_back(LoggedPosition(whitePos,frameCnt));
		}
		if( Matcher::pointDistLargerThan(whitePos,lastPos,2) ){
			whitePositions.push_back(LoggedPosition(whitePos,frameCnt));
			whiteMoved = true;
			lastPos = whitePos;
			std::cout << frameCnt << ": " << whitePositions[whitePositions.size()-1].pos << std::endl;
		}else if(whiteMoved){
			whitePositions.push_back(LoggedPosition(whitePos,frameCnt));
			lastPos = whitePos;
			std::cout << frameCnt << ": " << whitePositions[whitePositions.size()-1].pos << std::endl;
		}
		if(whitePositions.size() > 3){
			//b.filterNearPoints(whiteAreas);
			for( int iw=0;iw<whiteAreas.size();iw++ ){
				if(Matcher::pointDistanceFromLine(whitePositions[0].pos,whitePositions[whitePositions.size()-1].pos,whiteAreas[iw]) < 5){
					cv::circle(imageROI,whiteAreas[iw],3,cv::Scalar(255,255,255),2);
				}else{
				//	cv::circle(imageROI,whiteAreas[iw],3,cv::Scalar(0,0,0),2);
				}
				//cv::imshow("edges", imageROI);
        		//cv::waitKey(0);
			}
			//Matcher::pointDistanceFromLine();
		}
		if(whitePos.x == 0){
			//cv::imshow("edges", imageROI);
        	//cv::waitKey(0);
			//cv::inRange(imageROI, cv::Scalar(140, 140, 140), cv::Scalar(255, 255, 255), imageROI);
			//std::vector<cv::Point2i> whiteAreas = b.getWhiteAreas(image);
			/*for( std::vector<cv::Point2i>::iterator it = whiteAreas.begin() ; it<whiteAreas.end(); it++ ){
				cv::circle(imageROI,*it,5,cv::Scalar(128,128,128),3);
			}*/
			//if(cnt < 3){
			//	m.store(whiteAreas);
			//	cnt++;
			//}else{
			//	m.store(whiteAreas);
			//	std::vector<std::vector<cv::Point2i>> temp = m.getUnmatchingPoints();
			//	std::vector<cv::Point2i> matchings = m.getCollinearPointsToInitialPos(temp,lastPos); 
			//	for( int i=0;i<temp.size();i++){
			//		for( int j=0;j<temp[i].size();j++ ){
			//			cv::circle(imageROI,temp[i][j],5,cv::Scalar(128,128,128),3);
			//		}
			//	}
			//	for( int i=0;i<matchings.size();i++ ){
			//		cv::circle(imageROI,matchings[i],5,cv::Scalar(200,200,200),3);
			//	}
			//	m.reset();
			//	cnt = 0;
			//}
    		//cv::imshow("edges", imageROI);
        	//cv::waitKey(0);
    	}else{
    		//lastPos = whitePos;
    		//cv::inRange(imageROI, cv::Scalar(200, 240, 200), cv::Scalar(255, 255, 255), imageROI);
    		cv::circle(imageROI,whitePos,5,cv::Scalar(0,0,200),3);
    	}
    	for( int w=0;w<whitePositions.size();w++){
    		cv::circle(imageROI,whitePositions[w].pos,3,cv::Scalar(255,255,255),2);
    	}
    	float prevSpeed = -1;
    	for( int w=1;w<whitePositions.size();w++){
    		cv::Point2i p1 = t.rectify( cv::Point2i(t.bl.x,t.ul.y)+whitePositions[w].pos );
    		cv::Point2i p2 = t.rectify( cv::Point2i(t.bl.x,t.ul.y)+whitePositions[w-1].pos );
    		cv::Point2i diff = p1-p2;
    		float length = (sqrt(diff.x*diff.x + diff.y*diff.y));//*t.PIXEL_TO_CM;
    		if(whitePositions[w].frame == whitePositions[w-1].frame){
    			std::cerr << "Two white position in single frame\n";
    			break;
    		}
    		float speed = length / (whitePositions[w].frame-whitePositions[w-1].frame)*dt;
    		speed*=500;
    		if(prevSpeed == -1){
    			prevSpeed = speed;
    		}
    		//std::cout << "p1: " << p1 << "\tp2: " << p2 << "\tdiff: " << diff << "\tlength: " << length << "\tspeed: " << speed << std::endl;
    		line( speedImage, cv::Point( 5*(w-1) , 500-prevSpeed ) , cv::Point( 5*w , 500-speed ) , cv::Scalar( 255 ), 2, 8, 0  );
    		prevSpeed = speed;
    	}
    	cv::imshow("speed", speedImage);
		cv::imshow("edges", imageROI);
		char key;
        if( (key = cv::waitKey(10)) == 's'){
        	cv::waitKey(0);
        }else if(key == 'q'){ 
        	break;
        }
    }
    

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    return 0;
}


/* Equilaze hist
 cv::Mat channel[3];
    	cv::split(image, channel);
    	//cv::equalizeHist( channel[0], channel[0] );
    	cv::equalizeHist( channel[1], channel[1] );
    	//cv::equalizeHist( channel[2], channel[2] );
    	cv::merge(channel,3,image);

*/


/* draw histogram
		//#############
	cv::equalizeHist( maskedROI, maskedROI );
		cv::imshow("grayscale", maskedROI);
	cv::waitKey(0);
	int histSize = 256;
	float range[] = { 0, 256 } ;
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;
	cv::Mat hist;
	cv::calcHist( &maskedROI, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate );
	int hist_w = 512; int hist_h = 400;
  	int bin_w = round( (double) hist_w/histSize );
  	cv::Mat histImage( hist_h, hist_w, CV_8UC1, cv::Scalar( 0 ) );
  	cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
  	hist.at<uint8_t>(0) = 0;
  	hist.at<uint8_t>(1) = 0;
  	hist.at<uint8_t>(2) = 0;
  	hist.at<uint8_t>(3) = 0;
  	hist.at<uint8_t>(4) = 0;
  	hist.at<uint8_t>(5) = 0;
  	for( int i = 1; i < histSize; i++ ){
      	line( histImage, cv::Point( bin_w*(i-1), hist_h - round(hist.at<float>(i-1)) ) ,
                       cv::Point( bin_w*(i), hist_h - round(hist.at<float>(i)) ),
                       cv::Scalar( 255 ), 2, 8, 0  );
  	}
  	cv::imshow("grayscale", histImage);
	cv::waitKey(0);
	//#############
*/




